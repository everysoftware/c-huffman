#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "utils.h"
#include "nodes.h"

size_t GetSizeOfFile(const char* filename) {
	FILE* f = NULL;
	if (fopen_s(&f, filename, "rb") != 0) {
		perror("fopen_s");
		exit(EXIT_FAILURE);
	}
	fseek(f, 0L, SEEK_END);
	long flen = ftell(f);
	if (flen == -1) {
		perror("ftell");
		exit(EXIT_FAILURE);
	}
	fclose(f);
	return (size_t)flen;
}

bool FileExists(const char* path) {
	FILE* f = NULL;
	if (fopen_s(&f, path, "r") != 0) {
		return false;
	}
	fclose(f);
	return true;
}

void GetTime(char* buffer) {
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	sprintf_s(buffer, TIME_BUFFER_SIZE, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min,
		timeinfo.tm_sec);
}

/*
Парсит базовое имя файла и его расширение из пути.
Возвращает расширение, а имя заносит в буфер namebuf
*/
const char* ParsePath(const char* path, char* namebuf) {
	const char* pext = strrchr(path, '.');
	pext = pext == NULL ? "" : pext;
	const char* name_ls = strrchr(path, '\\');
	const char* name_rs = strrchr(path, '/');
	const char* name = name_ls > name_rs ? name_ls : name_rs;
	name = name == NULL ? path : (name + 1);
	strncpy_s(namebuf, FILENAME_MAX, name, strlen(name) - strlen(pext));
	return pext + 1;
}

/*
Получить путь до сжатого файла в buffer, вернуть расширение исходного файла
C:/.../file.ext -> compressed/file.huf
*/
const char* GetPathForCompressed(const char* path, char* buffer) {
	char name[FILENAME_MAX] = "";
	const char* ext = ParsePath(path, name);
	sprintf_s(buffer, FILENAME_MAX, "%s/%s.%s", COMPRESSED_DIR, name, COMPRESSED_EXT);
	return ext;
}
/*
Получить путь до распакованного файла
C:/.../file.huf -> decompressed/file.orig_ext
*/
bool GetPathForDecompressed(const char* path, const char* orig_ext, char* buffer) {
	char name[FILENAME_MAX] = "";
	const char* ext = ParsePath(path, name);
	if (strcmp(ext, COMPRESSED_EXT) != 0) {
		return false;
	}
	sprintf_s(buffer, FILENAME_MAX, "%s/%s.%s", DECOMPRESSED_DIR, name, orig_ext);
	return ext;
}

void PrintFreqTable(const FreqTable* freq) {
	if (!DEBUG_MODE) {
		return;
	}
	printf("Frequency table\n");
	printf("%u total symbols\n", freq->total_symb);
	printf("%u unique symbols\n", freq->unique_symb);
	printf("Content:\n");
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		printf("%c - %u\t", i, freq->data[i]);
	}
	printf("\n");
}

char** NewCodingTable() {
	char** temp = (char**)calloc(ASCII_SIZE, sizeof(char*));
	if (temp == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		temp[i] = (char*)calloc(ASCII_SIZE, sizeof(char));
		if (temp[i] == NULL) {
			perror("calloc");
			exit(EXIT_FAILURE);
		}
	}
	return temp;
}

void DeleteCodingTable(char** table) {
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		free(table[i]);
	}
	free(table);
}

void PrintCodingTable(const char * const * table) {
	if (!DEBUG_MODE) {
		return;
	}
	printf("Coding table\n");
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		if (table[i][0] != '\0') {
			printf("%c (%u) - %s\n", i, i, table[i]);
		}
	}
}

bool RaiseError(const char* msg) {
	printf("[Error] %s\n", msg);
	system("pause");
	return true;
}

void ShowProgress(const char* title, size_t done_bytes, size_t total_bytes) {
	static double last_progress;
	if (done_bytes == 0) {
		last_progress = 0;
	}
	double progress = (double)done_bytes / total_bytes * 100;
	if (progress - last_progress >= PROGRESS_STEP)
	{
		char timebuf[TIME_BUFFER_SIZE] = "";
		GetTime(timebuf);
		printf("(%s) %s %zu%%\n", timebuf, title, (size_t)progress);
		last_progress = progress;
	}
}

Node* MakeCodingTable(const FreqTable* freq, char** buffer) {
	printf("Make a coding table...\n");
	Node* phead = NULL;
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		if (freq->data[i] > 0) {
			Add2List(&phead, (unsigned char)i, freq->data[i]);
		}
	}
	Node* tree = MakeTreeFromList(phead);
	if (freq->unique_symb == 1) {
		strcpy_s(tree->code, ASCII_SIZE, "0");
	}
	SimmetricCode(tree);
	TreeCodesToBuffer(tree, 0, buffer);
	return tree;
}
