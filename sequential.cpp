#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char **argv) {
   const int NUM_FILES = 1000;
   unordered_map<string, int> map;
   double time = -omp_get_wtime();

   for (int i = 1; i <= NUM_FILES; i++) {
      ostringstream stream;
      stream << "files/" << i << ".txt";
      string fileName = stream.str();
      ifstream file;
      file.open(fileName);
      string word;
      while (file >> word) {
         map[word]++;
      }
   }

   time += omp_get_wtime();
   cout << "**Sequential Execution**" << endl;
   cout << "Map length is: " << map.size() << endl;
   cout << "Time taken: " << time << endl;
   return(EXIT_SUCCESS);
}
