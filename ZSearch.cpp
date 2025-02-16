
#include <cmath>
#include "ZSearch.h"

void ZSearch::search(Query* query)//z-SSQ-None
{
	Node* current = getHead(0);
	while (current != nullptr)
	{
		for (int i = 0; i < current->keys.size(); i++) {
			for (int k = 0; k < current->paths[i].size(); k++)
			{
				if (getJaccard(current->paths[i][k], query, delta)) {
					resultCount++;
				}
			}
		}
		current = current->nextList;
	}
}

void ZSearch::searchRange(Query *query) {//Z-SSQ-ZF
	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);

	unsigned long long headKey = getHeadKey(query, dif, queryLength - dif);
	unsigned long long rearKey = getRearKey(query, dif, queryLength + dif);
	Node* head = getHead(headKey);
	Node* rear = getHead(rearKey);
	Node* current = head;
	while (current->nextList != nullptr) {
		for (int i = 0; i < current->keys.size(); i++) {
			for (int k = 0; k < current->paths[i].size(); k++)
			{
				if (getJaccard(current->paths[i][k], query, delta)) {
					resultCount++;
				}
			}
		}
		if (current == rear) {
			break;
		} else {
			current = current->nextList;
		}
	}
}

void ZSearch::searchFilter(Query *query) {//Z-SSQ-HF

	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;

	Node* current = getHead(0);
	while (current != nullptr) {
		for (int i = 0; i < current->keys.size(); i++) {
			if (getResultOfCombinationFilter(query->bucketKey, current->keys[i]->bucketKey, dif, maxBits)) {
				for (int k = 0; k < current->paths[i].size(); k++) {
					if (getJaccard(current->paths[i][k], query, delta)) {
						resultCount++;
					}
				}
			}
		}
		current = current->nextList;
	}
}

void ZSearch::searchRangeFilter(Query *query) {//Z-SSQ-ZHF

	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;

	unsigned long long headKey = getHeadKey(query, dif, queryLength - dif);
	unsigned long long rearKey = getRearKey(query, dif, queryLength + dif);
	Node* head = getHead(headKey);
	Node* rear = getHead(rearKey);
	Node* current = head;
	while (current != nullptr) {
		for (int i = 0; i < current->keys.size(); i++) {
			if (getResultOfCombinationFilter(query->bucketKey, current->keys[i]->bucketKey, dif, maxBits)) {
				for (int k = 0; k < current->paths[i].size(); k++) {
					if (getJaccard(current->paths[i][k], query, delta)) {
						resultCount++;
					}
				}
			}
		}
		if (current == rear) {
			break;
		} else {
			current = current->nextList;
		}
	}
}

void ZSearch::searchLen(Query *query) {//ZM-SSQ-None
	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);

	unsigned long long headKey;
	unsigned long long rearKey;

	if (queryLength - dif < 0) {
		headKey = getHeadKey(query, 99999, 1);
	} else {
		headKey = getHeadKey(query, 99999, queryLength - dif);
	}

	if (queryLength + dif > MAXBIT) {
		rearKey = getRearKey(query, 99999, 255);
	} else {
		rearKey = getRearKey(query, 99999, queryLength + dif);
	}
	Node* head = getHead(headKey);
	Node* rear = getRear(rearKey);
	Node* current = head;
	while (current != nullptr) {
		for (int i = 0; i < current->keys.size(); i++) {
			for (int k = 0; k < current->paths[i].size(); k++) {
				if (getJaccard(current->paths[i][k], query, delta)) {
					resultCount++;
				}
			}
		}
		if (current == rear) {
			break;
		} else {
			current = current->nextList;
		}
	}
}

void ZSearch::searchLenRange(Query *query) {//ZM-SSQ-ZF
	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);
	for (int L = queryLength - dif; L <= queryLength + dif; L++) {
		if (L <= 0 || L > MAXBIT) {
			continue;
		}
		int difLen = floor((1 - delta) * (queryLength + L) / (1 + delta));
		unsigned long long headKey = getHeadKey(query, difLen, L);
		unsigned long long rearKey = getRearKey(query, difLen, L);
		Node* head = getHead(headKey);
		Node* rear = getRear(rearKey);
		Node* current = head;
		while (current != nullptr) {
			for (int i = 0; i < current->keys.size(); i++) {
				if (current->keys[i]->key >= headKey && current->keys[i]->key <= rearKey) {
					for (int k = 0; k < current->paths[i].size(); k++) {
						if (getJaccard(current->paths[i][k], query, delta)) {
							resultCount++;
						}
					}
				}
			}
			if (current == rear) {
				break;
			} else {
				current = current->nextList;
			}
		}
	}
}

void ZSearch::searchLenFilter(Query *query) { //ZM-SSQ-HF
	int queryLength = query->elems.size();
	int dif = floor((1 - delta) * queryLength / delta);
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;
	unsigned long long headKey;
	unsigned long long rearKey;

	if (queryLength - dif < 0) {
		headKey = getHeadKey(query, 99999, 1);
	} else {
		headKey = getHeadKey(query, 99999, queryLength - dif);
	}

	if (queryLength + dif > 255) {
		rearKey = getRearKey(query, 99999, 255);
	} else {
		rearKey = getRearKey(query, 99999, queryLength + dif);
	}
	Node* head = getHead(headKey);
	Node* rear = getRear(rearKey);
	Node* current = head;
	while (current != nullptr) {
		for (int i = 0; i < current->keys.size(); i++) {
			int currentLen = current->keys[i]->elems.size();
			int currentDif = floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
			if (getResultOfCombinationFilter(query->bucketKey, current->keys[i]->bucketKey, currentDif, maxBits)) {
				for (int k = 0; k < current->paths[i].size(); k++) {
					if (getJaccard(current->paths[i][k], query, delta)) {
						resultCount++;
					}
				}
			}
		}
		if (current == rear) {
			break;
		} else {
			current = current->nextList;
		}
	}
}

void ZSearch::searchLenRangeFilter(Query *query) {//ZM-SSQ-ZHF
	int queryLength = query->elems.size();
	double kFudgeFactor = .00000001;
	int dif = floor((1 - delta) * queryLength / delta);
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;
	for (int L = queryLength - dif; L <= queryLength + dif; L++) {
		if (L <= 0 || L > 255) {
			continue;
		}
		int difLen = floor((1 - delta) * (queryLength + L) / (1 + delta));
		unsigned long long headKey = getHeadKey(query, difLen, L);
		unsigned long long rearKey = getRearKey(query, difLen, L);
		Node* head = getHead(headKey);
		Node* rear = getRear(rearKey);
		Node* current = head;
		while (current != nullptr) {
			for (int i = 0; i < current->keys.size(); i++) {
				if (current->keys[i]->key >= headKey && current->keys[i]->key <= rearKey) {
					if (getResultOfCombinationFilter(query->bucketKey, current->keys[i]->bucketKey, difLen, maxBits)) {
						for (int k = 0; k < current->paths[i].size(); k++) {
							if (getJaccard(current->paths[i][k], query, delta)) {
								resultCount++;
							}

						}
					}
				}
			}
			if (current == rear) {
				break;
			} else {
				current = current->nextList;
			}
		}
	}
}


unsigned long long ZSearch::getHeadKey(Query* query, int dif, int currentLen)
{
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;
	int* bucketNum = getCountBucketNumber(query->key);

	for (int i = 0; i < PARTITION_NUMBER; i++)
	{
		bucketNum[i] -= dif;
		if (bucketNum[i] < 0)
		{
			bucketNum[i] = 0;
		}
	}

	return generateWord(bucketNum, currentLen, maxBits);
}

unsigned long long ZSearch::getRearKey(Query* query, int dif, int currentLen)
{
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;
	int* bucketNum = getCountBucketNumber(query->key);
	for (int i = 0; i < PARTITION_NUMBER; i++)
	{
		bucketNum[i] += dif;
		int max = (1 << maxBits) - 1;
		if (bucketNum[i] > max)
		{
			bucketNum[i] = max;
		}
	}
	return generateWord(bucketNum, currentLen, maxBits);
}

Node * ZSearch::getHead(unsigned long long key) {
	Node* searchNode = tree;
	while (!searchNode->isLeaf) {
		for (int i = 0; i < searchNode->keys.size(); i++)
		{
			if (searchNode->keys[i]->key > key) {
				searchNode = searchNode->pointers[i];
				break;
			}
			if (i == searchNode->keys.size() - 1)
			{
				searchNode = searchNode->pointers[i + 1];
				break;
			}
		}
	}
	return searchNode;
}

Node * ZSearch::getRear(unsigned long long key) {
	Node* searchNode = tree;
	while (!searchNode->isLeaf) {
		for (int i = 0; i < searchNode->keys.size(); i++)
		{
			if (searchNode->keys[i]->key > key) {
				searchNode = searchNode->pointers[i];
				break;
			}
			if (i == searchNode->keys.size() - 1)
			{
				searchNode = searchNode->pointers[i + 1];
				break;
			}
		}
	}
	return searchNode;
}

bool ZSearch::getResultOfCombinationFilter(unsigned long long recordKey, unsigned long long queryKey, int dif, int maxBits)
{
	int difCount = 0;
	unsigned long long mask = (1ULL << maxBits) - 1;

	for (int i = 0; i < PARTITION_NUMBER; i++)
	{

		difCount += abs((long long)((recordKey >> (i * maxBits)) & mask) - (long long)((queryKey >> (i * maxBits)) & mask));
		if (difCount > dif)
		{
			return false;
		}
	}
	return true;
}
