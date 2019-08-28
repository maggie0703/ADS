CXXFLAG += -std=c++11
all: bplustree

bplustree: main.cpp BPlusTree.cpp Node.cpp
	g++ -std=c++11 main.cpp BPlusTree.cpp Node.cpp -o bplustree
.PHONY:clean
clean:
	rm -f main.o Node.o BPlusTree.o bplustree output_file.txt
