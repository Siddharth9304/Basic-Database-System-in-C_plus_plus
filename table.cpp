#include "utility.cpp"

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