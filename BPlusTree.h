

#ifndef C___BPLUSTREE_H
#define C___BPLUSTREE_H

#include <iostream>
#include <vector>

class Node;
using namespace std;

class BPlusTree {

protected: Node * root;
protected: int order;
protected: Node * header;

public: void setHeader(Node *head);

public: void setRoot(Node *root);

public: int getOrder();

public: double search(int key);

public: vector<double> *search(int start, int end);

public: void del(int key);

public: void insert(int key, double value);

public: BPlusTree(int order);

};


#endif //C___BPLUSTREE_H
