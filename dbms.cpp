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

void enterPassword(){
    ifstream fin("password", ios::in | ios::binary);
    if(!fin){
        string password1,password2;
        bool flag = false;
        ofstream fout;
        fout.open("password", ios::app | ios::binary);
        do{
            if(flag)
                cout << "both passwords are differnt. Please set password again.\n\n";
            cout << "create password: ";
            getline(cin,password1);
            cout << "confirm password: ";
            getline(cin,password2);
            flag = true; 
        }while(password1!=password2);
        size_t size = password1.size();
        fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
        fout.write(password1.c_str(), size);
        fout.close();
        cout << endl;
    }
    string password1;
    size_t size;
    fin.read(reinterpret_cast<char*>(&size), sizeof(size));
    string password2(size, '\0');
    fin.read(&password2[0], size);
    fin.close();
    cout << "Enter password: ";
    getline(cin,password1);
    if(password1!=password2)
        exit(0);
}

int getCommandCode(string &input)
{
    int noQueries = queries.size();
    for(int i=0; i<noQueries; i++)
        if(startsWith(input,queries[i]))
            return i;
    return -1;
}

void printAsTable(const vector<string>&);
void printAsTable(const vector<vector<string>>&);

class Table {
private:
    int noRecords = 0;
    int columnCount = 0;
    vector<string> tableStructure;
    vector<vector<string>> rows;

public:
    Table() = default; // Default constructor
    Table(const vector<string>& tab):tableStructure(tab){
        rows.emplace_back(tab);
        columnCount = tab.size();
    }

    // Describe table
    void describe(string tabName) {
        vector<string> temp = rows[0];
        temp.insert(temp.begin(),"Columns_in_"+tabName);
        printAsTable(temp);
        cout << columnCount;
        cout << (columnCount > 1 ? " rows " : " row ");
        cout << "in set ";
    }

    // Insert into table
    void insertInto(const vector<string>& rec) {
        rows.emplace_back(rec);
        noRecords++;
    }

    // Select all from the table
    void selectAll() {
        if(noRecords==0)
            cout << "Empty set ";
        else
        {
            printAsTable(rows);
            cout << noRecords;
            cout << (noRecords > 1 ? " rows " : " row ");
            cout << "in set ";
        }
    }

    // Show a particular column in the table
    void showColumn(const string& colName) {
        int cNo = columnNumber(colName);
        if (cNo == -1)
            throw 0;
        if(noRecords==0)
            cout << "Empty set ";
        else
        {
            vector<string> temp;
            for (const auto& x : rows)
                temp.push_back(x[cNo]);
            printAsTable(temp);
            cout << noRecords;
            cout << (noRecords > 1 ? " rows " : " row ");
            cout << "in set ";
        }
    }

    // to delete a particular records in a caller table
    void deleteRecord(string &colName, string &field)
    {
        int cNo = columnNumber(colName);
        if(cNo == -1)
            throw 0;
        int i,noRecBefDel = noRecords;
        for(i=1; i<=noRecords; )
            if(toLowerCase(rows[i][cNo])==toLowerCase(field))
            {
                rows.erase(rows.begin()+i);
                noRecords--;
            }
            else
                i++;
        cout << "Query OK, " << noRecBefDel-noRecords <<" rows affected ";
        rows.shrink_to_fit();
    }

    // delete all records
    void deleteAllRecords()
    {
        vector<string> temp = rows[0];
        rows.clear();
        rows.emplace_back(temp);
        cout << "Query OK, " << noRecords << " rows affected ";
        noRecords = 0;
        rows.shrink_to_fit();
    }

    // to select particular records in a caller table 
    void selectAllWhere(string &colName, string field)
    {
        int cNo = columnNumber(colName);
        if(cNo == -1)
            throw 0;
        int i;
        vector<vector<string>> temp;
        temp.emplace_back(rows[0]);
        for(i=1; i<=noRecords; i++)
            if(toLowerCase(rows[i][cNo])==toLowerCase(field))
                temp.emplace_back(rows[i]);
        int noRows = temp.size()-1; 
        if(noRows==0)
            cout << "Empty set ";
        else
        {
            printAsTable(temp);
            cout << noRows;
            cout << (noRows > 1 ? " rows " : " row ");
            cout << "in set ";
        }
    }

    // Return number of columns in the table
    int noColumns() {
        return columnCount;
    }

    // Return whether column exists or not
    int columnNumber(const string& colName) {
        for (int i = 0; i < columnCount; i++)
            if (toLowerCase(rows[0][i]) == toLowerCase(colName))
                return i;
        return -1;
    }

    // Serialize
    void serialize(ofstream& out) const {
        //Serialize tableStructure
        size_t numElements = tableStructure.size();
        out.write(reinterpret_cast<const char*>(&numElements), sizeof(numElements));
        for (const auto& element : tableStructure) {
            size_t typeSize = element.size();
            out.write(reinterpret_cast<const char*>(&typeSize), sizeof(typeSize));
            out.write(element.c_str(), typeSize);
        }

        // Serialize noRecords
        out.write(reinterpret_cast<const char*>(&noRecords), sizeof(noRecords));

        // Serialize columnCount
        out.write(reinterpret_cast<const char*>(&columnCount), sizeof(columnCount));

        // Serialize rows
        size_t numRows = rows.size();
        out.write(reinterpret_cast<const char*>(&numRows), sizeof(numRows));
        for (const auto& row : rows) {
            size_t rowSize = row.size();
            out.write(reinterpret_cast<const char*>(&rowSize), sizeof(rowSize));
            for (const auto& value : row) {
                size_t valueSize = value.size();
                out.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
                out.write(value.c_str(), valueSize);
            }
        }
    }

    // Deserialize
    void deserialize(ifstream& in) {
        //Deserialize tableStructure
        size_t numElements;
        in.read(reinterpret_cast<char*>(&numElements), sizeof(numElements));
        tableStructure.resize(numElements);
        for (auto& element : tableStructure) {
            size_t typeSize;
            in.read(reinterpret_cast<char*>(&typeSize), sizeof(typeSize));
            element.resize(typeSize);
            in.read(&element[0], typeSize);
        }

        // Deserialize noRecords
        in.read(reinterpret_cast<char*>(&noRecords), sizeof(noRecords));

        // Deserialize columnCount
        in.read(reinterpret_cast<char*>(&columnCount), sizeof(columnCount));

        // Deserialize rows
        size_t numRows;
        in.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
        rows.resize(numRows);
        for (auto& row : rows) {
            size_t rowSize;
            in.read(reinterpret_cast<char*>(&rowSize), sizeof(rowSize));
            row.resize(rowSize);
            for (auto& value : row) {
                size_t valueSize;
                in.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
                value.resize(valueSize);
                in.read(&value[0], valueSize);
            }
        }
    }
};


// Function to print 2D vector of strings as a table
void printAsTable(const vector<vector<string>>& table) {
    if (table.empty()) {
        cout << "Table is empty!" << endl;
        return;
    }

    // Find the maximum width of each column
    vector<size_t> colWidths(table[0].size(), 0);
    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            colWidths[i] = max(colWidths[i], row[i].length());
        }
    }

    // Print the table with formatting
    for (const auto& row : table) {
        // Print row separator
        for (size_t i = 0; i < colWidths.size(); ++i) {
            cout << "+-" << string(colWidths[i], '-') << "-";
        }
        cout << "+" << endl;

        // Print row content
        for (size_t i = 0; i < row.size(); ++i) {
            cout << "| " << setw(colWidths[i]) << left << row[i] << " ";
        }
        cout << "|" << endl;
    }

    // Print the final row separator
    for (size_t i = 0; i < colWidths.size(); ++i) {
        cout << "+-" << string(colWidths[i], '-') << "-";
    }
    cout << "+" << endl;
}

// Function to print a 1D vector of strings, each string as a separate row in a table format
void printAsTable(const vector<string>& data) {
    if (data.empty()) {
        cout << "Table is empty!" << endl;
        return;
    }

    // Find the maximum width of the longest string (for table formatting)
    size_t colWidth = 0;
    for (const auto& word : data) {
        colWidth = max(colWidth, word.length());
    }

    // Print the table row by row
    for (const auto& word : data) {
        // Print row separator
        cout << "+-" << string(colWidth, '-') << "-+" << endl;

        // Print the word in each row with padding
        cout << "| " << setw(colWidth) << left << word << " |" << endl;
    }

    // Print the final row separator
    cout << "+-" << string(colWidth, '-') << "-+" << endl;
}

class Database {
private:
    static Database* currentDb;   // To point to the open database
    static string currentDatabaseName;
    vector<string> tableNames;
    vector<Table> tables;

    Database() {}
    
    // to check does given table exists in the current database or not
    int tableExists(string tabName)
    {
        auto it = find(tableNames.begin(), tableNames.end(), tabName);
        if (it == tableNames.end())
            return -1;
        return it - tableNames.begin();
    }

public:
    static void createDatabase(const string& dbName) {
        ifstream fin("databases/" + dbName, ios::in | ios::binary);
        if (!fin) {
            ofstream fout("databases/" + dbName, ios::out | ios::binary);
            Database db;
            fout.write(reinterpret_cast<const char*>(&db), sizeof(db));
            fout.close();
                
            fout.open("alldatabases", ios::app | ios::binary);
            size_t size = dbName.size();
            fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
            fout.write(dbName.c_str(), size);
            fout.close();

            cout << "Query OK, 1 row affected ";
        } else {
            fin.close();
            cout << "Can't create database '" << dbName <<"'; database exists ";
        }
    }

    static Database* useDatabase(const string& dbName) {
        ifstream fin("databases/" + dbName, ios::in | ios::binary);
        if (!fin) throw UNKNOWN_DATABASE;

        if (currentDb) delete currentDb;
        currentDb = new Database();

        // Deserialize the Database object
        currentDb->deserialize(fin);
        fin.close();
        currentDatabaseName = dbName;
        cout << "Database changed ";
        return currentDb;
    }

    static void status() {
        cout << "Current database: " << currentDatabaseName << endl;
    }

    // to create a table in the current database
    void createTable(const string &tabName, const vector<string>& tabStruct) {
        if (tableExists(tabName)!=-1)
            cout << "Table '" << tabName << "' already exists ";
        else {
            if(tabStruct.size()==0)
                cout << "A table must have at least one visible column. ";
            else
            {
                Table tab(tabStruct);
                tables.emplace_back(tab);
                tableNames.push_back(tabName);

                ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
                serialize(fout);
                fout.close();
                cout << "Query OK, 0 rows affected ";
            }
        }
    }

    // Show tables in the current database
    void showTables() const {
        int size = tableNames.size();
        if (size){
            vector<string> temp=tableNames;
            temp.insert(temp.begin(),"Tables_in_"+currentDatabaseName);
            printAsTable(temp);
            cout << size;
            cout << (size > 1 ? " rows " : " row ");
            cout << "in set ";
        }
        else
            cout << "Empty set ";
    }

    // Describe a particular table in the current database
    void describeTable(const string& tabName) {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else 
            tables[ind].describe(tabName);
    }

    // Drop a particular table in the current database
    void dropTable(const string& tabName) {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else {
            tableNames.erase(tableNames.begin()+ind);
            tables.erase(tables.begin() + ind);
            ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
            serialize(fout);
            fout.close();
            cout << "Query ok, 1 row affected ";
        }
    }

    // Insert a record into a particular table in the current database
    void insertIntoTable(const string& tabName, const vector<string>& rec) {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else if (tables[ind].noColumns() != rec.size())
            cout << "Column count doesn't match value count ";
        else {
            tables[ind].insertInto(rec);
            ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
            serialize(fout);
            fout.close();
            cout << "Query OK, 1 row affected ";
        }
    }

    // Show all records from a particular table in the current database
    void selectAllFrom(const string& tabName) {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else
            tables[ind].selectAll();
    }

    // Show a particular column of a particular table of the current database
    void selectColumnFrom(const string& tabName, const string& columnName) {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else {
            try {
                tables[ind].showColumn(columnName);
            } catch (int e) {
                cout << "Column '" << columnName << "' doesn't exist in table " << tabName << ' ';
            }
        }
    }

    // to delete a particular record of a particular table in a current database
    void deleteRecordFrom(string tabName, string columnName, string field)
    {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else
        {
            try {
                tables[ind].deleteRecord(columnName,field);;
            } catch (int e) {
                cout << "Column '" << columnName << "' doesn't exist in table " << tabName << ' ';
            }
            ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
            serialize(fout);
            fout.close();
        }
    }

    // to delete all records in one go from a particular table in the current database
    void deleteAllRecFrom(string tabName)
    {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else
        {
            tables[ind].deleteAllRecords();
            ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
            serialize(fout);
            fout.close();
        }
    }

    // to view some records having field same as argument
    void selectAllFromWhere(string tabName, string columnName, string field)
    {
        int ind = tableExists(tabName);
        if (ind==-1)
            cout << "Table '" << currentDatabaseName << "." << tabName << "' doesn't exist ";
        else
        {
            try {
                tables[ind].selectAllWhere(columnName,field);;
            } catch (int e) {
                cout << "Column '" << columnName << "' doesn't exist in table " << tabName << ' ';
            }
            ofstream fout("databases/" + currentDatabaseName, ios::out | ios::binary);
            serialize(fout);
            fout.close();
        }
    }

    // Serialize the Database object
    void serialize(ofstream& out) const {
        // Serialize the number of tables
        size_t numTables = tables.size();
        out.write(reinterpret_cast<const char*>(&numTables), sizeof(numTables));

        // Serialize each table
        for (const auto& table : tables) {
            table.serialize(out);
        }

        // Serialize tableNames
        size_t numTableNames = tableNames.size();
        out.write(reinterpret_cast<const char*>(&numTableNames), sizeof(numTableNames));
        for (const auto& name : tableNames) {
            size_t nameSize = name.size();
            out.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
            out.write(name.c_str(), nameSize);
        }
    }

    // Deserialize the Database object
    void deserialize(ifstream& in) {
        // Deserialize the number of tables
        size_t numTables;
        in.read(reinterpret_cast<char*>(&numTables), sizeof(numTables));

        // Deserialize each table
        tables.resize(numTables);
        for (auto& table : tables) {
            table.deserialize(in);
        }

        // Deserialize tableNames
        size_t numTableNames;
        in.read(reinterpret_cast<char*>(&numTableNames), sizeof(numTableNames));
        tableNames.resize(numTableNames);
        for (auto& name : tableNames) {
            size_t nameSize;
            in.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
            name.resize(nameSize);
            in.read(&name[0], nameSize);
        }
    }
    bool friend dropDatabase(string);
};

Database* Database::currentDb = nullptr;
string Database::currentDatabaseName = "Null";

bool dropDatabase(string dbName)
{
    string path = "databases\\"+dbName;
    ifstream fin;
    fin.open(path,ios::in|ios::binary);
    if(!fin)
        cout << "Can't drop database '" << dbName << "'; database doesn't exist ";
    else
    {
        fin.close();
        fin.open("alldatabases",ios::in|ios::binary);
        if(!fin)
            cout << "alldatabase file has corrupted ";
        else
        {
            ofstream fout;
            fout.open("tempfile",ios::out|ios::binary);
            size_t size1;
            while (fin.read(reinterpret_cast<char*>(&size1), sizeof(size1))) {
                string dbName1(size1, '\0');
                fin.read(&dbName1[0], size1);
                if(dbName1!=dbName)
                {
                    size_t size2 = dbName1.size();      //here is need to make size2 variable size1 and size2 have same values
                    fout.write(reinterpret_cast<const char*>(&size2), sizeof(size2));
                    fout.write(dbName1.c_str(), size2);
                }   
               
            } 
            fin.close();
            fout.close();
            remove("alldatabases");
            rename("tempfile","alldatabases");
            remove(path.c_str());
            cout << "Query OK, 1 row affected ";
            if(Database::currentDatabaseName==dbName)
            {
                delete Database::currentDb;
                Database::currentDb = nullptr;
                Database::currentDatabaseName = "Null";
                return true;
            }
        }
    }
}

void showDatabases()
{
    ifstream fin;
    fin.open("alldatabases",ios::in|ios::binary);
    if(!fin)
        throw NO_ALLDATABASESFILE;
    size_t size;
    vector<string> temp;
    temp.emplace_back("Database");
    while (fin.read(reinterpret_cast<char*>(&size), sizeof(size))) {
        string dbName(size, '\0');
        fin.read(&dbName[0], size);
        temp.emplace_back(dbName);
    }
    fin.close();
    int noDatabases = temp.size()-1;
    if(noDatabases)
    {  
        printAsTable(temp);
        cout << noDatabases;
        cout << (noDatabases==1?" row ":" rows ") << "in set ";
    }
    else 
        cout << "Empty set ";
}

Database *db = nullptr;

int main()
{
    enterPassword();
    cout << endl;
    string input,temp;
    while(1)
    {
        cout << "sid> ";
        getline(cin,input);
        if(input.back()!=';')
        {
            while(temp.back()!=';')
            {
                cout << "  -> ";
                getline(cin,temp);
                input.append(temp);
            }
            temp.clear();
        }
        // Get starting time
        auto start = std::chrono::high_resolution_clock::now();
        
        input.pop_back();
        if(input.back()==';')
            throw SYNTAX_ERROR;
        
        removeExtraSpaces(input);
        
        switch(getCommandCode(input))
        {
            case 0:
                if(input[15]==' ' && countWords(input)==3)
                    Database::createDatabase(toLowerCase(input.substr(16,input.size())));
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(7,8) << "'. Right syntax e.g. 'create database databaseName' ";
                break;
            case 1:
                if(input[13]==' ' && countWords(input)==3)
                    if(dropDatabase(input.substr(14,input.size())))
                        db = nullptr;
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(5,8) << "'. Right syntax e.g. 'drop database databaseName' ";
                break;
            case 2:
                if(input.size()==14)
                    showDatabases();
                else
                    cout << "You have an error in your syntax, do you mean 'show databases'. Please write carefully ";
                break;
            case 3:
                if(input.size()==4)
                    exit(0);
                else
                    cout << "You have an error in your syntax, do you mean 'quit'. Please write carefully ";
            case 4:
                if(input[3]==' ' && countWords(input)==2)
                {
                    try{
                        db = Database::useDatabase(input.substr(4,input.size()));
                    }catch(int){
                        cout << "Unknown database '" << input.substr(4,input.size()) << "' ";;
                    }
                }
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(0,3) << "'. Right syntax e.g. 'use databaseName' ";
                break;
            case 5:
                if(input.size()==6)
                    Database::status();
                else
                    cout << "You have an error in your syntax, do you mean 'status'. Please write carefully ";
                break;
            case 6:
                if(input.size()==11)
                    if(db)
                        db->showTables();
                    else    
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax, do you mean 'show tables'. Please write carefully ";
                break;
            case 7:
            {
                int noWords = countWords(input);
                if(noWords==2)
                    cout << "You have an error in your syntax somewhere after '" << input.substr(7,5) << "'. ";
                else if(noWords==3)
                    cout << "A table must have at least one visible column. ";
                else
                    if(db)
                        db->createTable(toLowerCase(getWord(input,3)),getWordsExcludingFirstThree(input));
                    else
                        cout << "No database selected ";
                break;
            }
            case 8:
                if(input[8]==' ' && countWords(input)==2)
                    if(db)
                        db->describeTable(input.substr(9,input.size()));
                    else
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(0,8) << "'. Right syntax e.g. 'describe tableName'. ";
                break;
            case 9:
            {
                int noWords = countWords(input);
                if(noWords==2)
                    cout << "You have an error in your syntax somewhere after '" << input.substr(7,5) << "' ";
                else
                    if(db)
                        db->insertIntoTable(toLowerCase(getWord(input,3)),getWordsExcludingFirstThree(input));
                    else
                        cout << "No database selected ";
                break;
            }
            case 10:
                if(input[13]==' ' && countWords(input)==4)
                    if(db)
                        db->selectAllFrom(toLowerCase(input.substr(14,input.size())));
                    else
                        cout << "No database selected ";
                else if(input[13]==' ' && countWords(input)==8 && toLowerCase(getWord(input,5))=="where" && toLowerCase(getWord(input,7))=="=")
                    if(db)
                        db->selectAllFromWhere(toLowerCase(getWord(input,4)),getWord(input,6),getWord(input,8));
                    else
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(9,4) << "'. Please write right syntax to select row/rows. ";
                break;
            case 11:
                if(input[18]==' ' && countWords(input)==5)
                    if(db)
                        db->selectColumnFrom(toLowerCase(getWord(input,4)),getWord(input,5));
                    else
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(14,4) << "'; Right syntax e.g. 'select column from tableName columnName'. ";
                break;
            case 12:
                if(input[10]==' ' && countWords(input)==3)
                    if(db)
                        db->dropTable(toLowerCase(input.substr(11,input.size())));
                    else
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(5,5) << "'; Right syntax e.g. 'drop Table tableName'. ";
                break;
            case 13:
                if(input[11]==' ' && countWords(input)==3)
                    if(db)
                        db->deleteAllRecFrom(toLowerCase(input.substr(12,input.size())));
                    else
                        cout << "No database selected ";
                else if(input[11]==' ' && countWords(input)==7 && toLowerCase(getWord(input,4))=="where" && toLowerCase(getWord(input,6))=="=")
                    if(db)
                        db->deleteRecordFrom(toLowerCase(getWord(input,3)),getWord(input,5),getWord(input,7));
                    else
                        cout << "No database selected ";
                else
                    cout << "You have an error in your syntax somewhere after '" << input.substr(7,4) << "'. Please write right syntax to delete row/rows. ";
                break;



            default:
                cout << "You have an error in your syntax; check the manual that corresponds to your SID server version for the right syntax ";
        }
        // Get ending time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate duration
        std::chrono::duration<double> duration = end - start;
        cout << '(' << round(duration.count()*1000.0)/1000.0 << " sec)\n\n";
    } 
}