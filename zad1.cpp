#include <iostream>
#include <thread>
#include <shared_mutex>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <time.h>

using namespace std;

int getSeconds() {
    return rand() % 5 + 1;;
}

class Judge {
public:
    shared_mutex decision;
    Judge(){}
};

class Person{
public:
    string id;
    Person(string ID) {
        id = ID;
    }
    virtual void Action() {}
};

class Reader {
public:
    string id;
    Reader(string ID) {
        id = ID;
    }
    void Action(fstream file, shared_mutex decision) {
        string line;

        shared_lock<shared_mutex> lock(decision);
        this_thread::sleep_for(chrono::seconds(getSeconds()));
        while (getline(file, line));
        lock.unlock();

        if(line.length() < 1) return;
        cout << id + line << endl;
    }
};

class Writer {
public:
    string id;
    Writer(string ID) {
        id = ID;
    }
    static void Action(Writer &w, fstream file, shared_mutex decision) {
        int number = rand() % 1000 + 1;

        lock_guard<shared_mutex> lock(decision);
        this_thread::sleep_for(chrono::seconds(getSeconds()));
        file << w.id + to_string(number) << endl;
    }
};

// void call_f(Person p, fstream file, shared_mutex decision) {
//     p.Action(file, decision);
// }

int main()
{
    srand(time(NULL));

    int num = 5;

    vector<thread> threads;
    vector<Writer> writers;
    vector<Reader> readers;
    Judge judge = Judge();
    fstream file("dane.txt", ios::app | ios::in);

    if(file.is_open()) {
        for (int i = 0; i < num; i++)
        {
            string wID = to_string(0) + to_string(i);
            string rID = to_string(1) + to_string(i);
            writers.push_back(Writer(wID));
            readers.push_back(Reader(rID));
        }
        
        for (int i = 0; i < num; i++){
            // threads[i] = thread(call_f, writers[i], file, judge.decision);
            // threads[i] = thread(call_f, readers[i], file, judge.decision);
            threads[i] = thread(Writer::Action, writers[i], file, judge.decision);
            // threads[i + num] = thread(readers[i].Action, file, judge.decision);
        }

        file.close();
        for(int i = 0; i < num * 2; i++) {
            threads[i].join();
        }
    } else {
        cout << "Couldn't open file: dane.txt!" << endl;
    }

    return 0;
}
