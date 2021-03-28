#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    unordered_map<string, int> map;
    queue<string> words;
    ifstream file;
    file.open("files/1.txt");
 
    string text;
    while (file >> text) {
        words.push(text);
    }
    file.close();

    while (!words.empty()) {
        string word = words.front();
        words.pop();
        map[word]++;
    }

    /*for (auto x : map) {
        cout << x.first << ": " << x.second << endl;
    }*/
    cout << "Map length: " << map.size() << endl;
}
