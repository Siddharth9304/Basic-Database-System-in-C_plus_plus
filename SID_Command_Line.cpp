#include "database.cpp"

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