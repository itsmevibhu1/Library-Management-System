#include<iostream>
#include<string>
#include "book.h"
#include "transaction.h"
#include "account.h"
using namespace std;
bool account::authenticate(string pass) const {
        return password == pass;
    }
void account::calculatefine(time_t due_time) {
        time_t curr_time = time(0);
        int late_days = (curr_time - due_time) / (60 * 60 * 24);
        if (late_days > 0) total_fine += late_days * finerate;
    }
void account::addtransaction(transaction t) {
        if(history.size() >= history_size) history.pop();
        history.push(t);
    }
void account::viewtransactions(){
        queue<transaction> q;
        int i = 1;
        while(!history.empty()) {
            transaction t = history.front();
            history.pop();
            q.push(t);
            cout << "---------------- Transaction " << i << " ----------------" << endl;
            t.print_transaction();
            i++;
        }
        while(!q.empty()) {
            transaction t = q.front();
            q.pop();
            history.push(t);
        }
    }
vector<string> account::account_to_csv(){
    vector<string> data;
    string c = username + "," + type + "," + password + "," + to_string(curr_borrowed.size()) + "," + to_string(history.size()) + '\n';
    data.push_back(c);
    for(const auto& it : curr_borrowed){
        string t_str = to_string(it.second);
        string c = it.first->isbn + "," + t_str + "\n";
        data.push_back(c);
    }
    queue<transaction> temp = history;
    while(!temp.empty()){
        transaction t = temp.front();
        temp.pop();
        string c = t.transaction_to_csv();
        data.push_back(c);
    }
    return data; 
}
