#pragma once
#include <iostream>
#include <string>
using namespace std;
class book{
    public:
        std::string genre, title, author, publisher, isbn;
        int year;
        std::string status;
        book(std::string g, std::string t, std::string a, std::string p, int y, std::string i);
        void printbook() const;
        string booktocsv() const;
        bool operator==(const book&b) const;
        bool operator>(const book&b) const;
        bool operator<(const book&b) const;

};