#ifndef UTILS_H
#define UTILS_H

#define DEBUG_MODE true

#define SAMPLES_DIR "samples"
#define COMPRESSED_DIR "compressed"
#define DECOMPRESSED_DIR "decompressed"
#define COMPRESSED_EXT "huf"
#define TIME_BUFFER_SIZE 16
#define ASCII_SIZE 256
#define PROGRESS_STEP 5
#define BYTE_SIZE 8 

#include <stdbool.h>

#include "nodes.h"

typedef union bit2char {
	char symb;
	struct bit {
		unsigned b1 : 1;
		unsigned b2 : 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
		unsigned b8 : 1;
	}mbit;
} BIT2CHAR;

typedef struct FreqTable {
	size_t data[ASCII_SIZE];
	size_t unique_symb;
	size_t total_symb;
} FreqTable;

typedef struct MetaInfo {
	const char* ext;
	size_t tail;
	FreqTable* freq;
} MetaInfo;

typedef struct Stat {
	size_t original;
	size_t excepted;
	size_t meta;
	size_t read;
	size_t written;
} Stat;

size_t GetSizeOfFile(const char* filename);

bool FileExists(const char* path);

const char* ParsePath(const char* path, char* namebuf);

const char* GetPathForCompressed(const char* path, char* buffer);

bool GetPathForDecompressed(const char* path, const char* orig_ext, char* buffer);

void PrintFreqTable(const FreqTable* freq);

char** NewCodingTable();

void DeleteCodingTable(char** table);

void PrintCodingTable(const char* const * table);

bool RaiseError(const char* msg);

void ShowProgress(const char* title, size_t done_bytes, size_t total_bytes);

Node* MakeCodingTable(const FreqTable* freq, char** buffer);

#endif
