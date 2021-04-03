#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

const int numFiles = 16;
queue<string> words;
unordered_map<string, int> masterMap;

void Reader(string fileName) {
    ifstream file;
    file.open(fileName);
    string text;

    while (getline(file, text)) {
        words.push(text);
    }
    file.close();
}

unordered_map<string, int> Mapper() {
    unordered_map<string, int> map;
    while (!words.empty())
    {
        string wrd;
        stringstream iss(words.front());
        while (iss >> wrd)
        {
            map[wrd]++;
        }
        words.pop();

    }

    return map;
}

void ReduceMapper(unordered_map<string,int> *map) {
    for (auto i : *map)
    {
        masterMap[i.first] += i.second;
    }
}

int main(int argc, char **argv) {

    for (int i = 1; i < numFiles+1; i++)
    {
        if (i != 9 && i != 10)
        {
            cout << i << ".txt is running." << endl;
            ostringstream stream;
            stream << "files/" << i << ".txt";
            string fileName = stream.str();
            Reader(fileName);
            cout << "Lines in Queue: " << words.size() << endl;
            unordered_map<string, int> map = Mapper();
            cout << "Map length: " << map.size() << endl;
            ReduceMapper(&map);
            cout << "Master Map length: " << masterMap.size() << endl;
        }
    }


    /*file.open("files/1.txt");
      Reader(&file);
      cout << words.size() << endl;
      unordered_map<string, int> map = Mapper();

      file.open("files/2.txt");
      Reader(&file);
      cout << words.size() << endl;
      unordered_map<string, int> map2 = Mapper();
      for (auto x : map) {
      cout << x.first << ": " << x.second << endl;
      }
      cout << "Map length: " << map.size() << endl;
      cout << "Map2 length: " << map2.size() << endl;

      ReduceMapper(&map);
      ReduceMapper(&map2);

      cout << "Master Map length: " << masterMap.size() << endl;*/

}
