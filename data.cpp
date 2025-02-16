#include "data.h"
#include <algorithm>
#include <stdio.h>
#include <string>
#include <climits>
using namespace std;

Data::Data(char* filename)
{
	datafile = filename;
	current = 0;
	maxElem = maxLen = 0;
	minElem = minLen = 2147483647;


}


Data::~Data()
{
	if (in != NULL) fclose(in);
}

void Data::readData()
{
	int id = 0;
	Record* rec;
	in = fopen(datafile, "rt");
	while ((rec = readNextRecord()) != NULL)
	{
		rec->id = id++;
		vec_trans.push_back(rec);
		if (rec->elems.size() > maxLen) maxLen = rec->elems.size();
		if (rec->elems.size() < minLen) minLen = rec->elems.size();
	}

	vector<Record*> vec_trans_temp;
	for (int i = 0; i < vec_trans.size(); i++) {
		if (vec_trans[i]->elems.size() > MAXBIT) {
			vec_longset.push_back(vec_trans[i]);
			continue;
		}
		vec_trans_temp.push_back(vec_trans[i]);
	}
	vec_trans = vec_trans_temp;

	fclose(in);
}


Record* Data::readNextRecord()
{
	set<int>::iterator it;
	Record* rec = new Record();
	char c;
	do {
		int item = 0, pos = 0;
		c = getc(in);
		while ((c >= '0') && (c <= '9')) {
			item *= 10;
			item += int(c) - int('0');
			c = getc(in);
			pos++;
		}
		if (pos) rec->elems.push_back(item);
		if (item > maxElem) maxElem = item;
		if (item < minElem) minElem = item;
		it = distinctTerms.find(item);
		if (it == distinctTerms.end())
			distinctTerms.insert(item);

	} while (c != '\n' && !feof(in));


	if (feof(in)) {
		rewind(in);
		return NULL;
	}
	return rec;
}



void Data::readQuery(char* queryfile)
{
	int id = 0;
	Query* rec;
	in = fopen(queryfile, "rt");
	while ((rec = readNextQuery()) != NULL)
	{
		vec_querys.push_back(rec);
		if (rec->elems.size() > maxLen) maxLen = rec->elems.size();
		if (rec->elems.size() < minLen) minLen = rec->elems.size();
	}
	fclose(in);
}

Query* Data::readNextQuery()
{
	set<int>::iterator it;
	Query* rec = new Query();
	char c;
	do {
		int item = 0, pos = 0;
		c = getc(in);
		while ((c >= '0') && (c <= '9')) {
			item *= 10;
			item += int(c) - int('0');
			c = getc(in);
			pos++;
		}
		if (pos) rec->elems.push_back(item);
		if (item > maxElem) maxElem = item;
		if (item < minElem) minElem = item;
		it = distinctTerms.find(item);
		if (it == distinctTerms.end())
			distinctTerms.insert(item);

	} while (c != '\n' && !feof(in));

	// if end of file is reached, rewind to beginning for next pass
	if (feof(in)) {
		rewind(in);
		return NULL;
	}
	return rec;
}


bool compareByValue(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second > b.second;
}

int* Data::getElemsBuckets() {

	map<int, int> countMap;
	int totalCount = 0;

	for (int i = 0; i < this->vec_trans.size(); i++)
	{
		for (int j = 0; j < vec_trans[i]->elems.size(); j++)
		{
			countMap[vec_trans[i]->elems[j]]++;
			totalCount++;
		}
	}
	vector<pair<int, int>> countVec(countMap.begin(), countMap.end());
	sort(countVec.begin(), countVec.end(), compareByValue);
	int* elemBucket = new int[100000000];
	long int* countArray = new long int[PARTITION_NUMBER]();

	for (int i = 0; i < countVec.size(); i++)
	{
		int pos = getMinElemPos(countArray);
		elemBucket[countVec[i].first] = pos;
		countArray[pos] += countVec[i].second;
	}
	return elemBucket;
}

int Data::getMinElemPos(long int* countArray) {
	long int min = LONG_MAX;
	int pos = -1;
	for (int i = 0; i < PARTITION_NUMBER; i++)
	{
		if (countArray[i] < min) {
			pos = i;
			min = countArray[i];
		}
	}
	return pos;
}
