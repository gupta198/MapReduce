#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>
#include <omp.h>

using namespace std;

void Reader(string fileName, queue<string> *words) {
   ifstream file;
   file.open(fileName);
   string text;

   while (getline(file, text)) {
      #pragma omp critical
      words->push(text);
   }
   file.close();
}

unordered_map<string, int> Mapper(queue<string> *words) {
   unordered_map<string, int> map;
   while (!words->empty()) {
      string wrd;
      string line;
      #pragma omp critical
      {
         line = words->front();
         words->pop();
      }
      stringstream ss;
      ss.str(line);
      while (ss >> wrd) {
         map[wrd]++;
      }
   }

   return map;
}

void ReduceMapper(unordered_map<string,int> *map, unordered_map<string, int> *masterMap) {
   for (auto i : *map) {
      #pragma omp atomic
      (*masterMap)[i.first] += i.second;
   }
}

int main(int argc, char **argv) {
   const int NUM_FILES = 250;
   const int NUM_MAPS = omp_get_num_threads();
   queue<string> words;
   unordered_map<string, int> masterMap;
   unordered_map<string, int> map[NUM_MAPS];

   double time = -omp_get_wtime();

   //1 reader per file
   #pragma omp parallel for //nowait
   for (int i = 1; i <= NUM_FILES; i++) {
      //cout << i << ".txt is running." << endl;
      //cout << omp_get_thread_num() << endl;
      ostringstream stream;
      stream << "files/" << i << ".txt";
      string fileName = stream.str();
      Reader(fileName, &words);
      //cout << "Lines in Queue: " << words.size() << endl;
   }
   //remove this barrier 
   #pragma omp parallel
   {
      #pragma omp single
      for (int i = 0; i < NUM_MAPS; i++) {
         #pragma omp task
         {
            map[i] = Mapper(&words);
            //cout << "Map length: " << map[i].size() << endl;
         }
      }
   }
   //barrier
   cout << "Execution time: " << time+omp_get_wtime() << endl;
   #pragma omp parallel for
   for (int i = 0; i < NUM_MAPS; i++) {
      ReduceMapper(&map[i], &masterMap);
   }
   cout << "Master Map length: " << masterMap.size() << endl;
   cout << "Execution time: " << time+omp_get_wtime() << endl;
   return(EXIT_SUCCESS);
}
