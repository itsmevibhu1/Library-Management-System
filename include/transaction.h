#pragma once
#include<iostream>
using namespace std;
class transaction{
    public:
        string type;
        string isbn;
        time_t borrow_time;
        string usrname;
        transaction(string type, string username,string isbn1,time_t time);
        void print_transaction() const;
        string transaction_to_csv() const;
};