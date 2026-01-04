#include<iostream>
#include<string>
#include<set>
#include<map>
#include<fstream>
#include "book.h"
#include "account.h"
#include "transaction.h"
#include "student.h"
#include "faculty.h"
#include "librarian.h"
#include "utils.h"
#include "logger.h"
#include "library.h"
using namespace std;
#include <sstream>
lib::lib(string n){
    name = n;
}

void lib::addtransaction(transaction t){
    history.push_back(t);
}

void lib::list_all_books(){
    for(auto it : allbooks){
        it.printbook();
    }
}

book* lib::find_book(const string& isbn){
    auto it = allbooks.find(book("", "", "", "", 0, isbn));
    if(it != allbooks.end()){
        return const_cast<book*>(&(*it));
    }
    cout << "------------------------------------------------------------" << endl;
    cout << "This book is not available in the library." << endl;
    cout << "------------------------------------------------------------" << endl;
    return nullptr;
}

account* lib::find_account(string username){
    if(all_accounts.find(username) == all_accounts.end()){
        return NULL;
    }
    return all_accounts.find(username)->second;
}

void lib::addaccount(string username,string password,string type){
    if(type == "student"){
        account* acc = new student(username,password);
        all_accounts[username] = acc;
    }
    else if(type == "faculty"){
        account* acc = new faculty(username,password);
        all_accounts[username] = acc;
    }
    else if(type == "librarian"){
        account* acc = new student(username,password);
        all_accounts[username] = acc;
    }
    else{
        cout << "------------------------------------------------------------" << endl;
        cout << "Invalid account type." << endl;
        cout << "------------------------------------------------------------" << endl;
    }
}

void lib::deleteaccount(account* acc){
    cout << "------------------------------------------------------------" << endl;
    cout << "Are you sure you want to delete this account? Press 1 to confirm: ";
    int x = getValidIntegerInput("");
    if(x == 1){
        all_accounts.erase(acc->username);
        delete acc;
        cout << "Account deleted successfully." << endl;
    }
    cout << "------------------------------------------------------------" << endl;
}
void lib::addbook(book b){
            allbooks.insert(b);
        }

void lib::borrow_book(account* acc, book* b){
    if(b == NULL){
        cout << "You have not selected a valid book!" << endl;
        return;
    }            
    if(allbooks.find(*b) == allbooks.end()) {
        cout << "This book is not here!" << endl;
        return;
    }
    if(b->status != "Available") {
        cout << "This book is not available at the moment." << endl;
        return;
    }
    if(acc->curr_borrowed.size() >= acc->max_book){
        cout << "You cannot borrow more than " << acc->max_book << " books." << endl;
        return;
    }

    b->printbook();
    b->status = "Not Available";
    time_t curr_time = time(0);
    time_t due_time = calculatetime(curr_time, acc->duedays);
    acc->curr_borrowed.push_back({b, due_time});
    transaction t("borrow", acc->username, b->isbn, curr_time);
    addtransaction(t);
    cout << "------------------------------------------------------------" << endl;
    cout << "Book borrowed successfully!" << endl;
    cout << "------------------------------------------------------------" << endl;
    acc->addtransaction(t);
}

void lib::return_book(account* acc, book *b){            
    if(b == NULL){
        cout << "Invalid book. Please check again!" << endl;
        return;
    }
    if(allbooks.find(*b) == allbooks.end()) {
        cout << "This book is not here!" << endl;
        return;
    }
    
    for (auto it = acc->curr_borrowed.begin(); it != acc->curr_borrowed.end(); it++) {
        if (it->first == b) {
            acc->calculatefine(it->second);
            b->status = "Available";
            time_t curr_time = time(0);
            acc->curr_borrowed.erase(it);
            transaction t("return", acc->username, b->isbn, curr_time);
            addtransaction(t);
            cout << "------------------------------------------------------------" << endl;
            cout << "Book returned successfully!" << endl;
            cout << "------------------------------------------------------------" << endl;
            acc->addtransaction(t);
            break;
        }
    }
}

void lib::overdued(account * acc){
    cout << "------------------------------------------------------------" << endl;
    cout << "Overdue Books:" << endl;
    for(auto it: acc->curr_borrowed){
        it.first->printbook();
    }
    cout << "------------------------------------------------------------" << endl;
}

void lib::returnmenu(account* acc){
    cout << "------------------------------------------------------------" << endl;
    cout << "Here are the books you have borrowed:" << endl;
    for(auto it : acc->curr_borrowed){
        it.first->printbook();
    }
    cout << "------------------------------------------------------------" << endl;
    cout << "Enter the ISBN of the book you want to return: ";
    string isbn;
    cin >> isbn;
    book* b = find_book(isbn);
    return_book(acc, b);
}

void lib::borrowmenu(account* acc){
    cout << "------------------------------------------------------------" << endl;  
    cout << "                     BORROW MENU                           " << endl;  
    cout << "------------------------------------------------------------" << endl;  
    cout << "Available books in the library:" << endl;
    list_all_books();
    cout << endl;
    cout << "Enter the ISBN of the book to borrow (or 0 to exit): ";
    string isbn;
    cin >> isbn;
    if(isbn == "0") return;
    book *b = find_book(isbn);
    borrow_book(acc, b);
}

void lib::mainpage(){
    load_all_data();
    while(true){
        cout << "------------------------------------------------------------" << endl;
        cout << "Welcome to " << name << " Library" << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "1. Login" << endl;
        cout << "2. Signup" << endl;
        cout << "3. Exit" << endl;
        int x = getValidIntegerInput("Select an option: ");
        if(x == 1){
            login();
        }
        else if(x == 2){
            signup();
        }
        else if(x == 3){
            store_all_data();
            break;
        }
        else{
            cout << "Invalid choice!" << endl;
        }
    }
}

void lib::login(){
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    account* acc = find_account(username);
    if(acc == NULL){
        cout << "Invalid account." << endl;
        return;
    }
    cout << "Enter password: ";
    cin >> password;
    if(acc->authenticate(password)){
        homepage(acc);
    }
    else{
        cout << "Incorrect password!" << endl;
    }
}

void lib::signup(){
    string type, username, password;
    cout << "Enter account type (student / faculty / librarian): ";
    while(true){
        cin >> type;
        if(type == "student" || type == "faculty" || type == "librarian"){
            cout << "Enter a username: ";
            while(true){
                cin >> username;
                if(all_accounts.find(username) == all_accounts.end()){
                    cout << "Enter password: ";
                    cin >> password;
                    addaccount(username, password, type);
                    cout << "------------------------------------------------------------" << endl;
                    cout << "Account created successfully!" << endl;
                    cout << "------------------------------------------------------------" << endl;
                    break;
                }
                else{
                    cout << "Username already exists. Try a different one." << endl;
                }
            }
            break;
        }
        else{
            cout << "Invalid account type. Please enter again: ";
        }
    }
}

void lib::homepage(account* acc){
    int x;
    while(true){
        cout << "------------------------------------------------------------" << endl;  
        cout << "                          MAIN MENU                         " << endl;  
        cout << "------------------------------------------------------------" << endl;  
        cout << "Hi, " << acc->username << "!" << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "1. Check Fine" << endl;
        cout << "2. Borrow Book" << endl;
        cout << "3. Return Book" << endl;
        cout << "4. View Overdue Books" << endl;
        cout << "5. View Transaction History" << endl;
        cout << "6. Exit" << endl;
        cout << "7. Delete Your Account" << endl;
        cout << "------------------------------------------------------------" << endl;

        x = getValidIntegerInput("Enter your choice: ");
        if(x == 1){
            cout << "------------------------------------------------------------" << endl;
            cout << "Your current fine is: " << acc->total_fine << endl;
            cout << "------------------------------------------------------------" << endl;
        }
        else if(x==2){
            borrowmenu(acc); 
        }
        else if(x==3){
            cout << "------------------------------------------------------------" << endl;
            cout << "Redirecting to Return Book Menu..." << endl;
            cout << "------------------------------------------------------------" << endl;
            returnmenu(acc);
        }
        else if(x==4){
            cout << "------------------------------------------------------------" << endl;
            cout << "Displaying Overdue Books..." << endl;
            cout << "------------------------------------------------------------" << endl;
            overdued(acc); 
        }
        else if(x==5){
            cout << "------------------------------------------------------------" << endl;
            cout << "Printing Transaction History..." << endl;
            cout << "------------------------------------------------------------" << endl;
            acc->viewtransactions(); 
        }
        else if(x==6){
            cout << "------------------------------------------------------------" << endl;
            cout << "See you later!!!" << endl;
            cout << "------------------------------------------------------------" << endl;
            break;
        }
        else if(x==7){
            deleteaccount(acc);
            break;
        }
        else{
            cout << "------------------------------------------------------------" << endl;
            cout << "Invalid option! Please try again." << endl;
            cout << "------------------------------------------------------------" << endl;
        }
    }
}
void lib::store_all_data(){
    ofstream fout0("data/books.csv"); 
    if (!fout0.is_open()) {
        cerr << "Failed to open books.csv for writing.\n";
        return;
    }
    fout0 << "Genre,Title,Author,Publisher,Year,ISBN,status\n";
    for (const auto& b : allbooks) {
        fout0 << b.booktocsv();
    }
    fout0.close();

    ofstream fout1("data/userdata.csv");
    if(!fout1.is_open()){
        cerr << "Failed to open userdata.csv for writing.\n";
        return;
    }
    for(auto& it : all_accounts){
        vector<string> data = it.second->account_to_csv();
        for(auto& line : data){
            fout1 << line;
        }
    }
    fout1.close();

    ofstream fout("data/history.csv", ios::app);
    if (!fout.is_open()) {
        cerr << "Failed to open history.csv for writing.\n";
        return;
    }
    for(auto& it: history){
        fout << it.transaction_to_csv();
    } 
    fout.close();
}

void lib::load_all_data() {
    // ---------------- Books ----------------
    ifstream fin("data/books.csv");
    if (!fin.is_open()) {
        cerr << "Failed to open books.csv for reading.\n";
        return;
    }

    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        stringstream ss(line);
        string genre, title, author, publisher, yearStr, isbn, status;

        getline(ss, genre, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, publisher, ',');
        getline(ss, yearStr, ',');
        getline(ss, isbn, ',');
        getline(ss, status, '\n');

        int year = stoi(yearStr);
        book b(genre, title, author, publisher, year, isbn);  // ✅ Corrected order
        b.status = status;
        allbooks.insert(b);
    }
    fin.close();

    // ---------------- Accounts ----------------
    ifstream fac("data/userdata.csv");
    if (!fac.is_open()) {
        cerr << "Failed to open userdata.csv for reading.\n";
        return;
    }

    while (getline(fac, line)) {
        stringstream ss(line);
        string username, type, password, borrowCountStr, historyCountStr;
        getline(ss, username, ',');
        getline(ss, type, ',');
        getline(ss, password, ',');
        getline(ss, borrowCountStr, ',');
        getline(ss, historyCountStr, '\n');

        int borrowCount = stoi(borrowCountStr);
        int histCount = stoi(historyCountStr);

        account* acc = nullptr;
        if(type == "student") acc = new student(username, password);
        else if(type == "faculty") acc = new faculty(username, password);
        else if(type == "librarian") acc = new librarian(username, password);
        else {
            cerr << "Unknown account type: " << type << " for user " << username << endl;
            continue;
        }

        // Read current borrowed books
        for (int i = 0; i < borrowCount; ++i) {
            getline(fac, line);
            stringstream bs(line);
            string isbn, timestr;
            getline(bs, isbn, ',');
            getline(bs, timestr, '\n');

            book* b = find_book(isbn);
            if (!b) continue;

            time_t t = stoll(timestr);
            acc->curr_borrowed.push_back({b, t});
        }

        for (int i = 0; i < histCount; ++i) {
            getline(fac, line);
            stringstream ts(line);
            string ttype, usrname, isbn, timestr;
            getline(ts, ttype, ',');
            getline(ts, usrname, ',');
            getline(ts, isbn, ',');
            getline(ts, timestr, '\n');

            struct tm tm{};
            strptime(timestr.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
            time_t time = mktime(&tm);

            transaction t(ttype, usrname, isbn, time);
            acc->history.push(t);
        }

        all_accounts[username] = acc;
    }
    fac.close();
}