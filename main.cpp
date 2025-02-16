#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <math.h>
#include "bplustree.h"
#include "util.h"
#include "data.h"
#include "node.h"
#include "ZSearch.h"


double delta = 0.9; //The  threshold
int treeOrder = 16;//the tree order
int maxBits = (WORD_SIZE - LEN_SIZE) / PARTITION_NUMBER;  // The number of binary bits required for n
char database[] = "retail";//Dataset name

int main() {
    cout << database << endl;
    cout<<"delta="<<delta<<endl;
    cout << "maxBits = " << maxBits << endl;
    cout << "PARTITION_NUMBER = " << PARTITION_NUMBER << endl;
    char *datafile = generateDatafile(database);
    char *queryfile = generateQueryfile(database);


    Data* data = new Data(datafile);
    data->readData();
    data->readQuery(queryfile);


    int* elemsBuckets = data->getElemsBuckets();//Global element partitioning
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < data->vec_trans.size(); i++)
    {
        generateZOrderKey(*data->vec_trans[i], maxBits, elemsBuckets);//Generate key
    }
    cout << "ZOrderKey generate succeeded!" << endl;

    Node* tree = NULL;
    sort(data->vec_trans.begin(), data->vec_trans.end(), compareRecords);//Keys in ascending order
    bulkLoadingInsert(tree, data->vec_trans, treeOrder);// Build Tree
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;

    cout << "Index Build time: " << duration.count() << " ms" << endl;
    cout << "tree size: " << getTreeSize(tree) << "Mb" << endl;

    // Query
    ZSearch zSearch(delta, tree);
    start = chrono::high_resolution_clock::now();
    int numOfKey = 0;
    for (int i = 0; i < data->vec_querys.size(); i++)
    {
        generateZOrderKey(*data->vec_querys[i], maxBits, elemsBuckets);
        if ( data->vec_querys[i]->elems.size() / zSearch.delta > MAXBIT) {
            for (int j = 0; j < data->vec_longset.size(); j++) {
                if (getJaccard(data->vec_longset[j], data->vec_querys[i], zSearch.delta)) {// Minimum coverage verification
                    zSearch.resultCount++;
                }
            }
        }
        if (data->vec_querys[i]->elems.size() * zSearch.delta > MAXBIT)
            continue;
        // Change LEN_SIZE to 0
       //zSearch.search(data->vec_querys[i]);//Z-SSQ-None
         //zSearch.searchRange(data->vec_querys[i]);//Z-SSQ-ZF
         //zSearch.searchFilter(data->vec_querys[i]);//Z-SSQ-HF
       //zSearch.searchRangeFilter(data->vec_querys[i]);//Z-SSQ-ZFH

        // Change LEN_SIZE to 8
        //zSearch.searchLen(data->vec_querys[i]);//ZM-SSQ-None
        //zSearch.searchLenRange(data->vec_querys[i]);//ZM-SSQ-ZF
        //zSearch.searchLenFilter(data->vec_querys[i]);//ZM-SSQ-HF
      zSearch.searchLenRangeFilter(data->vec_querys[i]);//ZM-SSQ-ZHF
    }
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << endl << "average search time: " << (float)duration.count() / data->vec_querys.size() << "ms" << endl;
    cout << "query number: " << zSearch.resultCount << endl;
    cout << "numOfKey: : " << numOfKey << endl;
}
