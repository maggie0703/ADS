

#include "Node.h"
#include <iostream>
#include <iterator>
#include <vector>
#include <list>

using namespace std;

Node::Node(bool isLeaf, bool isRoot)
{
    this->isRoot = isRoot;
    this->isLeaf = isLeaf;
    this->entries = new vector<bPair *>();
    if (!isLeaf)
    {
        this->children = new vector<Node *>();
    }

    this->parent = nullptr;
    this->previous = nullptr;
    this->next = nullptr;
}

Node::Node(bool isLeaf)
{
    this->isLeaf = isLeaf;
    this->isRoot = false;
    this->entries = new vector<bPair *>();
    if (!isLeaf)
    {
        this->children = new vector<Node *>();
    }
    this->parent = nullptr;
    this->previous = nullptr;
    this->next = nullptr;
}

double Node::get(int key)
{
    if (this->isLeaf)
    {
        // search in the entries;
        for (int i = 0; i < this->entries->size(); i++)
        {
            if (this->entries->at(i)->first == key)
            {
                return entries->at(i)->second;
            }
        }
    }
    else
    {
        // search in left node;
        if (key < this->entries->at(0)->first)
        {
            return children->at(0)->get(key);

            // search in right node;
        }
        else if (key > this->entries->at(this->entries->size() - 1)->first)
        {
            return children->at(this->entries->size() - 1)->get(key);
        }
        else
        {
            // search in the middle nodes;
            for (int i = 0; i < this->entries->size(); i++)
            {
                // check which to seach
                if (this->entries->at(i)->first <= key && this->entries->at(i + 1)->first > key)
                {
                    return this->children->at(i)->get(key);
                }
            }
        }
    }
    return 0;
}

void Node::insertOrUpdate(int key, double value, BPlusTree *tree)
{
    if (this->isLeaf)
    {
        // if contains or enough space update
        if (contain(key) || entries->size() < tree->getOrder())
        {
            insertOrUpdate(key, value);
            if (parent != nullptr)
            {
                parent->updateInsert(tree);
            }
        }
        else
        {
            // split into two nodes
            Node *left = new Node(true);
            Node *right = new Node(true);
            // connect previous;
            if (previous != nullptr)
            {
                previous->next = left;
                left->previous = previous;
            }
            // connect next;
            if (next != nullptr)
            {
                next->previous = right;
                right->next = next;
            }

            if (previous == nullptr)
            {
                tree->setHeader(left);
            }

            left->next = right;
            right->previous = left;
            previous = nullptr;
            next = nullptr;
            int leftSize = (tree->getOrder() + 1) / 2 + (tree->getOrder() + 1) % 2;
            int rightSize = (tree->getOrder() + 1) / 2;
            insertOrUpdate(key, value);

            // copy to left;
            for (int i = 0; i < leftSize; i++)
            {
                left->entries->push_back(this->entries->at(i));
            }
            // copy to right;
            for (int i = 0; i < rightSize; i++)
            {
                right->entries->push_back(this->entries->at(leftSize + i));
            }

            // if not root
            if (parent != nullptr)
            {
                int index = this->indexOfChild();
                parent->children->erase(parent->children->begin() + index);
                left->parent = parent;
                right->parent = parent;
                parent->children->insert(parent->children->begin() + index, left);
                parent->children->insert(parent->children->begin() + index + 1, right);

                // clear self;
                this->entries = nullptr;
                this->children = nullptr;
                parent->updateInsert(tree);
                this->parent = nullptr;
            }
            else
            {
                // not root any more;
                isRoot = false;
                Node *locParent = new Node(false, true);
                tree->setRoot(locParent);
                left->parent = locParent;
                right->parent = locParent;
                locParent->children->push_back(left);
                locParent->children->push_back(right);
                // clear self;
                this->entries = nullptr;
                this->children = nullptr;

                locParent->updateInsert(tree);
            }
        }
    }
    else
    {
        if (key < this->entries->at(0)->first)
        {
            // insert to left;
            return children->at(0)->insertOrUpdate(key, value, tree);
        }
        else if (key > this->entries->at(this->entries->size() - 1)->first)
        {
            // insert to right;
            return children->at(this->entries->size() - 1)->insertOrUpdate(key, value, tree);
        }
        else
        {
            // check which one to insert;
            for (int i = 0; i < this->entries->size(); i++)
            {
                if (this->entries->at(i)->first <= key && this->entries->at(i + 1)->first > key)
                {
                    return this->children->at(i)->insertOrUpdate(key, value, tree);
                }
            }
        }
    }
}

void Node::remove(int key, BPlusTree *tree)
{
    if (isLeaf)
    {
        // not contain;
        if (!contain(key))
        {
            return;
        }

        if (isRoot)
        {
            /// delete directly
            remove(key);
        }
        else
        {
            /// delete directly
            if (entries->size() > tree->getOrder() / 2 && entries->size() > 2)
            {
                remove(key);
            }
            else
            {
                /// borrow one from previous
                if (previous != nullptr && previous->entries->size() > tree->getOrder() / 2 && previous->entries->size() > 2 && previous->parent == parent)
                {
                    bPair *last = previous->entries->back();
                    previous->entries->pop_back();
                    entries->insert(entries->begin(), last);
                    remove(key);
                    /// borrow one from next
                }
                else if (
                    next != nullptr && next->entries->size() > tree->getOrder() / 2 && next->entries->size() > 2 && next->parent == parent)
                {
                    bPair *first = next->entries->front();
                    next->entries->erase(next->entries->begin());
                    entries->push_back(first);
                    remove(key);
                }
                else
                {
                    /// combine with previous
                    if (previous != nullptr && (previous->entries->size() <= tree->getOrder() / 2 || previous->entries->size() <= 2) && (previous->parent == parent))
                    {
                        for (int i = previous->entries->size() - 1; i >= 0; i--)
                        {
                            entries->insert(entries->begin(), previous->entries->at(i));
                        }
                        remove(key);
                        int index = previous->indexOfChild();

                        previous->parent = nullptr;
                        previous->entries = nullptr;
                        parent->children->erase(parent->children->begin() + index);

                        if (previous->previous != nullptr)
                        {
                            previous->previous->next = this;
                            Node *temp = this->previous;
                            this->previous = previous->previous;
                            temp->next = nullptr;
                            temp->previous = nullptr;
                        }
                        else
                        {
                            tree->setHeader(this);
                            previous->next = nullptr;
                            previous = nullptr;
                        }
                        /// combine with next
                    }
                    else if (next != nullptr && (next->entries->size() <= tree->getOrder() / 2 || next->entries->size() <= 2) && next->parent == parent)
                    {
                        for (int i = 0; i < next->entries->size(); i++)
                        {
                            entries->push_back(next->entries->at(i));
                        }
                        remove(key);
                        int index = next->indexOfChild();
                        parent->children->erase(parent->children->begin() + index);

                        if (next->next != nullptr)
                        {
                            Node *temp = next;
                            temp->next->previous = this;
                            next = temp->next;
                            temp->previous = nullptr;
                            temp->next = nullptr;
                        }
                        else
                        {
                            next->previous = nullptr;
                            next = nullptr;
                        }
                    }
                }
            }
            /// update parent after remove;
            parent->updateRemove(tree);
        }
    }
    else
    {
        if (key < this->entries->at(0)->first)
        {
            /// search left to remove;
            return children->at(0)->remove(key, tree);
        }
        else if (key > this->entries->at(this->entries->size() - 1)->first)
        {
            /// search right to remove;
            return children->at(this->entries->size() - 1)->remove(key, tree);
        }
        else
        {
            /// search which node to remove;
            for (int i = 0; i < this->entries->size() - 1; i++)
            {
                if (this->entries->at(i)->first <= key && this->entries->at(i + 1)->first > key)
                {
                    return this->children->at(i)->remove(key, tree);
                }
            }
        }
    }
}

void Node::updateInsert(BPlusTree *tree)
{
    validate(this, tree);
    /// after update, some node may has more nodes;
    /// split nodes if needed;
    if (children->size() > tree->getOrder())
    {
        Node *left = new Node(false);
        Node *right = new Node(false);
        int leftSize = (tree->getOrder() + 1) / 2 + (tree->getOrder() + 1) % 2;
        int rightSize = (tree->getOrder() + 1) / 2;

        for (int i = 0; i < leftSize; i++)
        {
            left->children->push_back(children->at(i));
            int key = children->at(i)->entries->at(0)->first;
            left->entries->push_back(new bPair(key, 0));
            children->at(i)->parent = left;
        }

        for (int i = 0; i < rightSize; i++)
        {
            right->children->push_back(children->at(leftSize + i));
            int key = children->at(leftSize + i)->entries->at(0)->first;
            right->entries->push_back(new bPair(key, 0));
            children->at(leftSize + i)->parent = right;
        }

        if (parent != nullptr)
        {
            int index = indexOfChild();
            parent->children->erase(parent->children->begin() + index);
            left->parent = parent;
            right->parent = parent;
            parent->children->insert(parent->children->begin() + index, left);
            parent->children->insert(parent->children->begin() + index + 1, right);
            this->entries = nullptr;
            this->children = nullptr;
            /// update insert recursive
            parent->updateInsert(tree);
            this->parent = nullptr;
        }
        else
        {
            /// not root any more
            isRoot = false;
            Node *locParent = new Node(false, true);
            tree->setRoot(locParent);
            left->parent = locParent;
            right->parent = locParent;
            locParent->children->push_back(left);
            locParent->children->push_back(right);
            this->entries = nullptr;
            this->children = nullptr;
            locParent->updateInsert(tree);
        }
    }
}

/// if node is removed, check every node for enough child;
void Node::updateRemove(BPlusTree *tree)
{
    validate(this, tree);

    /// not enough children
    if (children->size() < tree->getOrder() / 2 || children->size() < 2)
    {
        if (isRoot)
        {
            // it is ok that root has more than 2 children;
            if (children->size() >= 2)
            {
                return;
            }
            else
            {
                // use only child as root;
                Node *root = children->at(0);
                tree->setRoot(root);
                root->parent = nullptr;
                root->isRoot = true;
                root->entries = nullptr;
                root->children = nullptr;
            }
        }
        else
        {
            // calculate previous and next
            int curIndex = this->indexOfChild();
            int prevIndex = curIndex - 1;
            int nextIndex = curIndex + 1;
            Node *locPrevious = nullptr;
            Node *locNext = nullptr;
            if (prevIndex >= 0)
            {
                locPrevious = parent->children->at(prevIndex);
            }

            if (nextIndex < parent->children->size())
            {
                locNext = parent->children->at(nextIndex);
            }

            // if enough node in previous, get from it;
            if (locPrevious != nullptr && locPrevious->children->size() > tree->getOrder() / 2 && locPrevious->children->size() > 2)
            {
                Node *borrow = locPrevious->children->back();
                locPrevious->children->pop_back();
                borrow->parent = this;
                children->insert(children->begin(), borrow);
                validate(previous, tree);
                validate(this, tree);
                // update remove recursive
                parent->updateRemove(tree);

                // borrow from next
            }
            else if (locNext != nullptr && locNext->children->size() > tree->getOrder() / 2 && locNext->children->size() > 2)
            {

                Node *borrow = locNext->children->front();
                locNext->children->erase(locNext->children->begin());
                borrow->parent = this;
                children->push_back(borrow);

                validate(locNext, tree);
                validate(this, tree);

                parent->updateRemove(tree);
            }
            else
            {
                // combine node
                if (locPrevious != nullptr && (locPrevious->children->size() <= tree->getOrder() / 2 || locPrevious->children->size() <= 2))
                {
                    for (int i = locPrevious->children->size() - 1; i >= 0; i--)
                    {
                        Node *child = locPrevious->children->at(i);
                        children->insert(children->begin(), child);
                        child->parent = this;
                    }
                    locPrevious->children = nullptr;
                    locPrevious->entries = nullptr;
                    locPrevious->parent = nullptr;

                    validate(this, tree);
                    parent->updateRemove(tree);
                }
                else if (locNext != nullptr && (locNext->children->size() <= tree->getOrder() / 2 || locNext->children->size() <= 2))
                {
                    for (int i = 0; i < locNext->children->size(); i++)
                    {
                        Node *child = locNext->children->at(i);
                        children->push_back(child);
                        child->parent = this;
                    }
                    locNext->children = nullptr;
                    locNext->entries = nullptr;
                    locNext->parent = nullptr;
                    parent->children->erase(parent->children->begin() + nextIndex);
                    validate(this, tree);
                    parent->updateRemove(tree);
                }
            }
        }
    }
}
/// if node contain key
bool Node::contain(int key)
{
    for (auto &entrie : *this->entries)
    {
        if (entrie->first == key)
        {
            return true;
        }
    }
    return false;
}

/// insert into nodes;
void Node::insertOrUpdate(int key, double obj)
{
    auto pair = new bPair(key, obj);
    if (entries->empty())
    {
        entries->push_back(pair);
        return;
    }
    for (int i = 0; i < entries->size(); i++)
    {
        if (entries->at(i)->first == key)
        {
            entries->at(i)->second = obj;
            return;
        }
        else if (entries->at(i)->first > key)
        {
            entries->insert(entries->begin() + i, pair);
            return;
        }
    }
    entries->push_back(pair);
}

/// remove form node
void Node::remove(int key)
{
    int index = -1;
    for (int i = 0; i < entries->size(); i++)
    {
        if (entries->at(i)->first == key)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        entries->erase(entries->begin() + index);
    }
}

/// index of parent children array;
int Node::indexOfChild()
{
    for (int i = 0; i < parent->children->size(); i++)
    {
        if (parent->children->at(i) == this)
        {
            return i;
        }
    }
    return 0;
}

/// check tree;
void Node::validate(Node *node, BPlusTree *tree)
{
    if (node->entries->size() == node->children->size())
    {
        for (int i = 0; i < node->entries->size(); i++)
        {
            int key = node->children->at(i)->entries->at(0)->first;
            if (node->entries->at(i)->first != key)
            {
                node->entries->erase(node->entries->begin() + i);
                node->entries->insert(node->entries->begin() + i, new bPair(key, 0));
                if (!node->isRoot)
                {
                    validate(node->parent, tree);
                }
            }
        }
    }
    else if ((node->isRoot && node->children->size() > tree->getOrder() / 2) || (node->children->size() > tree->getOrder() / 2 && node->children->size() <= tree->getOrder() && node->children->size() >= 2))
    {
        /// rebuild tree;
        node->entries->clear();
        for (int i = 0; i < node->children->size(); i++)
        {
            int key = node->children->at(i)->entries->at(0)->first;
            node->entries->push_back(new bPair(key, 0));
            if (!node->isRoot)
            {
                validate(node->parent, tree);
            }
        }
    }
}

/// search keys in a region
void Node::searchRegion(int start, int end, vector<double> *dict)
{
    if (isLeaf)
    {
        for (int i = 0; i < entries->size(); i++)
        {
            double key = entries->at(i)->first;
            if (key >= start && key <= end)
            {
                dict->push_back(entries->at(i)->second);
            }
        }
    }
    else
    {
        for (int i = 0; i < children->size(); i++)
        {
            children->at(i)->searchRegion(start, end, dict);
        }
    }
}
