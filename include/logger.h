#pragma once
#include<iostream>
#include<string>
#include<limits>
#include<ctime>

using namespace std;

class logger {
private:
    enum level {
        error, warning, trace
    };
    static level m_curr_lev; // only declaration

public:
    logger(int x) {
        if (x == 1) m_curr_lev = error;
        else if (x == 2) m_curr_lev = warning;
        else if (x == 3) m_curr_lev = trace;
        else cout << "Not a valid level, level set to error" << endl;
    }

    static void printError(const string &prompt) {
        cout << "Error : " << prompt << endl;
    }

    static void printWarning(const string &prompt) {
        if (m_curr_lev != error)
            cout << "Warning : " << prompt << endl;
    }

    static void printTrace(const string &prompt) {
        if (m_curr_lev == trace)
            cout << "Trace : " << prompt << endl;
    }
};