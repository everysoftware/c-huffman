#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "decoding.h"
#include "utils.h"

void ReadExt(FILE* fin, char buffer[FILENAME_MAX]) {
	char c = fgetc(fin);
	size_t i = 0;
	while (c != '|') {
		buffer[i] = c;
		c = fgetc(fin);
		++i;
	}
	buffer[i] = 0;
}

void ReadMeta(FILE* fin, MetaInfo* buffer) {
	fscanf_s(fin, "%zx|%zx|", &buffer->tail, &buffer->freq->unique_symb);
	for (size_t i = 0; i < buffer->freq->unique_symb; ++i)
	{
		unsigned char c;
		size_t cfreq;
		fscanf_s(fin, "%c%zx|", &c, 1, &cfreq);
		buffer->freq->data[c] = cfreq;
		buffer->freq->total_symb += cfreq;
	}
}

bool DecodeAndWrite(FILE* fin, FILE* fout, Node* tree, size_t tail, Stat* stat) {
	stat->read = (size_t)ftell(fin);
	Node* head = tree;
	ShowProgress("Decompressing...", 0, 0);
	while (stat->read < stat->original)
	{
		BIT2CHAR symb = { 0 };
		unsigned mbit[BYTE_SIZE] = { 0 };
		stat->read += fread(&symb.symb, 1, 1, fin);
		mbit[0] = symb.mbit.b1;
		mbit[1] = symb.mbit.b2;
		mbit[2] = symb.mbit.b3;
		mbit[3] = symb.mbit.b4;
		mbit[4] = symb.mbit.b5;
		mbit[5] = symb.mbit.b6;
		mbit[6] = symb.mbit.b7;
		mbit[7] = symb.mbit.b8;
		size_t mbit_pos = 0;
		while (mbit_pos < BYTE_SIZE && (stat->read < stat->original || mbit_pos < tail || tail == 0) && head) {
			if (mbit[mbit_pos++] == 1) {
				head = head->right ? head->right : head;
			}
			else {
				head = head->left ? head->left : head;
			}
			if (head->is_symb) {

				fputc(head->symb, fout);
				++stat->written;
				head = tree;
			}
		}
		ShowProgress("Decompressing...", stat->read, stat->original);
	}
	return stat->written == stat->excepted;
}

bool HuffmanDecompress(const char* path) {
	printf("Original file : %s\n", path);
	FILE* fin = NULL;
	FILE* fout = NULL;
	if (fopen_s(&fin, path, "rb") != 0) {
		return RaiseError("Could not open source file for reading");
	}
	printf("Read meta info...\n");
	char ext[FILENAME_MAX] = { 0 };
	ReadExt(fin, ext);
	char orig_path[FILENAME_MAX] = { 0 };
	if (!GetPathForDecompressed(path, ext, orig_path)) {
		fclose(fin);
		return RaiseError("Incorrect file format");
	}
	printf("Decompressed file will be saved at : %s\n", orig_path);
	if (fopen_s(&fout, orig_path, "wb") != 0) {
		fclose(fin);
		return RaiseError("Could not open destination file for writing");
	}
	FreqTable freq = { 0 };
	MetaInfo meta = { 0 };
	meta.freq = &freq;
	ReadMeta(fin, &meta);
	Node* tree = MakeCodingTable(&freq, NULL);
	printf("Start decompressing...\n");
	Stat stat = { 0 };
	stat.original = GetSizeOfFile(path);
	stat.excepted = freq.total_symb;
	if (DecodeAndWrite(fin, fout, tree, meta.tail, &stat)) {
		system("cls");
		printf("Decompression completed!\n");
		printf("File saved at : %s\n", orig_path);
	}
	else {
		printf("Decompression failed. Written %zu bytes while excepted %zu bytes\n", stat.written, stat.excepted);
	}
	DelTree(tree);
	fclose(fin);
	fclose(fout);
	system("pause");

	return true;
}
