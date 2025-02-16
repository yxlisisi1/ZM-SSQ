#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <queue>
#include "node.h"
#include "data.h"

#define _IDCOLUMN 0

using namespace std;

class TreeValue {
public:
	int maxBits = 0;
	Record* minRecord = nullptr;
	Record* maxRecord = nullptr;
};

struct NodePair
{
	int len;
	Node* lenHead;
	Node* lenRear;
};


void bulkLoadingInsert(Node*& tree, vector<Record*> data, int treeOrder);
void upFirsts(vector<KeyType>& keys, vector<Node*>& pointers, int treeOrder, Node*& tree);
float getTreeSize(Node* tree);
