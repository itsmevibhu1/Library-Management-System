#include<iostream>
#include "transaction.h"
using namespace std;
transaction::transaction(string type, string username,string isbn1,time_t time){
            this->type = type; usrname = username; isbn = isbn1 ; borrow_time = time;
        }
void transaction::print_transaction() const {
    cout << "User: " << usrname << endl;
    cout << "Type: " << type << endl;
    cout << "ISBN: " << isbn << endl;

    char buffer[80];
    struct tm * timeinfo = localtime(&borrow_time);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    cout << "Time: " << buffer << endl;
}
string transaction::transaction_to_csv() const {
    char buffer[80];
    struct tm* timeinfo = localtime(&borrow_time);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    string csv_line = type + "," + usrname + "," + isbn + "," + buffer + "\n";
    return csv_line;
}