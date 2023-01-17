#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <crtdbg.h>
#include <stdbool.h>
#include <conio.h>

#include "tests.h"
#include "utils.h"
#include "nodes.h"
#include "encoding.h"
#include "decoding.h"

void PrintMenu() {
	printf("...... Huffman's algorithm ......\n");
	printf("1. Compress file\n");
	printf("2. Decompress file\n");
	printf("3. Exit\n");
}

bool CompressMenu() {
	system("cls");
	printf("Menu -> Compress file\n");
	char path[FILENAME_MAX] = { 0 };
	printf("Enter 'return' to return to the menu\n");
	printf("The path to the file :\n> ");
	gets_s(path, FILENAME_MAX);
	while (!FileExists(path) && strcmp(path, "return") != 0) {
		printf("[Error] No such file\n");
		printf("The path to the file :\n> ");
		gets_s(path, FILENAME_MAX);
	}
	if (strcmp(path, "return") == 0) {
		return true;
	}
	system("cls");
	HuffmanCompress(path);
	if (DEBUG_MODE) {
		printf("Start a decompressing test...\n");
		char temp[FILENAME_MAX] = { 0 };
		GetPathForCompressed(path, temp);
		return HuffmanDecompress(temp);
	}
	return true;
}

bool DecompressMenu() {
	system("cls");
	printf("Menu -> Decompress file\n");
	printf("Enter 'return' to return to the menu\n");
	char path[FILENAME_MAX] = { 0 };
	printf("The path to the file :\n> ");
	gets_s(path, FILENAME_MAX);
	while (!FileExists(path) && strcmp(path, "return") != 0) {
		printf("[Error] No such file\n");
		printf("The path to the file :\n> ");
		gets_s(path, FILENAME_MAX);
	}
	if (strcmp(path, "return") == 0) {
		return true;
	}
	system("cls");
	return HuffmanDecompress(path);
}

bool Menu() {
	system("cls");
	PrintMenu();
	char ch = _getch();
	while (ch != '1' && ch != '2' && ch != '3') {
		ch = _getch();
	}
	switch (ch) {
	case '1':
		return CompressMenu();
	case '2':
		return DecompressMenu();
	case '3':
		return false;
	}
	return false;
}

void Start() {
	while (Menu()) {}
}

int main() {
	setlocale(LC_ALL, "Russian");
	Start();
	if (_CrtDumpMemoryLeaks()) {
		printf("Memory leak is detected!");
	}
	else {
		printf("Finished with success");
	}
	return 0;
}
