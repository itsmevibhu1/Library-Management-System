# 📚 Library Management System (C++)

A command-line based **Library Management System** implemented in **C++** using Object-Oriented Programming. This project supports login/logout, book borrowing/returning, user history tracking, and persistent CSV-based storage.

---

## ✨ Features

### 👤 User Roles
- **Student**
  - Can issue up to 5 books for 30 days
  - Fine: ₹2/day after due date
- **Faculty**
  - Unlimited books for 60 days
  - Fine: ₹5/day after due date
- **Librarian**
  - Can manage books and users (add/update/delete)

### 📚 Book Management
- View all books
- Check availability
- Issue and return books

### 🔐 Account Management
- Login & signup
- Auto-persistence to `userdata.csv`
- Secure CSV write with **file locking**

### 🧾 Transaction Logs
- Tracks all borrow and return activity
- Stored in `history.csv`

### 💾 Persistent Storage
Data saved and loaded from:
- `data/books.csv`
- `data/userdata.csv`
- `data/history.csv`

---
## ⚙️ How to Compile and Run

Make sure you have `g++` installed and are in the root directory.

### 🔨 Compile:
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o build/main.out
./build/main.out

🗂️ Data Format
📘 books.csv
```bash
Genre,Title,Author,Publisher,Year,ISBN,status
Fantasy,HarryPotter,J.K.Rowling,Bloomsbury,1997,ISBN001,Available

👤 userdata.csv
```bash
username,type,password,num_borrowed,num_transactions
ISBN,timestamp
borrow_type,username,isbn,timestamp

🔁 history.csv
```bash
transaction_type,username,isbn,YYYY-MM-DD HH:MM:SS

👨‍💻 Author

Sahil Pandey
Undergraduate, Electrical Engineering
Indian Institute of Technology, Kanpur
