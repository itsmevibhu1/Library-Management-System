#include<iostream>
#include "book.h"
#include<string>
using namespace std;
book::book(std::string g, std::string t, std::string a, std::string p, int y, std::string i){
          genre = g;
          title = t;
          author = a;
          publisher = p;
          isbn = i;
          year = y;
          status = "Available";
        }
void book::printbook() const{
    std::cout<<title<<" | "<<author<<" | "<<publisher<<" | "<<year<<" | "<<isbn<<endl;
}
string book::booktocsv() const{
    string s = genre + "," + title + "," + author + "," + publisher + "," + to_string(year) + "," + isbn + "," + status + "\n";
    return s;
}
bool book::operator==(const book &b) const{
    return (b.genre == genre && b.isbn == isbn && b.year == year && b.publisher == publisher 
    && b.author == author && b.title == title);
}
bool book::operator>(const book &b) const{
    return b.isbn > isbn;
}

bool book::operator<(const book &b) const{
    return b.isbn < isbn;
}