#include <iostream>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <time.h>

using namespace std;

int getSeconds() {
    return rand() % 1 + 1;
}

int getNaturalNumber() {
    return rand() % 1000 + 1;
}

class Judge {
public:
    shared_mutex decision;
    mutex tie;
    Judge(){}
};

class Reader {
public:
    string id;
    Reader(string ID) {
        id = ID;
    }
    void Action1(fstream &file, shared_mutex &decision) {
        string line;
        int sec = getSeconds();
        
        this_thread::sleep_for(chrono::seconds(sec));
        shared_lock<shared_mutex> lock(decision);

        while (!file.eof()) {
            file >> line;
        }
        file.clear();
        file.seekg(0, ios::beg);

        if(line.length() < 1) return;
        cout << id + line << endl;

        lock.unlock();
    }

    void Action2(fstream &file, mutex &decision) {
        string line;
        int sec = getSeconds();
        
        this_thread::sleep_for(chrono::seconds(sec));
        lock_guard<mutex> lock(decision);

        while (!file.eof()) {
            file >> line;
        }
        file.clear();
        file.seekg(0, ios::beg);

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
    void Action1(fstream &file, shared_mutex &decision) {
        int sec = getSeconds();

        this_thread::sleep_for(chrono::seconds(sec));
        lock_guard<shared_mutex> lock(decision);
        file << id + to_string(getNaturalNumber()) << endl;
    }

    void Action2(fstream &file, mutex &decision) {
        int sec = getSeconds();

        this_thread::sleep_for(chrono::seconds(sec));
        lock_guard<mutex> lock(decision);
        file << id + to_string(getNaturalNumber()) << endl;
    }
};


/*
    Action1 - zagłodzenie writerów
    Action2 - bez zagłodzenie(remis)
*/

int main()
{
    srand(time(NULL));
    int num = 5;

    vector<thread> threads;
    vector<Writer> writers;
    vector<Reader> readers;
    Judge judge = Judge();
    fstream file("dane.txt", ios::app | ios::in);

    // zagłodzenie writerów
    if(file.is_open()) {
        for (int i = 0; i < num; i++)
        {
            string wID = to_string(0) + to_string(i);
            string rID = to_string(1) + to_string(i);
            writers.push_back(Writer(wID));
            readers.push_back(Reader(rID));
        }
        
        for (int i = 0; i < num; i++){
            thread thWriter = thread(&Writer::Action1, &writers[i], ref(file), ref(judge.decision));
            thread thReader = thread(&Reader::Action1, &readers[i], ref(file), ref(judge.decision));
            threads.push_back(move(thWriter));
            threads.push_back(move(thReader));
        }

        for(int i = 0; i < num * 2; i++) {
            threads[i].join();
        }
        file.close();
    } else {
        cout << "Couldn't open file: dane.txt!" << endl;
    }

    // remis
    // if(file.is_open()) {
    //     for (int i = 0; i < num; i++)
    //     {
    //         string wID = to_string(0) + to_string(i);
    //         string rID = to_string(1) + to_string(i);
    //         writers.push_back(Writer(wID));
    //         readers.push_back(Reader(rID));
    //     }
        
    //     for (int i = 0; i < num; i++){
    //         thread thWriter = thread(&Writer::Action2, &writers[i], ref(file), ref(judge.tie));
    //         thread thReader = thread(&Reader::Action2, &readers[i], ref(file), ref(judge.tie));
    //         threads.push_back(move(thWriter));
    //         threads.push_back(move(thReader));
    //     }

    //     for(int i = 0; i < num * 2; i++) {
    //         threads[i].join();
    //     }
    //     file.close();
    // } else {
    //     cout << "Couldn't open file: dane.txt!" << endl;
    // }

    return 0;
}
