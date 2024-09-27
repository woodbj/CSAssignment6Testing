#include "Tester.h"
#include <iostream>


using namespace std;

/** A quick-and-dirty parser */
int main(int argc, char **argv)
{
    if(argc <= 1){
        cerr << "need to specify an input file" << endl;
    }

    string filename = argv[1];
    filename = filename.substr(0, filename.find("."));

    Tester tests;
    tests.setFileName(filename);
    tests.setNumtests(1);
    tests.build();

    return 0;
}
