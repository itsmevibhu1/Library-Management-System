#pragma once
#include<iostream>
#include "account.h"
#include "library.h"

class faculty : public account{    
    public:
        faculty(string username,string password);
};