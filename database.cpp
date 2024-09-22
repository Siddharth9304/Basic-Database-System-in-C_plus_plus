#include "table.cpp"

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