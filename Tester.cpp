#include "Tester.h"
using namespace std;

Tester::Tester()
{
}

Tester::~Tester()
{
}

void Tester::getUsedMemory()
{
    fstream file;
    file.open(filename + ".vm", ios::in);

    if (file.is_open())
    {
        int stacksize = 0;
        maxStackSize = 0;
        string line;

        while (getline(file, line))
        {
            regex endl_re("\\r*\\n+");
            regex space_re("\\s+");
            line = regex_replace(line, endl_re, "");
            line = regex_replace(line, space_re, " ");
            vector<string> tokens;
            stringstream ss(line);
            string token;

            // Tokenize
            while (getline(ss, token, ' '))
            {
                if (!token.empty())
                {
                    tokens.push_back(token); // Avoid pushing empty tokens
                }
            }

            if (tokens.size() == 3)
            {
                // constant is on the inside so the stack size counter doesn't ignore it
                if (tokens[0] == "push")
                {
                    stacksize++;
                    if (tokens[1] != "constant")
                    {
                        string name = tokens[1] + " " + tokens[2];
                        usedMemory.emplace(name);
                    }
                }
                else if (tokens[0] == "pop")
                {
                    stacksize--;
                    if (tokens[1] != "constant")
                    {
                        string name = tokens[1] + " " + tokens[2];
                        usedMemory.emplace(name);
                    }
                }
                maxStackSize = (stacksize > maxStackSize) ? stacksize : maxStackSize;
            }
        }
    }
    else // error handling
    {
        cerr << filename + ".vm not found" << endl;
    }

    numArgs = 0;
    for (auto seg : usedMemory)
    {
        if (seg.find("argument") != string::npos)
        {
            numArgs++;
        }
    }
}

void Tester::printUsedMemory()
{
    cout << "Memory segments used:\n";
    for (auto segment : usedMemory)
    {
        cout << segment << endl;
    }
    cout << "max stack size: " << maxStackSize << endl;
    cout << "--end--\n";
}

void Tester::setFileName(string filename)
{
    this->filename = filename;
}

void Tester::setNumtests(int num)
{
    numtests = num;
}

int Tester::getRand(int min, int max)
{
    static bool first = true;
    if (first)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        first = false;
    }
    return (rand() % (max - min + 1)) + min;
}

void Tester::printTests()
{
    int testnum = 0;
    for (auto tests : testset)
    {
        cout << "Test No. " << testnum++ << '\t';
        for (auto tp : tests)
        {
            cout << "RAM[" << tp.offset + tp.segment << "], " << tp.value << '\t';
        }
        cout << endl;

    }
}

void Tester::writeTests()
{
    std::ostringstream oss;
    string casename;

    // start building the tests;
    int i = 0;
    for (auto tests : testset)
    {
        // set up common inputs
        string memsetup = "// set up memory\n";
        memsetup += "set RAM[0] 256,\n";
        memsetup += "set RAM[1] 300,\n";
        memsetup += "set RAM[2] 400,\n";
        memsetup += "set RAM[3] 3000,\n";
        memsetup += "set RAM[4] 3010,\n";

        string asmloop = "//loop\n";
        asmloop += "repeat 20000 {\n";
        asmloop += "\tticktock;\n";
        asmloop += "}\n";

        string vmloop = "//loop\n";
        vmloop += "repeat 1000 {\n";
        vmloop += "\tvmstep;\n";
        vmloop += "}\n";

        string outputs = "// enter outputs to compare\n";
        outputs += "output-list\n";

        outputs += "RAM[0]%D1.6.1\n";
        // outputs += "RAM[1]%D1.6.1\n";
        // outputs += "RAM[2]%D1.6.1\n";
        // outputs += "RAM[3]%D1.6.1\n";
        // outputs += "RAM[4]%D1.6.1\n";

        for (int i = 0; i < maxStackSize; i++)
        {
            outputs += "RAM[" + to_string(256 + i) + "]%D1.6.1\n";
        }

        // add outputs for all the memory segments used
        for (auto i : usedMemory)
        {
            Segment seg = parseSegment(i);
            int addr = segmap[seg.segment] + seg.offset;
            outputs += "RAM[";
            outputs += to_string(addr);
            outputs += "]%D1.11.1\n";
        }
        outputs += ";\noutput;";
        memsetup += "\n// Test inputs\n";
        for (auto vars : tests)
        {
            memsetup += "set RAM[" + to_string(vars.segment + vars.offset) + "] " + to_string(vars.value) + ",\n";
        }
        // get case numbering
        oss.str("");
        oss.clear();
        oss << std::setw(2) << std::setfill('0') << i;
        casename = filename + oss.str();
        i++;

        // create headers
        string asmheader = "// assembly test\n";
        asmheader += "load " + filename + ".asm,\n";
        asmheader += "output-file " + casename + ".out,\n";
        asmheader += "compare-to " + casename + ".cmp,\n";

        string vmheader = "// virtual machine test\n";
        vmheader += "load " + filename + ".vm,\n";
        vmheader += "output-file " + casename + ".cmp,\n";

        ofstream file;
        file.open(casename + ".tst");
        file << vmheader << '\n'
             << memsetup << '\n'
             << vmloop << '\n'
             << outputs;
        file.close();

        file.open(casename + ".asm.tst");
        file << asmheader << '\n'
             << memsetup << '\n'
             << asmloop << '\n'
             << outputs;
        file.close();
    }
}

Segment Tester::parseSegment(string input)
{
    Segment result;
    int space = input.find(" ");
    result.segment = input.substr(0, space);
    result.offset = std::stoi(input.substr(space + 1));
    return result;
}

void Tester::parseTestSet()
{
    testset.clear();
    vector<string> lines = StringHelper::splitFile(filename + ".test", '\n');

    for (string line : lines)
    {
        // cout << "Test: " << line << endl;
        testset.push_back(parseInputVariables(line));
    }
}

InputVariables Tester::parseInputVariables(string inputVariables)
{
    InputVariables iv;

    vector<string> lines = StringHelper::split(inputVariables, ';');

    for (string line : lines)
    {
        // cout << "Cases: " << line << endl;
        iv.push_back(parseTestPoints(line));
    }

    return iv;
}

TestPoint Tester::parseTestPoints(string testPoints)
{
    TestPoint tp;
    vector<string> tokens = StringHelper::split(testPoints, ' ');
    if (tokens[0] == "pointer")
    {
        if (tokens[1] == "0")
        {
            tokens[0] = "this";
            tokens[1] = "0";
        }
        else if (tokens[1] == "1")
        {
            tokens[0] = "that";
            tokens[1] = "0";
        }
        else
        {
            cerr << "pointer x not recognised\n";
        }
    }
    if (tokens[0] == "constant")
    {
        cerr << "constant isn't a memory segment\n";
    }

    tp.segment = segmap[tokens[0]];

    tp.offset = stoi(tokens[1]);
    if (tokens[2].find(":") != string::npos)
    {
        vector<string> range = StringHelper::split(tokens[2], ':');
        tp.value = getRand(stoi(range[0]), stoi(range[1]));
    }
    else
    {
        tp.value = stoi(tokens[2]);
    }

    return tp;
}

void Tester::build()
{
    getUsedMemory();
    parseTestSet();
    writeTests();
    printTests();
}
