#pragma once
#include<iostream>
#include<string>
#include<set>
#include<map>
#include "book.h"
#include "account.h"
#include "transaction.h"

class lib{
    protected:
        string name; 
        vector<transaction> history; 
        map<string,account*> all_accounts;
    public:
        set<book> allbooks;
        lib(string n);
        void addtransaction(transaction t);
    public:
        void list_all_books();
        book* find_book(const string& isbn);
    public:
        account* find_account(string username);
        void addaccount(string username,string password,string type);
        void deleteaccount(account* acc);
    public:
        void addbook(book b);
        void borrow_book(account* acc,book* b);
        void return_book(account* acc,book *b);
        void overdued(account * acc);

    public:
        void returnmenu(account* acc);
        void borrowmenu(account* acc);
        void mainpage();
        void homepage(account*);
        void login();
        void signup();
    private:
        void load_all_data();
        void store_all_data();
};