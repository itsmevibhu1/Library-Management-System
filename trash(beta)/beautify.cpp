#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <iomanip>
#include <limits>
#include <queue>
#include <set>
using namespace std;

int getValidIntegerInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) { 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter an integer." << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return value;
        }
    }
}

time_t calculatetime(time_t curr_time, int days) {
    return curr_time + days * 24 * 60 * 60;
}      

class lib;
class account;
class student;
class faculty;

class book {
public:
    string genre, title, author, publisher, isbn;
    int year;
    string status;
    book(string g, string t, string a, string p, int y, string i) {
        genre = g;
        title = t;
        author = a;
        publisher = p;
        isbn = i;
        year = y;
        status = "Available";
    }
    void printbook() const {
        cout << title << " | " << author << " | " << publisher << " | " << year << " | " << isbn << endl;
    }
    bool operator==(const book &b) const {
        return (b.genre == genre && b.isbn == isbn && b.year == year && b.publisher == publisher 
            && b.author == author && b.title == title);
    }
    bool operator>(const book &b) const {
        return b.isbn > isbn;
    }
    bool operator<(const book &b) const {
        return b.isbn < isbn;
    }
};

class transaction {
public:
    string type;
    string isbn;
    time_t borrow_time;
    string usrname;
    transaction(string ttype, string username, string isbn1, time_t time) {
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

class account {
public:
    string type = "";
    string password;
    int total_fine;
    int history_size;
    queue<transaction> history;
    string username;
    vector<pair<book*, time_t>> curr_borrowed;
    int max_book, duedays, finerate;
    lib* library = NULL;

    bool authenticate(string pass) {
        return password == pass;
    }

    void addtransaction(transaction t) {
        if(history.size() == history_size) {
            history.pop();
        }
        history.push(t);
    }

    void calculatefine(time_t due_time) {
        time_t curr_time = time(0);
        int late_days = (curr_time - due_time) / (60 * 60 * 24);
        if (late_days > 0) {
            total_fine += late_days * finerate;
        }
    }

    void viewtransactions() {
        queue<transaction> q;
        int i = 1;
        while(!history.empty()) {
            transaction t = history.front();
            history.pop();
            q.push(t);
            cout << "---------------- Transaction " << i << " ----------------" << endl;
            t.print_transaction();
            i++;
        }
        while(!q.empty()) {
            transaction t = q.front();
            q.pop();
            history.push(t);
        }
    }
};

class student : public account {
public:
    student(string usrname, string password, lib* l) {
        username = usrname;
        type = "student";
        this->password = password;
        total_fine = 0;
        max_book = 5;
        duedays = 30;
        finerate = 2;
        library = l;
        history_size = 5;
    }
};

class faculty : public account {
public:
    faculty(string usrname, string password, lib* l) {
        username = usrname;
        type = "faculty";
        this->password = password;
        total_fine = 0;
        max_book = INT_MAX;
        duedays = 60;
        finerate = 5;
        library = l;
        history_size = 5;
    }
};

class lib {
private:
    string name;
    set<book> allbooks;
    vector<transaction> history;
    map<string, account*> all_accounts;

public:
    lib(string n) {
        name = n;
    }

    void addtransaction(transaction t) {
        history.push_back(t);
    }

    void list_all_books() {
        cout << "---------------- All Books ----------------" << endl;
        for(auto it : allbooks) {
            it.printbook();
        }
        cout << "-------------------------------------------" << endl;
    }

    book* find_book(const string& isbn) {
        auto it = allbooks.find(book("", "", "", "", 0, isbn));
        if(it != allbooks.end()) {
            return const_cast<book*>(&(*it));
        }
        cout << "This book is not in the library." << endl;
        return nullptr;
    }

    account* find_account(string username) {
        if(all_accounts.find(username) == all_accounts.end()) {
            return NULL;
        }
        return all_accounts.find(username)->second;
    }

    void addaccount(string username, string password, string type) {
        if(type == "student") {
            account* acc = new student(username, password, this);
            all_accounts[username] = acc;
        } else if(type == "faculty") {
            account* acc = new faculty(username, password, this);
            all_accounts[username] = acc;
        } else {
            cout << "Invalid account type." << endl;
        }
    }

    void addbook(book b) {
        allbooks.insert(b);
    }

    void borrow_book(account* acc, book* b) {
        if(b == NULL) {
            cout << "You have not borrowed this book." << endl;
            return;
        }
        if(allbooks.find(*b) == allbooks.end()) {
            cout << "This book is not in the library." << endl;
            return;
        }
        if(b->status != "Available") {
            cout << "This book is currently not available." << endl;
            return;
        }
        if(acc->curr_borrowed.size() >= acc->max_book) {
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
        acc->addtransaction(t);
        cout << "Book borrowed successfully." << endl;
    }

    void return_book(account* acc, book* b) {
        if(b == NULL) {
            cout << "Invalid book. Check ISBN." << endl;
            return;
        }
        if(allbooks.find(*b) == allbooks.end()) {
            cout << "This book is not in the library." << endl;
            return;
        }
        for (auto it = acc->curr_borrowed.begin(); it != acc->curr_borrowed.end(); it++) {
            if (it->first == b) {
                acc->calculatefine(it->second);
                b->status = "Available";
                acc->curr_borrowed.erase(it);
                transaction t("return", acc->username, b->isbn, time(0));
                addtransaction(t);
                acc->addtransaction(t);
                cout << "Book returned successfully." << endl;
                return;
            }
        }
        cout << "This book was not found in your borrowed list." << endl;
    }

    void overdued(account* acc) {
        cout << "Overdue books:" << endl;
        for(auto it: acc->curr_borrowed) {
            it.first->printbook();
        }
    }

    void returnmenu(account* acc) {
        cout << "---------------- Return Menu ----------------" << endl;
        cout << "Books you have borrowed:" << endl;
        for(auto it : acc->curr_borrowed) {
            it.first->printbook();
        }
        cout << "Enter the ISBN of the book to return: ";
        string isbn;
        cin >> isbn;
        book* b = find_book(isbn);
        return_book(acc, b);
    }

    void borrowmenu(account* acc) {
        cout << "---------------- Borrow Menu ----------------" << endl;
        list_all_books();
        cout << "Enter the ISBN of the book to borrow or 0 to cancel: ";
        string isbn;
        cin >> isbn;
        if(isbn == "0") return;
        book* b = find_book(isbn);
        borrow_book(acc, b);
    }

    void login() {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        account* acc = find_account(username);
        if(acc == NULL) {
            cout << "Invalid account." << endl;
            return;
        }
        cout << "Enter password: ";
        cin >> password;
        if(acc->authenticate(password)) {
            mainpage(acc);
        } else {
            cout << "Incorrect password." << endl;
        }
    }

    void signup() {
        string type, username, password;
        cout << "Enter account type (student/faculty): ";
        while(true) {
            cin >> type;
            if(type == "student" || type == "faculty") {
                cout << "Enter a username: ";
                while(true) {
                    cin >> username;
                    if(all_accounts.find(username) == all_accounts.end()) {
                        cout << "Enter password: ";
                        cin >> password;
                        addaccount(username, password, type);
                        cout << "Account created successfully." << endl;
                        return;
                    } else {
                        cout << "Username already exists. Try again: ";
                    }
                }
            } else {
                cout << "Invalid type. Try again: ";
            }
        }
    }

    void mainpage(account* acc) {
        while(true) {
            cout << "\n---------------- Main Menu ----------------" << endl;
            cout << "Hello, " << acc->username << " (" << acc->type << ")" << endl;
            cout << "1. Check Fine" << endl;
            cout << "2. Borrow Book" << endl;
            cout << "3. Return Book" << endl;
            cout << "4. View Overdue Books" << endl;
            cout << "5. View Transaction History" << endl;
            cout << "6. Logout" << endl;
            int x = getValidIntegerInput("Enter your choice: ");
            if(x == 1) {
                cout << "Your current fine is: Rs. " << acc->total_fine << endl;
            } else if(x == 2) {
                borrowmenu(acc);
            } else if(x == 3) {
                returnmenu(acc);
            } else if(x == 4) {
                overdued(acc);
            } else if(x == 5) {
                acc->viewtransactions();
            } else if(x == 6) {
                cout << "Logged out successfully." << endl;
                break;
            } else {
                cout << "Invalid option. Try again." << endl;
            }
        }
    }

    void helloworld() {
        while(true) {
            cout << "\n========== Welcome to " << name << " Library ==========" << endl;
            cout << "1. Login" << endl;
            cout << "2. Sign Up" << endl;
            cout << "3. Exit" << endl;
            int x = getValidIntegerInput("Choose an option: ");
            if(x == 1) {
                login();
            } else if(x == 2) {
                signup();
            } else if(x == 3) {
                cout << "Exiting. Goodbye!" << endl;
                break;
            } else {
                cout << "Invalid choice. Try again." << endl;
            }
        }
    }
};

int main() {
    lib ml("PK Kelkar");

    ml.addaccount("sahil", "haha", "student");

    ml.addbook(book("Fiction", "The Alchemist", "Paulo Coelho", "HarperOne", 1988, "ISBN001"));
    ml.addbook(book("Sci-Fi", "Dune", "Frank Herbert", "Chilton Books", 1965, "ISBN002"));
    ml.addbook(book("Mystery", "Gone Girl", "Gillian Flynn", "Crown Publishing", 2012, "ISBN003"));
    ml.addbook(book("Fantasy", "Harry Potter", "J.K. Rowling", "Bloomsbury", 1997, "ISBN004"));
    ml.addbook(book("Non-Fiction", "Sapiens", "Yuval Harari", "Harvill Secker", 2011, "ISBN005"));
    ml.addbook(book("Horror", "The Shining", "Stephen King", "Doubleday", 1977, "ISBN006"));
    ml.addbook(book("Romance", "Pride and Prejudice", "Jane Austen", "T. Egerton", 1813, "ISBN007"));
    ml.addbook(book("Biography", "Long Walk to Freedom", "Nelson Mandela", "Little, Brown", 1994, "ISBN008"));
    ml.addbook(book("Philosophy", "The Republic", "Plato", "Penguin Classics", -380, "ISBN009"));
    ml.addbook(book("Tech", "Clean Code", "Robert C. Martin", "Prentice Hall", 2008, "ISBN010"));

    ml.helloworld();
}