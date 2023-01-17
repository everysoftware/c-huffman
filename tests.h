#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

void MakeASCIITest();

void MakeTextTest(const char* fileName, const char* alphabet, uint16_t size_in_MB);

void TestParsePath();

#endif
