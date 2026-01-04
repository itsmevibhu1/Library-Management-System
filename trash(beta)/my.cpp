#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <iomanip>
#include <limits> // For numeric_limits
using namespace std;
#include "logger.h"
#include <set>
//g++ main.cpp -o myoutput
// ./ output
int getValidIntegerInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) { 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input! Please enter an integer.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return value;
        }
    }
}
time_t calculatetime(time_t curr_time, int days) {
    return curr_time + days * 24 * 60 * 60 - days * 24 * 60 * 60; 
}      

class book{
    public:
        string genre, title, author, publisher, isbn;
        int year;
        string status;
        book(string g, string t, string a, string p, int y, string i){
          genre = g;
          title = t;
          author = a;
          publisher = p;
          isbn = i;
          year = y;
          status = "Available";
        }
        void printbook() const{
            cout<<title<<" | "<<author<<" | "<<publisher<<" | "<<year<<" | "<<isbn<<endl;
        }
        string booktocsv(){
            string s = isbn+','+title+','+author+','+publisher+','+genre+','+status+'\n';
        }
        bool operator==(const book &b) const{
            return (b.genre == genre && b.isbn == isbn && b.year == year && b.publisher == publisher 
            && b.author == author && b.title == title);
        }
        bool operator>(const book &b) const{
            return b.isbn > isbn;
        }
        bool operator<(const book &b) const{
            return b.isbn < isbn;
        }
};

class transaction{
    public:
        string type;
        string isbn;
        time_t borrow_time;
        string usrname;
        transaction(string ttype, string username,string isbn1,time_t time){
            type = ttype; usrname = username; isbn = isbn1 ; borrow_time = time;
        }
        void print_transaction() const {
            cout << "User: " << usrname << endl;
            cout << "Type: " << type << endl;
            cout << "ISBN: " << isbn << endl;

            char buffer[80];
            struct tm * timeinfo = localtime(&borrow_time);
            strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
            cout << "Time: " << buffer << endl;
        }
};
class account{
    public:
        string type = "";
        string password; int total_fine;
        int history_size; queue<transaction> history;
        string username;
        vector<pair<book*,time_t>> curr_borrowed;
        int max_book,duedays,finerate;
    public:
        bool authenticate(string pass){
            return password == pass;
        }
        void addtransaction(transaction t){
            if(history.size()==history_size){
                history.pop();
            }
            history.push(t);
        }
        void calculatefine(time_t due_time) {
            time_t curr_time = time(0);
            int late_days = (curr_time - due_time) / 1;
            if (late_days > 0) {
                total_fine += late_days * finerate;
            }
        }
        void viewtransactions(){
            queue<transaction> q;
            int i = 1;
            while(!history.empty()){
                transaction t = history.front();
                history.pop();
                q.push(t);
                cout<<"Transaction "<<i<<": "<<endl;
                t.print_transaction();
                i++;
            }
            while(!q.empty()){
                transaction t = q.front();
                q.pop();
                history.push(t);
            }
            return;
        }
};

class student : public account{    
    public:
        student(string usrname,string password){
            username = usrname;
            type = "student";
            this -> password = password;
            total_fine = 0;
            max_book = 5;
            duedays = 30;
            finerate = 2;
            history_size = 5;
        }
};
class faculty : public account{    
    public:
        faculty(string usrname,string password){
            type = "faculty";
            username = usrname;
            this->password = password;
            total_fine = 0;
            max_book = INT_MAX;
            duedays = 60;
            finerate = 5;
            history_size = 5;
        }
};



class lib{
    protected:
        string name; 
        vector<transaction> history; 
        map<string,account*> all_accounts;
    public:
        set<book> allbooks;
        
        lib(string n){
            name = n;
        }
        void addtransaction(transaction t){
            history.push_back(t);
        }
    public:
        void list_all_books(){
            for(auto it : allbooks){
                it.printbook();
            }
        }
        book* find_book(const string& isbn){
            auto it = allbooks.find(book("", "", "", "", 0, isbn));
            if(it != allbooks.end()){
                return const_cast<book*>(&(*it));
            }
            cout << "This book is not here in the library" << endl;
            return nullptr;
        }
    public:
        account* find_account(string username){
            if(all_accounts.find(username) == all_accounts.end()){
                return NULL;
            }
            account* acc = all_accounts.find(username)->second;
            return acc;
        }
        void addaccount(string username,string password,string type){
            if(type == "student"){
                account* acc = new student(username,password);
                all_accounts[username]= acc;
            }
            else if(type == "faculty"){
                account* acc = new faculty(username,password);
                all_accounts[username]= acc;
            }
            else if(type == "librarian"){
                account* acc = new student(username,password);
                all_accounts[username]= acc;
            }
            else{
                cout<<"Invalid account type"<<endl;
            }
        }
        void deleteaccount(account* acc){
            cout<<"are you sure you want to delete this account press 1 to confirm?"<<endl;
            int x = getValidIntegerInput("");
            if(x==1){
                all_accounts.erase(acc->username);
                delete acc;
            }
        }
    public:
        void addbook(book b){
            allbooks.insert(b);
        }
        void borrow_book(account* acc,book* b){
            if(b == NULL){
                cout<<"you have not borrowed this !"<<endl;
                return;
            }            
            if(allbooks.find(*b) == allbooks.end()) {cout << "This book is not here !" << endl; return;}
            if(b->status != "Available") {cout << "This book is not avaliable at the moment" <<endl; return;}

            if(acc->curr_borrowed.size() >= acc->max_book){
                cout<<"You cannot borrow more then "<<acc->max_book<<" books"<<endl;
                return;
            }
            b->printbook();
            b->status = "Not Available";
            time_t curr_time = time(0);
            time_t due_time = calculatetime(curr_time,acc->duedays);
            acc->curr_borrowed.push_back({b,due_time});
            transaction t( "borrow" ,acc->username,b->isbn,curr_time);
            addtransaction(t);
            cout<<"haha"<<endl;
            acc->addtransaction(t);
            return;
        }

        void return_book(account* acc,book *b){            
            if(b == NULL){
                cout<<"Invalid book check again!"<<endl;
                return;
            }
            if(allbooks.find(*b) == allbooks.end()) {cout << "This book is not here !" << endl; return;}
            
            for (auto it = acc->curr_borrowed.begin(); it != acc->curr_borrowed.end(); it++) {
                if (it->first == b) {
                    acc->calculatefine(it->second);
                    b->status = "Available";
                    time_t curr_time = time(0);
                    acc->curr_borrowed.erase(it);
                    transaction t( "return" ,acc->username,b->isbn,curr_time);
                    addtransaction(t);
                    cout<<"haha"<<endl;
                    acc->addtransaction(t);
                    break;
                }
            }
            return;
        }
        void overdued(account * acc){
            for(auto it: acc->curr_borrowed){
                it.first->printbook();
            }
        }
    public:
        void returnmenu(account* acc){
            cout<<"here are the books that you borrowed"<<endl;
            for(auto it : acc->curr_borrowed){
                it.first->printbook();
            }
            cout<<"enter the isbn of the book you want to return"<<endl;
            string isbn;
            cin>>isbn;
            book* b = find_book(isbn);
            return_book(acc,b);
        }

        void borrowmenu(account* acc){
            cout<<"--------------------------------------------------------"<<endl;  
            cout<<"-----------------------BORROW MENU----------------------"<<endl;  
            cout<<"--------------------------------------------------------"<<endl;  
            cout<<"Here are the books avaliable at our libaray!"<<endl;
            list_all_books();
            cout<<endl;
            cout<<"Now,Enter the book's isbn you want to borrow or enter 0 if you want to exit!"<<endl;
            string isbn;
            cin>>isbn;
            if(isbn == "0") return;
            book *b = find_book(isbn);
            borrow_book(acc,b);
            return;
        }
        void helloworld(){
            while(true){
                cout<<"Welcome to "<<name<<" Library"<<endl;
                cout<<"Press 1: to login"<<endl;
                cout<<"Press 2: to signup"<<endl;
                cout<<"Press 3: to exit"<<endl;
                int x;
                x = getValidIntegerInput("Select a choice");
                if(x==1){
                    login();
                }
                else if(x==2){
                    signup();
                }
                else if(x==3){
                    break;
                }
                else{
                    cout<<"Invalid choice !"<<endl;
                }
            }
        }
        void login(){
            string username,password;
            cout<<"enter username"<<endl;
            cin>>username;
            account* acc = find_account(username);
            if(acc == NULL){
                cout<<"Invalid account"<<endl;
                return;
            }
            cout<<"enter password"<<endl;
            cin>>password;
            if(acc->authenticate(password)){
                mainpage(acc);
            }
            else{
                cout<<"incorect password";
                return;
            }
        }   
        void signup(){
            string type,username,password;
            cout<<"enter account type";
            while(true){
                cin>>type;
                if(type == "student" || type =="faculty" || type == "librarian"){
                    cout<<"enter a username"<<endl;
                    while(true){
                        cin>>username;
                        if(all_accounts.find(username)==all_accounts.end()){
                            cout<<"enter password"<<endl;
                            cin>> password;
                            addaccount(username,password,type);
                            cout<<"account created"<<endl;
                            break;
                        }
                        else{
                            cout<<"username already exists try different one"<<endl;
                        }
                    }
                    break;
                }
                else{
                    cout<<"Invalid type"<<endl;
                }
            }
            
        }
        void mainpage(account* acc){
            cout<<"--------------------------------------------------------"<<endl;  
            cout<<"-----------------------MAIN MENU----------------------"<<endl;  
            cout<<"--------------------------------------------------------"<<endl;  
            cout<< "Hi, "<<acc->username<<acc->type<<"!"<<endl;
            cout<<"----------------------------------------" <<endl;
            cout<<"Press 1: to check fine" <<endl;
            cout<<"Press 2: to borrow book" <<endl;
            cout<<"Press 3: to return book" <<endl;
            cout<<"Press 4: to view overdued books" <<endl;
            cout<<"Press 5: to view history"<<endl;
            cout<<"Press 6: to exit"<<endl;
            cout<<"Press 7: to delete you account"<<endl;
            int x;
            while(true){
                x = getValidIntegerInput("");
                if(x == 1){
                    cout<<"your current fine is "<<acc->total_fine<<endl;
                    
                }
                else if(x==2){
                    borrowmenu(acc); 
                    
                }
                else if(x==3){
                    cout<<"taking you to the returnbook menu"<<endl;  
                    returnmenu(acc);
                    
                }
                else if(x==4){
                     cout<<"taking you to the overduebook menu"<<endl; 
                     overdued(acc); 
                     
                }
                else if(x==5){
                     cout<<"printing history"<<endl; 
                     acc->viewtransactions(); 
                
                }
                else if(x==6){
                    break;
                }
                else if(x==7){
                    deleteaccount(acc);
                    break;
                }
                else{
                    cout<<"enter a valid option"<<endl;
                }
            }
        }
};

class librarian : public account{
    private:
        lib* l;
    public:
        librarian(string usrname,string password){
                username = usrname;
                type = "student";
                this -> password = password;
                total_fine = 0;
                max_book = 5;
                duedays = 30;
                finerate = 2;
                history_size = 5;
            }
    public:
        void addbook(book &b){
            l->allbooks.insert(b);
        }
        void deletebook(book &b){
            if(l->allbooks.find(b) != l->allbooks.end()){
                l->allbooks.erase(b);
            }
            return;
        }
};
int main(){
    lib ml("PK kelkar");
    ml.addaccount("sahil","haha","student");
    book b1("Fiction", "The Alchemist", "Paulo Coelho", "HarperOne", 1988, "ISBN001");
    book b2("Sci-Fi", "Dune", "Frank Herbert", "Chilton Books", 1965, "ISBN002");
    book b3("Mystery", "Gone Girl", "Gillian Flynn", "Crown Publishing", 2012, "ISBN003");
    book b4("Fantasy", "Harry Potter", "J.K. Rowling", "Bloomsbury", 1997, "ISBN004");
    book b5("Non-Fiction", "Sapiens", "Yuval Harari", "Harvill Secker", 2011, "ISBN005");
    book b6("Horror", "The Shining", "Stephen King", "Doubleday", 1977, "ISBN006");
    book b7("Romance", "Pride and Prejudice", "Jane Austen", "T. Egerton", 1813, "ISBN007");
    book b8("Biography", "Long Walk to Freedom", "Nelson Mandela", "Little, Brown", 1994, "ISBN008");
    book b9("Philosophy", "The Republic", "Plato", "Penguin Classics", -380, "ISBN009");
    book b10("Tech", "Clean Code", "Robert C. Martin", "Prentice Hall", 2008, "ISBN010");
    ml.addbook(b1);
    ml.addbook(b2);
    ml.addbook(b3);
    ml.addbook(b4);
    ml.addbook(b5);
    ml.addbook(b6);
    ml.addbook(b7);
    ml.addbook(b8);
    ml.addbook(b9);
    ml.addbook(b10);
    ml.list_all_books();
    ml.helloworld();

}
