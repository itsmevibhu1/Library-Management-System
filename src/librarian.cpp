#include<iostream>
#include "librarian.h"
#include "utils.h"
using namespace std;

librarian::librarian(string usrname,string password){
    this->username = usrname;
    this->type = "librarian";
    this -> password = password;
    this->total_fine = 0;
    this->max_book = INT_MAX;
    this->duedays = 60;
    this->finerate = 5;
    this->history_size = 5;
    
}