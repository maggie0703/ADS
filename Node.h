

#ifndef C___NODE_H
#define C___NODE_H

#include <list>
#include "BPlusTree.h"

using namespace std;
typedef list<double> bList;
typedef pair<int, double> bPair;

class Node {
public: bool isRoot;
public: bool isLeaf;
protected: Node *parent;
protected: Node *previous;
protected: Node *next;
protected: vector<bPair*> *entries;
protected: vector<Node*> * children;

public: Node(bool isLeaf, bool isRoot);

public: Node(bool isLeaf);

public: double get(int key);

public: void searchRegion(int start, int end, vector<double> *dict);

public: void insertOrUpdate(int key, double value,BPlusTree *tree);

private: void insertOrUpdate(int key, double obj);

public: void updateInsert(BPlusTree *tree);

public: void updateRemove(BPlusTree *tree);

public: bool contain(int key);

public: int indexOfChild();

public: void remove(int key, BPlusTree *tree);

protected: void remove(int key);

public :
    static void validate(Node *node,BPlusTree *tree);
};


#endif //C___NODE_H
