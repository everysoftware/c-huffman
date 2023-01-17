#include <stdio.h>
#include <stdlib.h> // rand
#include <time.h> // time
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>

#include "tests.h"
#include "utils.h"

#define ASCII_SIZE 256
#define MB_SIZE 1048576

void MakeASCIITest() {
	FILE* f = NULL;
	if (fopen_s(&f, "ascii.txt", "wb") != 0) {
		perror("fopen_s");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < ASCII_SIZE; ++i) {
		fputc(i, f);
	}
	fclose(f);
	printf("ASCII test done!");
}

void MakeTextTest(const char* filename, const char* alphabet, uint16_t size_in_MB) {
	srand((unsigned)time(NULL));
	FILE* f = NULL;
	if (fopen_s(&f, filename, "wb") != 0) {
		perror("fopen_s");
		exit(EXIT_FAILURE);
	}
	size_t n = strlen(alphabet);
	uint64_t bytes = (uint64_t)size_in_MB * MB_SIZE;
	for (uint64_t i = 0; i < bytes; ++i) {
		fputc(alphabet[rand() % n], f);
	}
	fclose(f);
	printf("Text test done!");
}

void TestParsePath() {
	const char tests[][FILENAME_MAX] = {
		"C:\\Users\\pravi\\Downloads\\Raspisanie_2022_2023_IMiKN_3_modul_BAK_PI.xlsx",
		"Raspisanie_2022_2023_IMiKN_3_modul_BAK_PI.xlsx",
		".xlsx",
		".",
		"",
		"aboba",
		"aboba.",
		"C:\\first/second.txt",
		"C:/first\\second.txt"
	};
	const size_t k = sizeof tests / sizeof(const char[256]);
	for (size_t i = 0; i < k; ++i) {
		printf("Test %u\n", i);
		printf("\tpath : %s\n", tests[i]);
		char name[FILENAME_MAX] = "";
		const char* ext = ParsePath(tests[i], name);
		printf("\tname : %s (%u)\n", name, strlen(name));
		printf("\text : %s (%u)\n", ext, strlen(ext));
		char compressed[FILENAME_MAX] = "";
		GetPathForCompressed(tests[i], compressed);
		printf("\tcompressed path : %s (%u)\n", compressed, strlen(compressed));
	}
}
