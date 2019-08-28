# ADS
B+tree
1.Makefile: it generates an executable file named “bplustree” and it can allow you to rebuild the program by typing “make clean” if you need.
2.Example.txt: it’s an example of an input file.
3.output_file.txt: it’s the output of the input file.
4.main.cpp: it gets each line of an input file, splits the string, implements required operations like “Insert”, and finally writes results into an output file.
5.BPlusTree.h: it defines a class named “BPlusTree”, which includes the definition of such functions as “search”, “insert”, “delete”, “initialize”.
6.BPlusTree.cpp: it contains the specific implementation of the functions defined in the “BPlusTree.h”.
7.Node.h: it defines pointers and functions of a node. For example, the function named “updateInsert” is to split nodes if needed after update when inserting a pair of a key and a value.
8.Node.cpp: it contains the corresponding functions of the definitions in the “Node.h”.
