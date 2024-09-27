#ifndef STRINGHELPER_H
#define STRINGHELPER_H
#include <vector>
#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
using namespace std;

class StringHelper
{
private:
    /* data */
public:
    static vector<string> split(string input, char delimiter);
    static vector<string> splitFile(string filename, char delimiter);
    static string clean(string str, char ch);
};

#endif