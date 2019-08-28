#include <iostream>
#include <fstream>
#include <string>
#include "BPlusTree.h"
#include <locale>

std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

// split string method;
std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false)
{
    std::vector<std::string> tokens;

    for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
    {
        size_t position = s.find(delimiter, start);
        end = position != string::npos ? position : s.length();

        std::string token = s.substr(start, end - start);
        if (!removeEmptyEntries || !token.empty())
        {
            tokens.push_back(token);
        }
    }

    if (!removeEmptyEntries &&
        (s.empty() || endsWith(s, delimiter)))
    {
        tokens.push_back("");
    }

    return tokens;
}

int main(int argc, char *argv[]) {
    ifstream fin(argv[1]);
    ofstream outfile("./output_file.txt");

    string line;

    while(getline(fin, line)) {
        std::cout << line << std::endl;
        vector<string> results = split(line, "(");
        string method = trim(results.front());
        string value = trim(split(results.back(), ")").front());
        BPlusTree *tree;

        if (method == "Initialize") {
            std::cout << "Initialize" << std::endl;
            int order = std::stoi(value);
            tree = new BPlusTree(order);
        } else if (method == "Insert") {
            std::cout << "Insert" << std::endl;
            int key = stoi(split(value, ",").front());
            double price = stod(split(value, ",").back());
            tree->insert(key, price);
        } else if (method == "Delete") {
            std::cout << "Delete" << std::endl;
            int key = std::stoi(value);
            tree->del(key);
        } else if (method == "Search") {
            if (value.find(',') != std::string::npos) {
                int start = stoi(split(value, ",").front());
                int end = stoi(split(value, ",").back());
                vector<double> *results = tree->search(start, end);
                if (results->empty()) {
                    outfile << "NULL";
                }

                for (int i = 0; i < results->size(); i++) {
                    outfile << results->at(i);
                    if (i != results->size() - 1) {
                        outfile << ",";
                    }
                }
                outfile << std::endl;
            } else {
                int key = stoi(split(value, ",").back());
                double price = tree->search(key);
                if (price == 0) {
                    outfile <<"NULL"<< std::endl;
                } else {
                    outfile << price << std::endl;
                }
            }

        } else {
            std::cout << "None" << std::endl;
        }
    }

    outfile.close();
    return 0;
}

