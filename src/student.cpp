#include<iostream>
#include "student.h"
using namespace std;

student::student(string usrname,string password){
    this->username = usrname;
    this->type = "student";
    this -> password = password;
    this->total_fine = 0;
    this->max_book = 5;
    this->duedays = 30;
    this->finerate = 2;
    this->history_size = 5;
}
