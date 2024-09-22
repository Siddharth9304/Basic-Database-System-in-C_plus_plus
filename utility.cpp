#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdio>
#include <tuple>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <chrono>
#include <cmath>

#define UNKNOWN_DATABASE 1
#define DATABASE_EXISTS 2
#define NO_ALLDATABASESFILE 3
#define DATABASE_DOESNOT_EXISTS 4
#define INVALID_COMMAND 5
#define SYNTAX_ERROR 6

using namespace std;

int countWords(const string& str) {
    stringstream ss(str);  // Create a stringstream object
    string word;
    int count = 0;

    // Extract words from the stream
    while (ss >> word) {
        count++;
    }

    return count;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    // Check if the prefix length is greater than the string length
    if (prefix.size() > str.size()) {
        return false;
    }

    // Compare each character in the prefix and the string in a case-insensitive way
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(str[i]) != prefix[i]) {
            return false;
        }
    }

    return true;
}

std::string toLowerCase(const string& str) {
    std::string lowerStr = str;  // Create a copy of the original string
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return lowerStr;
}

void removeExtraSpaces(string& input) {
    stringstream ss(input);
    string word;
    string result;

    // Use a stringstream to extract words, skipping multiple spaces.
    while (ss >> word) {
        if (!result.empty()) {
            result += " ";  // Add a single space between words.
        }
        result += word;    // Append the word.
    }

    // Update the original string with the processed result.
    input = result;
}


string getWord(const string& str, int num) {
    stringstream ss(str);
    string word;
    int wordCount = 0;

    // Extract words from the string stream
    while (ss >> word) {
        wordCount++;
        if (wordCount == num) {
            return word;
        }
    }
    
    return ""; // This line should never be reached due to the guarantee of having more than 2 words.
}

vector<string> getWordsExcludingFirstThree(const string& str) {
    stringstream ss(str);
    string word;
    vector<string> result;
    int wordCount = 0;

    // Extract words from the string stream
    while (ss >> word) {
        wordCount++;
        // Start adding words to the result after the first three words
        if (wordCount > 3) {
            result.push_back(word);
        }
    }
    
    return result;
}


vector<string> queries = {
    "create database",
    "drop database",
    "show databases",
    "quit",
    "use",
    "status",
    "show tables",
    "create table",
    "describe",
    "insert into",
    "select * from",
    "select column from",
    "drop table",
    "delete from",
};

void createPassword(){
    ifstream fin("password", ios::in | ios::binary);
    if(!fin){
        string password1,password2;
        bool flag = false;
        do{
            if(flag)
                cout << "both passwords are differnt. Please set password again.\n\n";
            cout << "create root password: ";
            getline(cin,password1);
            cout << "confirm password: ";
            getline(cin,password2);
            flag = true; 
        }while(password1!=password2);
        ofstream fout;
        fout.open("password", ios::app | ios::binary);
        size_t size = password1.size();
        fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
        fout.write(password1.c_str(), size);
        fout.close();
        fout.open("alldatabases", ios::out | ios::binary);
        fout.close();
        cout << endl;
    }
}

int getCommandCode(string &input)
{
    int noQueries = queries.size();
    for(int i=0; i<noQueries; i++)
        if(startsWith(input,queries[i]))
            return i;
    return -1;
}
