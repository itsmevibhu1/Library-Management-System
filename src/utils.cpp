#include "utils.h"
#include <iostream>
#include <limits>

using namespace std;

int getValidIntegerInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter an integer.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

time_t calculatetime(time_t curr_time, int days) {
    return curr_time + days * 24 * 60 * 60;
}