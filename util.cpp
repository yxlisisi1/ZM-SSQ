#include "util.h"

bool compareRecords(const Record* a, const Record* b) {
	return a->key < b->key;
}

char* generateFile(const char database[], const char file_extension[]) {
	// Check if the database is NULL
	if (database == nullptr) {
		std::cerr << "Error: database is null." << std::endl;
		return nullptr;
	}

	// Calculate the total length
	const int base_path_len = strlen("/mnt/hgfs/data2/githubCode/ZcurveDataset1/");
	const int query_path_len = strlen("/query/");
	const int file_ext_len = strlen(file_extension);
	const int total_len = base_path_len + strlen(database) * 2 + query_path_len + file_ext_len + 1; // +1 for null terminator

	// Dynamic memory allocation
	char* datafile = new char[total_len];

	// Build the path
	snprintf(datafile, total_len, "/mnt/hgfs/data2/githubCode/ZcurveDataset1/%s/query/%s%s", database, database, file_extension);


	return datafile;
}

char* generateDatafile(const char database[]) {
	return generateFile(database, ".txt");
}

char* generateQueryfile(const char database[]) {
	return generateFile(database, "_query.txt");
}

unsigned long long generateMask(int num_bits) {
	unsigned long long mask = (1ULL << num_bits) - 1;
	return mask;
}

void generateZOrderKey(Record& record, int maxBits, int* elemsbucket)
{
	vector<int> elems = record.elems;
	int countArr[PARTITION_NUMBER] = { 0 };
	for (int i = 0; i < elems.size(); i++)
	{
		int no = elemsbucket[elems[i]];
		if (no < 0)
		{
			throw "unexpected elements encountered!";
		}
		countArr[no]++;
	}
	record.key = generateWord(countArr, elems.size(), maxBits);
	record.bucketKey = generateBucketKey(countArr, maxBits);
}

void generateZOrderKey(Query& record, int maxBits, int* elemsbucket)
{
	vector<int> elems = record.elems;
	int countArr[PARTITION_NUMBER] = { 0 };
	for (int i = 0; i < elems.size(); i++)
	{
		int no = elemsbucket[elems[i]];
		if (no < 0)
		{
			throw "unexpected elements encountered!";
		}
		countArr[no]++;
	}
	record.key = generateWord(countArr, elems.size(), maxBits);
	record.bucketKey = generateBucketKey(countArr, maxBits);
}

int* getCountBucketNumber(unsigned long long key) {
	int* countBucketNumber = (int*)malloc(sizeof(int) * PARTITION_NUMBER);
	int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;
	for (int i = 0; i < PARTITION_NUMBER; i++) {
		unsigned long long mask = 1ULL << (WORD_SIZE - 1 - LEN_SIZE - i);
		int res = 0;
		for (int j = 0; j < maxBits; j++)
		{
			if (mask & key)
			{
				res = (res << 1) | 1;
			}
			else
			{
				res = res << 1;
			}
			mask = mask >> PARTITION_NUMBER;
		}
		countBucketNumber[i] = res;
	}
	return countBucketNumber;
}

unsigned long long generateWord(const int* bucketNum, int len, int maxBits) {
	unsigned long long result = 0;
	unsigned long long word = 0;
	int i;

	// Check if LEN_SIZE is 0,if so, skip the shift operation
	if (LEN_SIZE > 0) {
		result = static_cast<unsigned long long>(len) << (WORD_SIZE - LEN_SIZE);
	}

	for (i = 0; i < PARTITION_NUMBER * maxBits; i++) {
		int wordPos = WORD_SIZE - LEN_SIZE - i % WORD_SIZE - 1;
		unsigned int mask = 1 << (maxBits - i / PARTITION_NUMBER - 1);

		if (mask & bucketNum[i % PARTITION_NUMBER]) {
			word |= 1ULL << wordPos;
		}

		if (i % WORD_SIZE == WORD_SIZE - 1) {
			result |= word;
		}
	}

	if (i % WORD_SIZE != 0) {
		result |= word;
	}

	return result;
}

unsigned long long generateBucketKey(const int* bucketNum, int maxBits) {
	unsigned long long result = 0;
	int i;
	for (i = 0; i < PARTITION_NUMBER; i++) {
		unsigned long long shiftedBucket = (unsigned long long)(bucketNum[i] & ((1 << maxBits) - 1))
											<< (maxBits * (PARTITION_NUMBER - i - 1));
		result |= shiftedBucket;
	}

	return result;
}


bool getJaccard(Record *rec, Query *que, double delta) {
	int queryLen = que->elems.size();
	int recLen = rec->elems.size();
	int T = (int)ceil(delta * (queryLen + recLen) / (1 + delta));
	int commonCount = 0;
	int i = 0, j = 0;
	while (i < queryLen && j < recLen) {
		if (rec->elems[j] == que->elems[i]) {
			commonCount++;
			if (commonCount >= T) {
				return true;
			}
			i++;
			j++;
		} else if (que->elems[i] < rec->elems[j]) {
			i++;
		} else {
			j++;
		}
		int remainingCount = min(queryLen - i, recLen - j);
		if (commonCount + remainingCount < T) {
			return false;
		}
	}
	return commonCount >= T;
}
