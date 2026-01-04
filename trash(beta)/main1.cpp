#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <iomanip>
#include <limits> // For numeric_limits

using namespace std;

// Helper function to validate integer inputs
int getValidIntegerInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) { // Check if input is not an integer
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input! Please enter an integer.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the buffer
            return value;
        }
    }
}

// Function to format a timestamp into a human-readable string
// Input: `timestamp` (time_t) - The timestamp to format (seconds since epoch)
// Output: `string` - The formatted date and time string in "dd/mm/yyyy HH:MM:SS" format
string formatTimestamp(time_t timestamp) {
    char buffer[20];
    struct tm* timeinfo = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Function to parse a date-time string into a timestamp (time_t)
// Input: `datetime` (string) - The date-time string in "dd/mm/yyyy HH:MM:SS" format
// Output: `time_t` - The timestamp (seconds since epoch)
time_t parseTimestamp(const string& datetime) {
    struct tm timeinfo = {};
    sscanf(datetime.c_str(), "%d/%d/%d %d:%d:%d", 
           &timeinfo.tm_mday, &timeinfo.tm_mon, &timeinfo.tm_year, 
           &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);
    timeinfo.tm_mon -= 1;         // tm_mon is 0-based (0 = Jan)
    timeinfo.tm_year -= 1900;     // tm_year is years since 1900
    return mktime(&timeinfo);
}

class Book {
public:
    string genre, title, author, publisher, isbn;
    int year;
    string status;

    Book(string g, string t, string a, string p, int y, string i, string s = "Available")
        : genre(g), title(t), author(a), publisher(p), year(y), isbn(i), status(s) {}

    string toCSV() const {
        return genre + "," + title + "," + author + "," + publisher + "," + to_string(year) + "," +
               isbn + "," + status + "\n";
    }
};

class Account {
    private:
        string userType;
        // borrowedBooks stores all information about the borrowed books of a particular user, which includes currently borrowed books and past borrowing history
        vector<tuple<string, string, string, string, string, string, int>> borrowedBooks;
        // Each tuple in the vector represents a borrowed book and contains the following elements:
        // 1. string: ISBN of the book
        // 2. string: Title of the book
        // 3. string: Borrowed date (in "dd/mm/yyyy HH:MM:SS" format)
        // 4. string: Returned date (in "dd/mm/yyyy HH:MM:SS" format, "Not Returned" if not yet returned)
        // 5. string: Due date (in "dd/mm/yyyy HH:MM:SS" format)
        // 6. string: Status of the book ("Not Returned", "Returned", or "Overdue")
        // 7. int: Fine incurred (in rupees, 0 if no fine)
        
        int fines;
    
    public:
        Account(string type) : userType(type), fines(0) {}
    
        void borrowBook(string isbn, string title) {
            int borrowDays = (userType == "Student") ? 15 : 30;

            time_t now = time(0); // Get the current time as a timestamp
            struct tm* dueTm = localtime(&now); // Convert the timestamp to local time

            // Calculate the due date by adding the borrowing period to the current date
            dueTm->tm_mday += borrowDays;  // Add the borrowing period (in days)
            dueTm->tm_hour = 23;           // Set the due time to 23:59:59
            dueTm->tm_min = 59;
            dueTm->tm_sec = 59;
            time_t dueTime = mktime(dueTm); // Convert the modified tm struct back to a timestamp

            // Format the borrowed and due dates into strings
            string borrowedDate = formatTimestamp(now); // Format the current time
            string dueDate = formatTimestamp(dueTime);  // Format the due time
        
            // Add the book to the borrowedBooks list
            borrowedBooks.push_back(make_tuple(isbn, title, borrowedDate, "Not Returned", dueDate, "Not Returned", 0));
        }
        
        int calculateDifferenceInDays(const string& date1, const string& date2) {
            // Parse both dates into tm structures
            struct tm tm1 = {};
            struct tm tm2 = {};
    
            sscanf(date1.c_str(), "%d/%d/%d", &tm1.tm_mday, &tm1.tm_mon, &tm1.tm_year);
            sscanf(date2.c_str(), "%d/%d/%d", &tm2.tm_mday, &tm2.tm_mon, &tm2.tm_year);
    
            // Adjust tm_mon and tm_year for mktime
            tm1.tm_mon -= 1;    // tm_mon is 0-based (0 = January)
            tm1.tm_year -= 1900; // tm_year is years since 1900
            tm2.tm_mon -= 1;
            tm2.tm_year -= 1900;
    
            // Convert tm structures to time_t (seconds since epoch)
            time_t time1 = mktime(&tm1);
            time_t time2 = mktime(&tm2);
    
            // Calculate the difference in seconds
            double secondsDiff = difftime(time1, time2);
    
            // Convert the difference to days, since we are only interested in the whole number of days
            int daysDiff = static_cast<int>(secondsDiff / 86400);
    
            return daysDiff;
        }
    
        bool returnBook(string isbn, string userType) {
            time_t now = time(0); // Get current time
            string returnedDate = formatTimestamp(now);
            string currentDate = formatTimestamp(now).substr(0, 10); // Extract "dd/mm/yyyy" from timestamp

            bool valid = 0;
            for (auto &book : borrowedBooks) {
                if (get<0>(book) == isbn && get<5>(book) != "Returned") {
                    valid = 1;
                    get<3>(book) = returnedDate;
                    get<5>(book) = "Returned";
                    
                    // Fine Calculation: Only if returned after 23:59:59 of due date AND user is a Student
                    string dueDate = get<4>(book).substr(0, 10); // Extract "dd/mm/yyyy" from due date

                    // Calculate difference in days
                    int overdueDays = calculateDifferenceInDays(currentDate, dueDate);

                    cout << "\nBook returned successfully!"<< endl;

                    if (userType == "Student") {
                        if (overdueDays > 0) {
                            int fine = overdueDays * 10; // ₹10 per day fine
                            get<6>(book) = fine; // Update the fine
                            fines += fine;
                            cout << "Overdue period: " << overdueDays << " days" << endl;
                            cout << "Fine incurred: Rs " << fine << endl;
                            cout << "Please return books on time in the future.\n";
                        }
                        else {
                            cout << "Overdue period: 0 days" << endl;
                            cout << "Fine incurred: Rs 0" << endl;
                        }
                    }

                    if (userType == "Faculty") {
                        cout << "Overdue period: " << max(0,overdueDays) << " days" << endl;
                        if (overdueDays > 0) cout << "Please return books on time in the future.\n";
                    }

                    return valid;
                }
            }
            return valid;
        }        
        
    
        bool hasBook(string isbn) {
            for (const auto &book : borrowedBooks) {
                if (get<0>(book) == isbn) {
                    return true;
                }
            }
            return false;
        }
    
        int getFine() const { return fines; }
        void setFine(int amount) { fines = amount; }


        // Returns a string containing the borrowed books in CSV format
        string getBorrowedBooks() const {
            string result;
            for (const auto& book : borrowedBooks) {
                result += get<0>(book) + "|" + get<1>(book) + "|" + get<2>(book) + "|" + get<3>(book) + "|" + 
                        get<4>(book) + "|" + get<5>(book) + "|" + to_string(get<6>(book)) + ";";
            }
            return result;
        }

        vector<tuple<string, string, string, string, string, string, int>> getBorrowedBooksList() const {
            return borrowedBooks;
        } 

        // Load the borrowed books from a CSV string
        void loadFromCSV(const string& books, int fineAmount) {
            borrowedBooks.clear();
            stringstream ss(books);
            string bookEntry;
            
            while (getline(ss, bookEntry, ';')) {
                if (bookEntry.empty()) continue;
                stringstream bookStream(bookEntry);
                string isbn, title, borrowedDate, returnStatus, dueDate, status;
                int fine;
                
                getline(bookStream, isbn, '|');
                getline(bookStream, title, '|');
                getline(bookStream, borrowedDate, '|');
                getline(bookStream, returnStatus, '|');
                getline(bookStream, dueDate, '|');
                getline(bookStream, status, '|');
                bookStream >> fine;
                
                borrowedBooks.emplace_back(isbn, title, borrowedDate, returnStatus, dueDate, status, fine);
            }
            fines = fineAmount;
        }

        // Returns the past borrowing history of the user - only returned books are included
        vector<tuple<string, string, string, string, string, string, int>> getPastBorrowedBooksList() const {
            vector<tuple<string, string, string, string, string, string, int>> pastBorrowedBooks;
            for (const auto& book : borrowedBooks) {
                if (get<5>(book) == "Returned") {
                    pastBorrowedBooks.push_back(book);
                }
            }
            return pastBorrowedBooks;
        }
        
        // Returns the number of books currently borrowed by the user
        int getCurrentlyBorrowedCount() const {
            int count = 0;
            for (const auto& book : borrowedBooks) {
                if (!(get<5>(book) == "Returned")) {
                    count++;
                }
            }
            return count;
        }
        
        // For real-time checking of fines of students
        void calculateFines() {
            int totalFine = 0;
            time_t now = time(0); // Get current time
            string currentDate = formatTimestamp(now).substr(0, 10); // Extract "dd/mm/yyyy" from timestamp
    
            for (auto &book : borrowedBooks) {
                if (!(get<5>(book) == "Returned")) {
                    string dueDate = get<4>(book).substr(0, 10); // Extract "dd/mm/yyyy" from due date
    
                    // Calculate difference in days
                    int overdueDays = calculateDifferenceInDays(currentDate, dueDate);
    
                    if (overdueDays > 0 && userType == "Student") {
                        int fine = overdueDays * 10; // ₹10 per day fine
                        get<6>(book) = fine;
                        get<5>(book) = "Overdue";
                        totalFine += fine;
                    }
                }
            }
            fines = totalFine;
        }

        // For real-time checking of overdue books of faculty
        bool hasOverdueBooksForMoreThan60Days() {
            time_t now = time(0); // Get current time
            string currentDate = formatTimestamp(now).substr(0, 10); // Extract "dd/mm/yyyy" from timestamp
    
            for (const auto& book : borrowedBooks) {
                if (get<5>(book) == "Not Returned" || get<5>(book) == "Overdue") {
                    string dueDate = get<4>(book).substr(0, 10); // Extract "dd/mm/yyyy" from due date
    
                    // Calculate difference in days
                    int overdueDays = calculateDifferenceInDays(currentDate, dueDate);
    
                    if (overdueDays > 60) {
                        return true;
                    }
                }
            }
            return false;
        }

        // For real-time checking of overdue books
        bool hasOverdueBooks() {
            time_t now = time(0);
            for (const auto &book : borrowedBooks) {
                if (!(get<5>(book) == "Returned")) {
                    time_t dueTime = parseTimestamp(get<4>(book));
                    if (now > dueTime) {
                        return true;
                    }
                }
            }
            return false;
        }

        // For returning overdue books
        vector<tuple<string, string, string, string, string, string, int>> getOverdueBooks() {
            vector<tuple<string, string, string, string, string, string, int>> overdueBooks;
            time_t now = time(0);
            for (auto &book : borrowedBooks) {
                if (!(get<5>(book) == "Returned")) {
                    time_t dueTime = parseTimestamp(get<4>(book));
                    if (now > dueTime) {
                        // Update status to "Overdue"
                        get<5>(book) = "Overdue";
                        overdueBooks.push_back(book);
                    }
                }
            }
            return overdueBooks;
        }
    };

class User {
    protected:
        int user_id;
        string username, password, role;
        string name; 
        int age;     
        string gender; 
        Account account;
    
    public:
        User(int id, string u, string p, string r, const string& borrowedBooks, int fineAmount, string n = "", int a = 0, string g = "")
            : user_id(id), username(u), password(p), role(r), name(n), age(a), gender(g), account(r) {
            account.loadFromCSV(borrowedBooks, fineAmount);
        }

        virtual bool checkCanBorrow() = 0;
        virtual void borrowBook(Book& book) = 0;
        virtual void returnBook(Book& book) = 0;
        virtual string getRole() const = 0;
    
        bool authenticate(string u, string p) {
            return (u == username && p == password);
        }
    
        Account& getAccount() { return account; }
        virtual void showDetails() const = 0;
    
        int getUserID() const { return user_id; }
        string getUsername() const { return username; }
        string getPassword() const { return password; }
        string getName() const { return name; }
        int getAge() const { return age; }
        string getGender() const { return gender; }

        // To convert user data to CSV string
        string toCSV() const {
            return to_string(user_id) + "," + username + "," + password + "," + role + "," +
                    name + "," + to_string(age) + "," + gender + "," +
                    account.getBorrowedBooks() + "," + to_string(account.getFine()) + "\n";
        }
    
        void update_user_id(int id) { 
            user_id = id; 
        }
    
        // For updating user profile
        virtual void editProfile() {
            string currentPassword, newUsername, newPassword, newName, newGender;
            int newAge;

            cout << "\nEnter new username: ";
            cin >> newUsername;

            // Ask for the current password
            cout << "Enter your current password: ";
            cin >> currentPassword;
    
            // Verify the current password
            if (currentPassword != password) {
                cout << "\nIncorrect current password. Profile update failed.\n";
                return;
            }

            cout << "Password verified!\n";
            
            // If the current password is correct, proceed with the update
            cout << "Enter new password: ";
            cin >> newPassword;

            cout << "Enter new full name: ";
            cin.ignore(); // To handle the newline character left by previous input
            getline(cin, newName); // Allow spaces in the name

            age = getValidIntegerInput("Enter new age: "); // Defensive programming to ensure int input
            
            while (true) {
                cout << "Enter gender (M/F/O): ";
                cin >> newGender;
                if (newGender == "M" || newGender == "F" || newGender == "O") { //Defensive programming
                    break;
                } else {
                    cout << "Invalid gender! Please enter M, F, or O.\n";
                }
            }
    
            // Update the fields
            username = newUsername;
            password = newPassword;
            name = newName;
            age = newAge;
            gender = newGender;
    
            cout << "\nProfile updated successfully!\n";
        }
};

class Student : public User {
    public:
    Student(int id, string u, string p, const string& borrowedBooks, int fineAmount,string n = "", int a = 0, string g = "") 
            : User(id, u, p, "Student", borrowedBooks, fineAmount, n, a, g) {}

    string getRole() const override { 
        return "Student"; 
    }

    bool checkCanBorrow() override {
        if (account.getFine() > 0) {
            cout << "\nYou have unpaid fines. Please clear your fines to borrow books.\n"; //To prevent borrowing until fines are cleared
            return false;
        }
        if (account.getCurrentlyBorrowedCount() >= 3) {
            cout << "Borrow limit reached (3 books max)!\nSorry, you cannot borrow more books.\n"; //To prevent borrowing more than 3 books
            return false;
        }
        return true;
    }

    void borrowBook(Book& book) override {
        if (book.status == "Reserved") {
            cout << "\nSorry, this book is RESERVED and cannot be borrowed!\n"; //We have assumed that reserved books cannot be borrowed
            return;
        }
        if (book.status != "Available") {
            cout << "\nSorry, book is NOT AVAILABLE!\n";
            return;
        }
        account.borrowBook(book.isbn, book.title);
        book.status = "Borrowed";
        cout << "Book borrowed successfully!\n";
    }

    void returnBook(Book& book) override {
        if (!account.hasBook(book.isbn)) {
            cout << "You didn't borrow this book!\n";
            return;
        }
        bool valid = account.returnBook(book.isbn, "Student");
        if (valid == 0) {
            cout << "You already returned this book!\n";
        }
        if (valid == 1) {
            book.status = "Available";
        }
    }

    void showDetails() const override {
        cout << "User ID: " << user_id << ", Username: " << username 
             << ", Name: " << name << "\nAge: " << age << ", Gender: " << gender
             << ", Role: Student"
             << ", Fine: " << account.getFine() << " rupees\n";
    }
};

class Faculty : public User {
    public:
        Faculty(int id, string u, string p, const string& borrowedBooks, int fineAmount, string n = "", int a = 0, string g = "") 
        : User(id, u, p, "Faculty", borrowedBooks, fineAmount, n, a, g) {}

        string getRole() const override { 
            return "Faculty"; 
        }

        bool checkCanBorrow() override {
            if (account.hasOverdueBooksForMoreThan60Days()) {
                cout << "\nYou have overdue books for more than 60 days!\nPlease return them to borrow additional books.\n"; //To prevent borrowing until overdue books are returned
                return false;
            }
            if (account.getCurrentlyBorrowedCount() >= 5) {
                cout << "Borrow limit reached (5 books max)!\nSorry, you cannot borrow more books.\n"; //To prevent borrowing more than 5 books
                return false;
            }
            return true;
        }

        void borrowBook(Book& book) override {
            if (book.status == "Reserved") {
                cout << "\nSorry, this book is RESERVED and cannot be borrowed.\n"; //We have assumed that reserved books cannot be borrowed
                return;
            }
            if (book.status != "Available") {
                cout << "\nSorry, book is NOT AVAILABLE!\n";
                return;
            }
            account.borrowBook(book.isbn, book.title);
            book.status = "Borrowed";
            cout << "Book borrowed successfully!\n";
        }

        void returnBook(Book& book) override {
            if (!account.hasBook(book.isbn)) {
                cout << "You didn't borrow this book!\n";
                return;
            }
            bool valid = account.returnBook(book.isbn, "Faculty");
            if (valid == 0) {
                cout << "You already returned this book!\n";
            }
            if (valid == 1) {
                book.status = "Available";
            }
        }

        void showDetails() const override {
            cout << "User ID: " << user_id << ", Username: " << username 
                << ", Name: " << name << "\nAge: " << age << ", Gender: " << gender
                << ", Role: Faculty\n";
        }
};

class Librarian : public User {
    public:
        Librarian(int id, string u, string p, const string& borrowedBooks, int fineAmount, string n = "", int a = 0, string g = "") 
            : User(id, u, p, "Librarian", borrowedBooks, fineAmount, n, a, g) {}
    
        string getRole() const override { return "Librarian"; }

        void showDetails() const override {
            cout << "User ID: " << user_id << ", Username: " << username 
                 << ", Name: " << name << "\nAge: " << age << ", Gender: " << gender
                 << ", Role: Librarian\n";
        }

        bool checkCanBorrow() override {
            cout << "Librarians cannot borrow books!\n";
            return false;
        }
    
        void borrowBook(Book& book) override {
            cout << "Librarians cannot borrow books!\n";
        }
    
        void returnBook(Book& book) override {
            cout << "Librarians cannot return books!\n";
        }
    
        void addBook(vector<Book>& library) {
            string genre, title, author, publisher, isbn, status;
            int year;
            cout << "Enter genre: ";
            getline(cin, genre);
            cout << "Enter title: ";
            getline(cin, title);
            cout << "Enter author: ";
            getline(cin, author);
            cout << "Enter publisher: ";
            getline(cin, publisher);

            year = getValidIntegerInput("Enter year: "); // Defensive programming to ensure int input
            cout << "Enter ISBN: ";
            cin >> isbn;
            cout << "Enter status (Available/Reserved): ";
            cin >> status;
        
            // Validate status input
            if (status != "Available" && status != "Reserved") {
                cout << "Invalid status! Setting status to 'Available' by default.\n";
                status = "Available";
            }
        
            library.push_back(Book(genre, title, author, publisher, year, isbn, status));
            cout << "\nBook added successfully!\n";
        }
    
        void removeBook(vector<Book>& library) {
            cout << "Enter book ISBN to remove: ";
            string isbn;
            cin >> isbn;
            for (auto it = library.begin(); it != library.end(); ++it) {
                if (it->isbn == isbn) {
                    library.erase(it);
                    cout << "Book removed successfully!\n";
                    return;
                }
            }
            cout << "Book not found!\n";
        }
    
        void updateBook(vector<Book>& library) {
            cout << "Enter book ISBN to update: ";
            string isbn;
            cin >> isbn;
            string status;
            for (auto& book : library) {
                if (book.isbn == isbn) {
                    cout << "\nEnter new genre: ";
                    cin.ignore(); // To handle the newline character left by previous input
                    getline(cin, book.genre);
                    cout << "Enter new title: ";
                    getline(cin, book.title);
                    cout << "Enter new author: ";
                    getline(cin, book.author);
                    cout << "Enter new publisher: ";
                    getline(cin, book.publisher);

                    int year = getValidIntegerInput("Enter new year: "); // Defensive programming to ensure int input
                    book.year = year;
                    // cout << "Enter new ISBN: ";
                    // cin >> isbn;
                    cout << "Enter new status (Available/Reserved): ";
                    cin >> status;

                    if (status != "Available" && status != "Reserved") {
                        cout << "Invalid status! Setting status to 'Available' by default.\n";
                        book.status = "Available";
                    }
                    else book.status = status;
                    cout << "\nBook updated successfully!\n\n";
                    return;
                }
            }
            cout << "\nBook not found!\n";
        }
    
        void addUser(vector<User*>& users) {
            string name, gender, username, password;
            int age;
            int id = users.size() + 1; // Assign the next available User ID
            cout << "Enter full name: ";
            getline(cin, name);
            age = getValidIntegerInput("Enter age: "); // Defensive programming to ensure int input

            while (true) {
                cout << "Enter gender (M/F/O): ";
                cin >> gender;
                if (gender == "M" || gender == "F" || gender == "O") { //Defensive programming
                    break;
                } else {
                    cout << "Invalid gender! Please enter M, F, or O.\n";
                }
            }
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            int choice;
            while (true) {
                choice = getValidIntegerInput("Choose role (1 for Student, 2 for Faculty, 3 for Librarian): "); //Defensive programming to ensure int input
                if (choice >= 1 && choice <= 3) { // Valid range for role selection
                    break;
                } else {
                    cout << "Invalid choice! Please enter 1, 2, or 3.\n";
                }
            }
    
            string emptyBorrowedBooks = ""; // New users start with no borrowed books
            int initialFine = 0;            // New users start with zero fine

            if (choice == 1) {
                users.push_back(new Student(id, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
            } else if (choice == 2) {
                users.push_back(new Faculty(id, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
            } else if (choice == 3) {
                users.push_back(new Librarian(id, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
            } else {
                cout << "\nInvalid choice. User not added.\n";
                return;
            }
    
            cout << "\nUser added successfully! User ID is " << id << ".\n";
        }
    
        void removeUser(vector<User*>& users) {
            cout << "\nEnter user ID to remove: ";
            int id;
            cin >> id;
        
            // Find the user to remove
            auto it = users.begin();
            while (it != users.end()) {
                if ((*it)->getUserID() == id) {
                    // Delete the user object to free memory
                    delete *it;
                    // Remove the user from the vector
                    it = users.erase(it);
                    cout << "\nUser removed successfully!\n";
        
                    // Reassign user IDs to maintain continuity
                    for (size_t i = 0; i < users.size(); ++i) {
                        users[i]->update_user_id(i+1); // Assign IDs from 1 to N
                    }
        
                    return;
                } else {
                    ++it;
                }
            }
        
            // If the user ID is not found
            cout << "User not found!\n";
        }
    };   

class Library {
    public:
        vector<Book> books;
        vector<User*> users;

        void addBook(Book book) { books.push_back(book); }
        void addUser(User* user) { users.push_back(user); }


        // Function to check if a file is locked by another program
        bool isFileLocked(const string& filename) {
            ofstream file(filename, ios::app);
            if (!file) {
                return true; // File is locked or cannot be opened
            }
            return false; // File is not locked
        }

        void saveBooksToFile() {
            if (isFileLocked("books.csv")) {
                cerr << "Error: books.csv is locked by another program. Please close it and try again.\n";
                return;
            }
        
            ofstream file("books.csv");
            if (file.is_open()) {
                for (const auto &book : books) {
                    file << book.toCSV();
                }
                file.close();
            } else {
                cerr << "Error: Unable to open books.csv for writing.\n";
            }
        }

        void loadBooksFromFile() {
            ifstream file("books.csv");
            string g, t, a, p, i, s;
            int y;

            //Load the books from the file
            while (getline(file, g, ',') && getline(file, t, ',') && getline(file, a, ',') &&
                getline(file, p, ',') && file >> y && file.ignore() &&
                getline(file, i, ',') &&
                getline(file, s)) {
                books.push_back(Book(g, t, a, p, y, i));
                books.back().status = s;
            }
            file.close();
        }

        void saveUsersToFile() {
            if (isFileLocked("users.csv")) {
                cerr << "Error: users.csv is locked by another program. Please close it and try again.\n";
                return;
            }
        
            ofstream file("users.csv");
            if (file.is_open()) {
                for (auto user : users) {
                    file << user->toCSV();
                }
                file.close();
            } else {
                cerr << "Error: Unable to open users.csv for writing.\n";
            }
        }

        void loadUsersFromFile() {
            ifstream file("users.csv");
            string line;
            users.clear();
        
            while (getline(file, line)) {
                stringstream ss(line);
                int id, age, fineAmount;
                string username, password, role, name, gender, borrowedBooks;

                // Parse the CSV line into user data
                ss >> id;
                ss.ignore();
                getline(ss, username, ',');
                getline(ss, password, ',');
                getline(ss, role, ',');
                getline(ss, name, ',');
                ss >> age;
                ss.ignore();
                getline(ss, gender, ',');
                getline(ss, borrowedBooks, ',');
                ss >> fineAmount;
        
                if (role == "Student") {
                    users.push_back(new Student(id, username, password, borrowedBooks, fineAmount, name, age, gender));
                } else if (role == "Faculty") {
                    users.push_back(new Faculty(id, username, password, borrowedBooks, fineAmount, name, age, gender));
                } else if (role == "Librarian") {
                    users.push_back(new Librarian(id, username, password, borrowedBooks, fineAmount, name, age, gender));
                }
            }
            file.close();
    }

    int getNextUserID() {
        return users.size() + 1;
    }
};

// Helper function to print a table header
void printAllTableHeader() {
    cout << left << setw(15) << "ISBN"
         << setw(15) << "Genre" 
         << setw(45) << "Title" 
         << setw(25) << "Author"
         << setw(10) << "Status" 
         << endl;
    cout << setfill('-') << setw(110) << "" << setfill(' ') << endl;
}

// Helper function to print book data in table format
void printBookTableRow(const Book& book) {
    cout << left << setw(15) << book.isbn 
         << setw(15) << book.genre
         << setw(45) << book.title 
         << setw(25) << book.author
         << setw(10) << book.status 
         << endl;
}

void printCurrentlyBorrowedTableHeader() {
    cout << left << setw(14) << "ISBN" 
         << setw(45) << "Title" 
         << setw(14) << "Status"
         << setw(21) << "Borrowed Date"
        //  << setw(25) << "Returned Date"
         << setw(21) << "Due Date" 
         << setw(5) << "Fine" 
         << endl;
    cout << setfill('-') << setw(120) << "" << setfill(' ') << endl;
}

void printTableHeader() {
    cout << left << setw(14) << "ISBN" 
         << setw(45) << "Title" 
         << setw(14) << "Status"
         << setw(21) << "Borrowed Date"
         << setw(21) << "Returned Date"
        //  << setw(25) << "Due Date" 
         << setw(5) << "Fine" 
         << endl;
    cout << setfill('-') << setw(120) << "" << setfill(' ') << endl;
}

void printCurrentlyBorrowedBookTableRow(const tuple<string, string, string, string, string, string, int>& book) {
    cout << left << setw(15) << get<0>(book) 
         << setw(45) << get<1>(book) 
         << setw(14) << get<5>(book) 
         << setw(21) << get<2>(book)
        //  << setw(25) << get<3>(book) 
         << setw(21) << get<4>(book) 
         << setw(5) << get<6>(book) 
         << endl;
}

void printBorrowedBookTableRow(const tuple<string, string, string, string, string, string, int>& book) {
    cout << left << setw(15) << get<0>(book) 
         << setw(45) << get<1>(book) 
         << setw(14) << get<5>(book) 
         << setw(21) << get<2>(book)
         << setw(21) << get<3>(book) 
        //  << setw(25) << get<4>(book) 
         << setw(5) << get<6>(book) 
         << endl;
}

void mainMenu(Library &lib, User* currentUser) {
    while (true) {
        // Display the main menu
        cout << "\n=========== Main Menu ===========\n";
        cout << "\n1. Edit Profile\n";
        cout << "2. View All Books\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. View Currently Borrowed Books\n";
        cout << "6. View Past Borrowing History\n";
        cout << "7. View Fines\n";
        cout << "8. View Overdue Books\n";
        cout << "9. Pay Fines\n";
        cout << "10. Logout\n";
        // cout << "\nChoose an option (write the option number): ";     

        if (currentUser->getRole() == "Student") {
            currentUser->getAccount().calculateFines();
        }
        // Each time User returns to Main Menu, overdue books are rechecked and updated (for faculty)
        else if (currentUser->getRole() == "Faculty") {
            auto overdueBooks = currentUser->getAccount().getOverdueBooks();
        }
        int choice;
        while (true) {
            choice = getValidIntegerInput("\nChoose an option (write the option number): "); //Defensive programming to ensure int input
            if (choice >= 1 && choice <= 10) { // Valid range for main menu
                break;
            } else {
                cout << "Invalid choice! Please enter a number between 1 and 10.\n";
            }
        }
        
        if (choice == 1) {
            // Edit Profile
            currentUser->editProfile();
            lib.saveUsersToFile(); // Save changes to the users.csv file
            
        } else if (choice == 2) {
            // View All Books
            cout << "\nLibrary Books:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }

        } else if (choice == 3) {
            // Borrow Book
            bool canBorrow = currentUser->checkCanBorrow();
            if (!canBorrow) continue;
            cout << "\nLibrary Books:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }
            cout << "\nEnter the ISBN of the book you want to borrow (simply copy-paste from above): ";
            string isbn;
            cin >> isbn;
            bool valid = false;
            for (auto &book : lib.books) {
                if (book.isbn == isbn) {
                    valid = true;
                    currentUser->borrowBook(book);
                    break;
                }
            }
            if (!valid) {
                cout << "\nInvalid ISBN!\n";
                continue;
            }
            lib.saveUsersToFile();
            lib.saveBooksToFile();
        
        } else if (choice == 4) {
            // Return Book
            cout << "\nYour Currently Borrowed Books:\n\n";
            printCurrentlyBorrowedTableHeader();
            auto deserializedBooks = currentUser->getAccount().getBorrowedBooksList();
            bool hasBorrowedBooks = false;

            for (const auto& book : deserializedBooks) {
                if (get<5>(book) != "Returned") {
                    hasBorrowedBooks = true;
                    printCurrentlyBorrowedBookTableRow(book);
                }
            }

            if (!hasBorrowedBooks) {
                cout << "\nYou have no books to return.\n";
                continue;
            }

            cout << "\nEnter the ISBN of the book you want to return (simply copy-paste from above): ";
            string isbn;
            cin >> isbn;
            bool valid = false;

            for (auto &book : lib.books) {
                if (book.isbn == isbn) {
                    valid = true;
                    currentUser->returnBook(book);
                    break;
                }
            }

            if (!valid) {
                cout << "\nInvalid ISBN!\n";
                continue;
            }

            lib.saveUsersToFile();
            lib.saveBooksToFile();

        } else if (choice == 5) {
            // View Currently Borrowed Books
            cout << "\nYour Currently Borrowed Books:\n\n";
            printCurrentlyBorrowedTableHeader();
            auto deserializedBooks = currentUser->getAccount().getBorrowedBooksList();
            bool hasBorrowedBooks = false;

            for (const auto& book : deserializedBooks) {
                if (get<5>(book) != "Returned") {
                    hasBorrowedBooks = true;
                    printCurrentlyBorrowedBookTableRow(book);
                }
            }

            if (!hasBorrowedBooks) {
                cout << "\nYou have no currently borrowed books.\n";
            }

        } else if (choice == 6) {
            // View Past Borrowing History
            cout << "\nYour Past Borrowing History:\n\n";
            printTableHeader();
            auto pastBorrowedBooks = currentUser->getAccount().getPastBorrowedBooksList();
            if (!pastBorrowedBooks.empty()) {
                for (const auto& book : pastBorrowedBooks) {
                    printBorrowedBookTableRow(book);
                }
            } else {
                cout << "You have no past borrowing history.\n";
            }

        } else if (choice == 7) {
            // View Fines
            if (currentUser->getRole() == "Student") {
                cout << "\nTotal Fine: " << currentUser->getAccount().getFine() << " rupees.\n";
            } else {
                cout << "\nFines are not applicable for faculty members.\n";
            }
        } else if (choice == 8) {
            // View Overdue Books
            auto overdueBooks = currentUser->getAccount().getOverdueBooks();
            if (!overdueBooks.empty()) {
                cout << "\nYour Overdue Books:\n";
                printCurrentlyBorrowedTableHeader();
                for (const auto& book : overdueBooks) {
                    printCurrentlyBorrowedBookTableRow(book);
                }
            } else {
                cout << "\nNo overdue books.\n";
            }
        } else if (choice == 9) {
            // Pay Fines (for students only)
            if (currentUser->getRole() == "Student") {
                int fineAmount = currentUser->getAccount().getFine();
                if (fineAmount > 0) {
                    cout << "\nYour total outstanding fine is: " << fineAmount << " rupees.\n";
                    cout << "Simulating payment...\n";
                    currentUser->getAccount().setFine(0); // Reset fine to zero
                    cout << "Payment successful! Your fines have been cleared.\n";
                } else {
                    cout << "\nYou have no outstanding fines.\n";
                }
            } else {
                cout << "\nNo fines for faculty members.\n";
            }
        } else if (choice == 10) {
            // Logout
            break;
        } else {
            cout << "\nInvalid choice! Please try again.\n";
        }
    }
}

//Helpfer functions to print table format for librarian menu

void printUserTableHeader() {
    cout << left << setw(10) << "User ID" 
         << setw(20) << "Username" 
         << setw(30) << "Name"
         << setw(10) << "Age"
         << setw(10) << "Gender"
         << setw(15) << "Role" 
         << endl;
    cout << setfill('-') << setw(90) << "" << setfill(' ') << endl;
}

void printUserTableRow(const User* user) {
    cout << left << setw(10) << user->getUserID() 
         << setw(20) << user->getUsername() 
         << setw(30) << user->getName()
         << setw(10) << user->getAge()
         << setw(10) << user->getGender()
         << setw(15) << user->getRole() 
         << endl;
}

void librarianMenu(Library &lib, Librarian* librarian) {
    while (true) {
        // Display the librarian menu
        cout << "\n===== Librarian Menu =====\n\n";
        cout << "1. Edit Profile\n";
        cout << "2. View All Books \n";
        cout << "3. Add Book\n";
        cout << "4. Remove Book\n";
        cout << "5. Update Book\n";
        cout << "6. View All Users\n";
        cout << "7. Add User\n";
        cout << "8. Remove User\n";
        cout << "9. Logout\n";
        // cout << "\nChoose an option (write the option number): ";

        int choice;
        while (true) {
            choice = getValidIntegerInput("\nChoose an option (write the option number): "); //Defensive programming to ensure int input
            if (choice >= 1 && choice <= 9) { // Valid range for librarian menu
                break;
            } else {
                cout << "Invalid choice! Please enter a number between 1 and 9.\n";
            }
        }

        if (choice == 1) {
            // Edit Profile
            librarian->editProfile();
            lib.saveUsersToFile(); // Save changes to the users.csv file
        }

        else if (choice == 2) {
            // View All Books
            cout << "\nLibrary Books: \n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }
        } else if (choice == 3) {
            // Add Book
            cout << "\n===== Add Book =====\n";
            librarian->addBook(lib.books);
            lib.saveBooksToFile();
            cout << "\nUpdated Library Book List:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }

        } else if (choice == 4) {
            // Remove Book
            cout << "\n===== Remove Book =====\n\n";
            cout << "Current Library Books:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }
            cout << "\n";
            librarian->removeBook(lib.books);
            lib.saveBooksToFile();
            cout << "\nUpdated Library Book List:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }

        } else if (choice == 5) {
            // Update Book
            cout << "\n===== Update Book =====\n\n";
            cout << "Current Library Books:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }
            cout << "\n";
            librarian->updateBook(lib.books);
            lib.saveBooksToFile();
            cout << "Updated Library Book List:\n\n";
            printAllTableHeader();
            for (const auto& book : lib.books) {
                printBookTableRow(book);
            }

        } else if (choice == 6) {
            // View All Users
            cout << "\nLibrary Users: \n\n";
            printUserTableHeader();
            for (const auto& user : lib.users) {
                printUserTableRow(user);
            }
            
        }else if (choice == 7) {
            // Add User
            cout << "\n===== Add User =====\n\n";
            librarian->addUser(lib.users);
            lib.saveUsersToFile();

            // Display updated user list
            cout << "\nUpdated Users:\n\n";
            printUserTableHeader();
            for (const auto& user : lib.users) {
                printUserTableRow(user);
            }

        } else if (choice == 8) {
            // Remove User
            cout << "\n===== Remove User =====\n\n";
            cout << "Current Users:\n\n";
            printUserTableHeader();
            for (const auto& user : lib.users) {
                printUserTableRow(user);
            }
            librarian->removeUser(lib.users);
            lib.saveUsersToFile();

            // Display updated user list
            cout << "\nUpdated User List:\n\n";
            printUserTableHeader();
            for (const auto& user : lib.users) {
                printUserTableRow(user);
            }

        } else if (choice == 9) {
            // Logout
            cout << "\nLogging out...\n";
            break;

        } else {
            cout << "Invalid choice! Please try again.\n";
        }
    }
}


void signup(Library &lib) {
    string username, password, name, gender;
    int age;
    cout << "\nEnter your full name: ";
    cin.ignore(); // To handle the newline character left by previous input
    getline(cin, name); // Allow spaces in the name

    age = getValidIntegerInput("Enter your age in years: "); // Defensive programming to ensure int input

    while (true) {
        cout << "Enter gender (M/F/O): ";
        cin >> gender;
        if (gender == "M" || gender == "F" || gender == "O") { //Defensive programming
            break;
        } else {
            cout << "Invalid gender! Please enter M, F, or O.\n";
        }
    }

    cout << "Enter username (should not contain spaces): ";
    cin >> username;
    cout << "Enter password (should not contain spaces): ";
    cin >> password;

    int choice;
    while (true) {
        choice = getValidIntegerInput("Choose role (1 for Student, 2 for Faculty, 3 for Librarian): "); //Defensive programming to ensure int input
        if (choice >= 1 && choice <= 3) { // Valid range for role selection
            break;
        } else {
            cout << "Invalid choice! Please enter 1, 2, or 3.\n";
        }
    }

    int newUserID = lib.getNextUserID();
    string emptyBorrowedBooks = ""; // New users start with no borrowed books
    int initialFine = 0;            // New users start with zero fine

    if (choice == 1) {
        lib.addUser(new Student(newUserID, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
    } else if (choice == 2) {
        lib.addUser(new Faculty(newUserID, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
    } else if (choice == 3) {
        lib.addUser(new Librarian(newUserID, username, password, emptyBorrowedBooks, initialFine, name, age, gender));
    } else {
        cout << "Invalid choice. Signup failed.\n";
        return;
    }

    lib.saveUsersToFile();
    cout << "\nSignup successful! Your User ID is " << newUserID << ". You can now log in.\n";
}

// Function to print centered text
void printCentered(const string& text, int width = 60) {
    int padding = (width - text.length()) / 2;
    if (padding > 0) {
        cout << string(padding, ' ') << text << endl;
    } else {
        cout << text << endl;
    }
}

// Function to handle login
void login(Library &lib) {
    string username, password;
    cout << "\n======== Login ========\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    for (auto user : lib.users) {
        if (user->authenticate(username, password)) {
            cout << "\n============================================================\n";
            printCentered("Welcome, " + user->getName() + "!");
            cout << "============================================================\n\n";
            user->showDetails();

            // Check for overdue books and fines
            if (user->getRole() == "Student") {
                user->getAccount().calculateFines();
                auto overdueBooks = user->getAccount().getOverdueBooks();
                if (!overdueBooks.empty()) {
                    cout << "\nYou have overdue book(s):\n";
                    printTableHeader();
                    for (const auto& book : overdueBooks) {
                        printBorrowedBookTableRow(book);
                    }
                } else {
                    cout << "\nNo overdue books.";
                }

            } else if (user->getRole() == "Faculty") {
                auto overdueBooks = user->getAccount().getOverdueBooks();
                if (!overdueBooks.empty()) {
                    cout << "\nYou have overdue book(s):\n";
                    printTableHeader();
                    for (const auto& book : overdueBooks) {
                        printBorrowedBookTableRow(book);
                    }
                } else {
                    cout << "\nNo overdue books.\n";
                }
            }

            cout << "\n";
            if (user->getRole() == "Librarian") {
                librarianMenu(lib, dynamic_cast<Librarian*>(user));
            } else {
                mainMenu(lib, user);
            }
            return;
        }
    }
    cout << "\nInvalid credentials!\n";
}

// Function to display the stylish initial page
void displayWelcomePage() {
    cout << "\n";
    cout << "============================================================\n";
    printCentered("WELCOME TO");
    cout << "\n";
    printCentered("LIBRARY MANAGEMENT SYSTEM");
    cout << "\n";
    printCentered("Made by Aritra Ray");
    cout << "============================================================\n";
}

int main() {
    Library lib;

    // Check if books.csv and users.csv are empty
    ifstream booksFile("books.csv");
    ifstream usersFile("users.csv");

    bool booksFileEmpty = booksFile.peek() == ifstream::traits_type::eof();
    bool usersFileEmpty = usersFile.peek() == ifstream::traits_type::eof();

    booksFile.close();
    usersFile.close();

    // If both files are empty, initialize the library with default data
    if (booksFileEmpty && usersFileEmpty) {
        cout << "\n\nInitializing library with default data...\n";

        // Add 10 books
        lib.addBook(Book("Fiction", "The Great Gatsby", "F. Scott Fitzgerald", "Scribner", 1925, "9780743273565", "Available"));
        lib.addBook(Book("Fiction", "To Kill a Mockingbird", "Harper Lee", "J.B. Lippincott & Co.", 1960, "9780061120084", "Available"));
        lib.addBook(Book("Fiction", "1984", "George Orwell", "Secker & Warburg", 1949, "9780451524935", "Available"));
        lib.addBook(Book("Non-Fiction", "Sapiens: A Brief History of Humankind", "Yuval Noah Harari", "Harper", 2011, "9780062316097", "Available"));
        lib.addBook(Book("Self-Help", "The Power of Now", "Eckhart Tolle", "New World Library", 1997, "9781577314806", "Available"));
        lib.addBook(Book("Science", "A Brief History of Time", "Stephen Hawking", "Bantam Books", 1988, "9780553380163", "Available"));
        lib.addBook(Book("Science", "Cosmos", "Carl Sagan", "Random House", 1980, "9780345331359", "Available"));
        lib.addBook(Book("Fantasy", "The Hobbit", "J.R.R. Tolkien", "Allen & Unwin", 1937, "9780547928227", "Reserved"));
        lib.addBook(Book("Fantasy", "Harry Potter and the Philosopher's Stone", "J.K. Rowling", "Bloomsbury", 1997, "9780747532699", "Available"));
        lib.addBook(Book("Mystery", "The Da Vinci Code", "Dan Brown", "Doubleday", 2003, "9780307474278", "Available"));

        // Add 5 students
        lib.addUser(new Student(1, "aritra", "10001", "", 0, "Aritra Ray", 20, "M"));
        lib.addUser(new Student(2, "priya", "10002", "", 0, "Shanaya Srivastava", 22, "F"));
        lib.addUser(new Student(3, "rahul", "10003", "", 0, "Rahul Verma", 24, "M"));
        lib.addUser(new Student(4, "anita", "10004", "", 0, "Anita Singh", 23, "F"));
        lib.addUser(new Student(5, "soham", "10005", "", 0, "Soham Das", 21, "M"));

        // Add 3 faculty
        lib.addUser(new Faculty(6, "neha", "20001", "", 0, "Neha Gupta", 35, "F"));
        lib.addUser(new Faculty(7, "raj", "20002", "", 0, "Rajkumar Jain", 40, "M"));
        lib.addUser(new Faculty(8, "amit", "20003", "", 0, "Amit Vinayak Shinde", 38, "M"));

        // Add 1 librarian
        lib.addUser(new Librarian(9, "anil", "30001", "", 0, "Anil Kumar Jha", 45, "M"));

        // Save the default data to files
        lib.saveBooksToFile();
        lib.saveUsersToFile();

        cout << "Default data initialized successfully!\n";
    }
    // Load data from files
    else {
        cout << "\n\nLoading data from files...\n";
        lib.loadUsersFromFile();
        lib.loadBooksFromFile();
    }

    displayWelcomePage();
    cout << "\nHello there! What do you want to do today?\n\n";

    //Inital menu loop
    while (true) {
        cout << "1. Login\n";
        cout << "2. Signup\n";
        cout << "3. Exit\n";
        cout << "\nChoose an option (1, 2 or 3): ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            login(lib);
        } else if (choice == 2) {
            signup(lib);
        } else if (choice == 3) {
            cout << "\nGoodbye! Have a nice day.\n";
            break;
        }
        else {
            cout << "\nInvalid choice! Please try again.\n";
        }
        cout << "\n==== Initial Menu ====\n";
    }
    return 0;
}