#ifndef TESTER_H
#define TESTER_H

#include <set>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "StringHelper.h"

using namespace std;

struct Segment{
    string segment;
    int offset;
};

struct TestPoint{
    int segment;
    int offset;
    int value;
};

typedef vector<TestPoint> InputVariables;
typedef vector<InputVariables> TestSet;

class Tester
{
private:
    string filename;
    set<string> usedMemory;
    int maxStackSize = 0;
    int numtests = 1;
    int numArgs;
    void getUsedMemory();
    void printUsedMemory();
    int getRand(int min, int max);
    void writeTests();

    TestSet testset;
    void parseTestSet();
    Segment parseSegment(string input);
    InputVariables parseInputVariables(string inputVariables);
    TestPoint parseTestPoints(string testPoints);


    map<string, int> segmap = {
        {"local", 300},
        {"argument", 400},
        {"this", 3000},
        {"that", 3010},
        {"temp", 5},
        {"static", 16}
    };

public:
    Tester();
    ~Tester();

    void setFileName(string filename);
    void setNumtests(int num);
    void build();
};


#endif