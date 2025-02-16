

#ifndef ZSEARCH_H
#define ZSEARCH_H

#include "util.h"
#include "record.h"
#include "bplustree.h"

class ZSearch
{
public:
    int resultCount = 0;
    double delta;
    Node* tree = NULL;

    ZSearch(double delta, Node* tree) {
        this->delta = delta;
        this->tree = tree;
    }


    void search(Query* query);
    void searchRange(Query* query);
    void searchFilter(Query* query);
    void searchRangeFilter(Query* query);

    void searchLen(Query* query);
    void searchLenRange(Query* query);
    void searchLenFilter(Query* query);
    void searchLenRangeFilter(Query* query);

    static unsigned long long getHeadKey(Query* query, int dif, int currentLen);
    static unsigned long long getRearKey(Query* query, int dif, int currentLen);

    Node* getHead(unsigned long long key);
    Node* getRear(unsigned long long key);
    static bool getResultOfCombinationFilter(unsigned long long recordKey, unsigned long long queryKey, int dif, int maxBits);

};

#endif
