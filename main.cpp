#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
queue<string> words;

void Reader(ifstream *file) {
	string text;

	while (std::getline(*file, text)) {
		words.push(text);
	}
	file->close();
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

int main(int argc, char **argv) {
    ifstream file;
    file.open("files/1.txt");
 
	Reader(&file);
	cout << words.size();
	unordered_map<string, int> map = Mapper();
    for (auto x : map) {
        cout << x.first << ": " << x.second << endl;
    }
    cout << "Map length: " << map.size() << endl;

}

