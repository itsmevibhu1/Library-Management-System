#pragma once
#include<iostream>
#include "account.h"
using namespace std;

class librarian : public account{    
    public:
        librarian(string username,string password);
        void homepage();
        
};