#pragma once
#include<iostream>
#include "account.h"
using namespace std;

class student : public account{    
    public:
        student(string username,string password);
};