#include "StringHelper.h"

vector<string> StringHelper::split(string input, char delimiter)
{
    stringstream ss(input);
    string token;
    vector<string> tokens;

    // Tokenize the testPoints string (adjust delimiter if necessary)
    while (getline(ss, token, delimiter))
    {
        clean(token, ' ');
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

vector<string> StringHelper::splitFile(string filename, char delimiter)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " + filename << std::endl;
        return vector<string>();
    }

    string text((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

    return StringHelper::split(text, '\n');
}

string StringHelper::clean(string str, char ch)
{
    std::string newstring; // Create a new string to hold the result
    for (auto c : str)
    {
        if (c != ch) // Check if the current character is not equal to 'ch'
        {
            newstring.append(1, c); // Append the character to newstring
        }
    }
    return newstring; // Return the cleaned string
}
