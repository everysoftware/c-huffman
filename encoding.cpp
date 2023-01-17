#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "encoding.h"
#include "nodes.h"
#include "utils.h"

void FillByte(BIT2CHAR* symb, const char* str) {
	symb->mbit.b1 = (unsigned)str[0];
	symb->mbit.b2 = (unsigned)str[1];
	symb->mbit.b3 = (unsigned)str[2];
	symb->mbit.b4 = (unsigned)str[3];
	symb->mbit.b5 = (unsigned)str[4];
	symb->mbit.b6 = (unsigned)str[5];
	symb->mbit.b7 = (unsigned)str[6];
	symb->mbit.b8 = (unsigned)str[7];
}

void ShiftLeft(char* str) {
	size_t i = 0;
	while (str[BYTE_SIZE + i]) {
		str[i] = str[BYTE_SIZE + i];
		i++;
	}
	str[i] = 0;
}

void GetFreq(const char* path, FreqTable* buffer) {
	FILE* f = NULL;
	if (fopen_s(&f, path, "rb") != 0) {
		perror("fopen_s");
		exit(EXIT_FAILURE);
	}
	buffer->total_symb = GetSizeOfFile(path);
	for (size_t i = 0; i < buffer->total_symb; ++i) {
		++buffer->data[(unsigned char)fgetc(f)];
	}
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		buffer->unique_symb += (buffer->data[i] > 0);
	}
	fclose(f);
}

size_t WriteMeta(FILE* fo, const MetaInfo* meta) {
	size_t mlen = fprintf(fo, "%s|%c|%zx|", 
		meta->ext, 
		(char)('0' + meta->tail),
		meta->freq->unique_symb);
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		if (meta->freq->data[i] > 0) {
			mlen += fprintf(fo, "%c%zx|", i, meta->freq->data[i]);
		}
	}
	return mlen;
}

bool EncodeAndWrite(FILE* fin, FILE* fout, const char * const * table, Stat* stat) {
	ShowProgress("", 0, 0);
	char str[ASCII_SIZE] = { 0 };
	unsigned char buff[1] = { 0 };
	while (stat->read < stat->original || strlen(str) > 0) {
		while (stat->read < stat->original && strlen(str) < BYTE_SIZE) {
			stat->read += fread(buff, 1, 1, fin);
			strcat_s(str, ASCII_SIZE, table[buff[0]]);
		}
		while (strlen(str) < BYTE_SIZE) {
			strcat_s(str, ASCII_SIZE, "0");
		}
		BIT2CHAR symb;
		FillByte(&symb, str);
		stat->written += fwrite(&symb.symb, 1, 1, fout);
		ShowProgress("Compressing...", stat->read, stat->original);
		ShiftLeft(str);
	}
	return stat->written == stat->excepted;
}

void PrintCompressionStat(const Stat* stat) {
	printf("- Original file : %zu bytes\n", stat->original);
	printf("- Compressed file : %zu bytes\n", stat->excepted + stat->meta);
	printf("- Compressed file = %.2Lf x Original file\n", ((long double)stat->excepted + stat->meta) / stat->original);
}

bool HuffmanCompress(const char* path) {
	printf("Original file : %s\n", path);
	char huf_path[FILENAME_MAX] = { 0 };
	const char* ext = GetPathForCompressed(path, huf_path);
	printf("The compressed file will be located at : %s\n", huf_path);
	FILE* fout = NULL;
	FILE* fin = NULL;
	if (fopen_s(&fin, path, "rb") != 0) {
		return RaiseError("Could not open source file for reading");
	}
	if (fopen_s(&fout, huf_path, "wb") != 0) {
		fclose(fin);
		return RaiseError("Could not open destination file for writing");
	}

	printf("Make a frequency table...\n");
	FreqTable freq = { 0 };
	GetFreq(path, &freq);
	char** table = NewCodingTable();
	DelTree(MakeCodingTable(&freq, table));

	printf("Generate meta info...\n");
	uint64_t len = 0;
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		len += (uint64_t)strlen(table[i]) * freq.data[i];
	}
	unsigned char tail = len % BYTE_SIZE;
	MetaInfo meta = { ext, tail, &freq };

	printf("Write meta info...\n");
	Stat stat = {
		freq.total_symb,
		(size_t)((len - tail) / BYTE_SIZE) + (tail > 0),
		WriteMeta(fout, &meta)
	};

	printf("Start compressing...\n");
	if (EncodeAndWrite(fin, fout, table, &stat)) {
		system("cls");
		printf("Compression completed!\n");
	}
	else {
		printf("[Error] Written %zu bytes while excepted %zu bytes\n", stat.written, stat.excepted);
	}
	printf("File saved at : %s\n", huf_path);
	PrintCompressionStat(&stat);

	DeleteCodingTable(table);
	fclose(fin);
	fclose(fout);
	system("pause");

	return true;
}

