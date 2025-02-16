#pragma once
#include <vector>

using namespace std;

struct Query
{
public:
	vector<int> elems;
	unsigned long long key;
	unsigned long long bucketKey;
};

class Record
{
public:
	int id;
	vector<int> elems;
	unsigned long long key;
	unsigned long long bucketKey;
};
