#include "bplustree.h"

void bulkLoadingInsert(Node*& tree, vector<Record*> data, int treeOrder) {

	size_t count = 0;

	vector<Node*> nodes;
	vector<KeyType> upperKeys;
	upperKeys.clear();
	vector<Node*> upperPointers;
	upperPointers.clear();

	while (count < data.size()) {
		Node* temporaryNode = new Node(treeOrder, true);
		temporaryNode->paths.clear();
		try {
			KeyType tempKey;
			tempKey = data[count];
			while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 && count < data.size()) {
				tempKey = data[count];
				if (temporaryNode->numberOfKeys > 0 && !(data[count]->key >
					temporaryNode->keys[temporaryNode->numberOfKeys - 1]->key)) {
					temporaryNode->paths[temporaryNode->paths.size() - 1].push_back(tempKey);
				}
				else {

					vector<KeyType> newVector;
					temporaryNode->keys.push_back(tempKey);
					newVector.push_back(tempKey);
					temporaryNode->paths.push_back(newVector);
					temporaryNode->numberOfKeys++;
				}
				count++;
			}
			if (nodes.size() > 0) {
				nodes[nodes.size() - 1]->nextList = temporaryNode;
				temporaryNode->lastList = nodes[nodes.size() - 1];
			}
			nodes.push_back(temporaryNode);

			while (count < data.size() && nodes.size() > 0 && !(data[count]->key > nodes[nodes.size() - 1]
				->keys[nodes[nodes.size() - 1]->numberOfKeys - 1]->key)) {
				tempKey = data[count];
				nodes[nodes.size() - 1]->paths[temporaryNode->paths.size() - 1].push_back(tempKey);
				count++;
			}
		}
		catch (...) {
			cout << "[Exception] function 'bulkloading': error during tree construct. this "
				"column doesn't exist in line :"
				<< count << "\n";
			return;
		}
	}

	if (nodes[0]->numberOfKeys < (treeOrder - 1) / 2) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}

	if (nodes[nodes.size() - 1]->numberOfKeys < (treeOrder - 1) / 2) {
		for (int i = 0; i < nodes[nodes.size() - 1]->numberOfKeys; i++) {
			nodes[nodes.size() - 2]->keys.push_back(nodes[nodes.size() - 1]->keys[i]);
			nodes[nodes.size() - 2]->paths.push_back(
				nodes[nodes.size() - 1]->paths[i]);
			nodes[nodes.size() - 2]->numberOfKeys++;
		}
		nodes[nodes.size() - 2]->nextList = NULL;
		nodes.erase(nodes.begin() + nodes.size() - 1);
	}

	if (nodes.size() == 1) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}

	for (size_t i = 1; i < nodes.size(); i++) {
		upperKeys.push_back(nodes[i]->keys[0]);
		upperPointers.push_back(nodes[i - 1]);
	}
	upperPointers.push_back(nodes[nodes.size() - 1]);
	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

void upFirsts(vector<KeyType>& keys, vector<Node*>& pointers, int treeOrder, Node*& tree) {
	vector<KeyType> upperKeys;
	vector<Node*> upperPointers;
	vector<Node*> level;

	size_t count = 0;
	while (count < keys.size()) {
		Node* temporaryNode = new Node(treeOrder);

		while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 &&
			count < keys.size()) {
			temporaryNode->keys.push_back(keys[count]);
			temporaryNode->pointers.push_back(pointers[count]);
			pointers[count]->parent = temporaryNode;
			temporaryNode->numberOfKeys++;
			count++;
			}

		pointers[count]->parent = temporaryNode;
		temporaryNode->pointers.push_back(pointers[count]);
		level.push_back(temporaryNode);

		if (keys.size() - count > ((size_t)treeOrder - 1) / 2) {
			upperKeys.push_back(keys[count]);
			upperPointers.push_back(level[level.size() - 1]);
			count++;
		}
		else {
			while (count < keys.size()) {
				level[level.size() - 1]->keys.push_back(keys[count]);
				level[level.size() - 1]->numberOfKeys++;
				level[level.size() - 1]->pointers.push_back(pointers[count + 1]);
				pointers[count + 1]->parent = level[level.size() - 1];
				count++;
			}
		}
	}

	upperPointers.push_back(level[level.size() - 1]);


	if (level.size() == 1) {
		tree = level[0];
		tree->parent = NULL;
		return;
	}

	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

float getTreeSize(Node* tree) {
	float result = 0;

	if (!tree) return 0;
	queue<Node*> q;
	q.push(tree);
	while (!q.empty()) {
		Node* current = q.front();
		q.pop();

		// Push the pointer into the queue
		if (!current->isLeaf) {
			result += 8;
			result += current->keys.size() * 8;
			result += current->pointers.size() * 8;
			for (Node* child : current->pointers) {
				if (child) {
					q.push(child);
				}
			}
		} else {
			result += 8 * 3;
			result += current->keys.size() * 8 * 3;
			for (int i = 0 ; i < current->paths.size(); i++) {
				result += current->paths[i].size() * 8;
			}
		}
	}
	return result / (1024 * 1024);
}
