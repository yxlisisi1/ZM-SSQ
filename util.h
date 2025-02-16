

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <math.h>
#include <vector>
#include "record.h"


using namespace std;

#define PARTITION_NUMBER 8
#define MAXBIT 255


#define LEN_SIZE 8
#define WORD_SIZE (sizeof(unsigned long long) * 8)
#define GetBit(v, n) ((v) & ((UINT64)1 << (n)))

bool compareRecords(const Record* a, const Record* b);
char* generateDatafile(const char database[]);
char* generateQueryfile(const char database[]);
double getJaccard(Record *rec, Query *que);
bool getJaccard(Record *rec, Query *que, double delta);

unsigned long long generateMask(int num_bits);
int* getCountBucketNumber(unsigned long long key);

void generateZOrderKey(Record& record, int maxBits, int* elemsbucket);
void generateZOrderKey(Query& record, int maxBits, int* elemsbucket);
unsigned long long generateWord(const int *bucketNum, int len, int maxBits);
unsigned long long generateBucketKey(const int* bucketNum, int maxBits);

#endif
