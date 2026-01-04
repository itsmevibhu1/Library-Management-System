#include<iostream>
#include<string>
#include<set>
#include<map>
#include "book.h"
#include "account.h"
#include "transaction.h"
#include "student.h"
#include "faculty.h"
#include "librarian.h"
#include "utils.h"
#include "logger.h"
#include<library.h>
using namespace std;
int main(){
    cout << R"(
===========================================
|  Library Management System              |
|  Made with stress, by Vibhu Yadav!!     |
===========================================
)" << endl;
    lib ml("PK kelkar");
  ml.mainpage();
}
