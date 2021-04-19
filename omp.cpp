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
   if (file) {
     ostringstream ss;
     ss << file.rdbuf();
     text = ss.str();
   }
  // while (getline(file, text)) {
      #pragma omp critical
      words->push(text);
  // }
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
   unordered_map<string,int>::iterator it;
   for (auto i : *map) {
      it = (*masterMap).find(i.first);
      if (it != (*masterMap).end()){
        #pragma omp atomic
        it->second += i.second;
      } else {
        #pragma omp critical
        (*masterMap)[i.first] += i.second;
      }
   }
}

int main(int argc, char **argv) {
   const int NUM_FILES = 1000;
   const int NUM_MAPS = omp_get_max_threads();
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
   double timeRead = time + omp_get_wtime();
   cout << "Queue size: " << words.size() << endl;
   cout << "All files Read.... Time taken to Read Files: " << timeRead << endl; 
   #pragma omp parallel for
      for (int i = 0; i < NUM_MAPS; i++) {
            map[i] = Mapper(&words);
            //cout << "Map length: " << map[i].size() << endl;
      }
   //barrier
   cout << "Finished Mapping Queue... Time taken to Map Queue: " << time + omp_get_wtime()-timeRead << endl;
   //cout << "Execution time: " << time+omp_get_wtime() << endl;
   masterMap = map[0];
   #pragma omp parallel for
   for (int i = 1; i < NUM_MAPS; i++) {
      ReduceMapper(&map[i], &masterMap);
   }
   cout << "Finished Reducing Maps..." << endl << endl << endl;
   cout << "**OpenMP Implementation**" << endl << "Number of threads: " << omp_get_max_threads() << endl;
   cout << "Master Map length: " << masterMap.size() << endl;
   cout << "Execution time: " << time+omp_get_wtime() << endl;
   return(EXIT_SUCCESS);
}
