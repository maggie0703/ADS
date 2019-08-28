

#include "BPlusTree.h"
#include "Node.h"

void BPlusTree::setHeader(Node *head) {
    this->header = head;

}

void BPlusTree::setRoot(Node *root) {
    this->root = root;
}

int BPlusTree::getOrder() {
    return this->order;
}

double BPlusTree::search(int key) {
    return root->get(key);
}

vector<double> *BPlusTree::search(int start, int end) {
    vector<double> * result = new vector<double>();
    root->searchRegion(start, end, result);
    return result;
}

BPlusTree::BPlusTree(int order) {
    this->order = order;
    this->root = new Node(true, true);
    this->header = root;
}

void BPlusTree::insert(int key, double value) {
    this->root->insertOrUpdate(key, value, this);
}

void BPlusTree::del(int key) {
    root->remove(key, this);
}
