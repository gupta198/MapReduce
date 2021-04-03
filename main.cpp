#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;


void Reader(string fileName, queue<string> *words) {
    ifstream file;
    file.open(fileName);
    string text;

    while (getline(file, text)) {
        words->push(text);
    }
    file.close();
}

unordered_map<string, int> Mapper(queue<string> *words) {
    unordered_map<string, int> map;
    while (!words->empty()) {
        string wrd;
        stringstream iss(words->front());
        while (iss >> wrd) {
            map[wrd]++;
        }
        words->pop();
    }

    return map;
}

void ReduceMapper(unordered_map<string,int> *map, unordered_map<string, int> *masterMap) {
    for (auto i : *map) {
        (*masterMap)[i.first] += i.second;
    }
}

int main(int argc, char **argv) {
    const int NUM_FILES = 16;
    const int NUM_MAPS = 3;
    queue<string> words;
    unordered_map<string, int> masterMap;
    unordered_map<string, int> map[NUM_MAPS];

    //1 reader per file
    for (int i = 1; i <= NUM_FILES; i++) {
        if (i == 9 && i == 10) {
            continue;
        }
        cout << i << ".txt is running." << endl;
        ostringstream stream;
        stream << "files/" << i << ".txt";
        string fileName = stream.str();
        Reader(fileName, &words);
        cout << "Lines in Queue: " << words.size() << endl;
    }

    for (int i = 0; i < NUM_MAPS; i++) {
        map[i] = Mapper(&words);
        cout << "Map length: " << map[i].size() << endl;
    }
    //barrier
    for (int i = 0; i < NUM_MAPS; i++) {
        ReduceMapper(&map[i], &masterMap);
    }
    cout << "Master Map length: " << masterMap.size() << endl;
    return(EXIT_SUCCESS);
}
