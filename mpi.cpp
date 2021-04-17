#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>
#include <omp.h>
#include <mpi.h>

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
   cout << words->size() << endl;
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

//Takes in a map and a pointer to a string and int array
//and converts the map to those arrays. Function will return total number of bytes
int createArrFromMap(unordered_map<string, int> map, char *strArr, int *intArr) {
   int numBytes = 0;
   int n = 0;

   for (auto i : map) {
      char *word = (char *)i.first.c_str();
      memcpy(&(strArr[numBytes]), word, i.first.length() + 1);
      intArr[n++] = i.second;
      numBytes += i.first.length() + 1;
   }
   return numBytes;
}

unordered_map<string, int> reconstructMap(char *strMap, int *intMap, int len) {
   unordered_map<string, int> map;
   int n = 0;
   stringstream buff;
   string str;

   for (int i = 0, j = 0; i < len; i++, j++) {
      //buff << strMap[i];
      //buff >> str;
      if (strMap[i] != '\0') {
         str.push_back(strMap[i]);
      } else { 
         //cout << str << " ";
         j = -1;
         map[str] = intMap[n++];
         //buff.str("");
         str = "";
      }
   }

   return map;
}

void Print1D(char *str, int len) {
   for (int i = 0; i < len; i++) {
      if (str[i] == '\0') {
         cout << " ";
         continue;
      }
      cout << str[i];
   }
}

void PrintArr(char **arr, int len) {
   for (int i = 0; i < len; i++) {
      cout << arr[i] << " ";
   }
}

int main(int argc, char **argv) {
   const int NUM_FILES = 14;
   const int NUM_MAPS = omp_get_num_threads();
   queue<string> words;
   unordered_map<string, int> masterMap;
   unordered_map<string, int> map[NUM_MAPS];

   int pid, numP, N;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numP);
   MPI_Comm_rank(MPI_COMM_WORLD, &pid);
   //cout << "numP = " << numP << endl;

   double time = -omp_get_wtime();
   int numToProc;
   int numExtra;
   
   if (pid == 0) {
      numToProc = NUM_FILES / numP;
      //numExtra = NUM_FILES % numP;
      
      //if the pid is <= numExtra, then it needs to process numToProc + 1
      //else it needs to prcess numToProc

      for (int i = 1; i < numP; i++) {
         int n = numToProc;
         MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
         /*if (i <= numExtra) {
            *n = numToProc + 1;
            MPI_Isend(n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
         } else {
            *n = numToProc;
            MPI_Isend(n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
         }*/
         //pass file i to process (i % nump)
      }
      //numToProc += numExtra;
   } else {
      MPI_Status Stat;
      MPI_Recv(&numToProc, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
   }


   //1 reader per file
   #pragma omp parallel for schedule(guided) //nowait
   for (int i = numToProc * pid + 1; i < NUM_FILES; i++) {
      //cout << i << ".txt is running." << endl;
      //cout << omp_get_thread_num() << endl;
      //#pragma omp critical
      //cout << "PID " << pid << " :" << i << endl; 
      ostringstream stream;
      stream << "files/" << 15 - i << ".txt";
      string filename = stream.str();
      Reader(filename, &words);
      //cout << "lines in queue: " << words.size() << endl;
   }
   if (pid == 0) {
      #pragma omp parallel for schedule(guided)
      for (int i = numP * numToProc + 1; i <= NUM_FILES; i++) {
         ostringstream stream;
         stream << "files/" << 15 - i << ".txt";
         string filename = stream.str();
         //cout << filename << endl;
         Reader(filename, &words);
      }
   }
   //remove this barrier 
   #pragma omp parallel
   {
      #pragma omp single
      for (int i = 0; i < NUM_MAPS; i++) {
         #pragma omp task
         {
            map[i] = Mapper(&words);
            //cout << "map length: " << map[i].size() << endl;
         }
      }
   }
   /*if (pid != 0) {
                         //MPI_INT, int
      MPI_Isend(&map, 1, sizeof(unordered_map<string, int>)
   }*/
   //barrier
   #pragma omp parallel for
   for (int i = 0; i < NUM_MAPS; i++) {
      ReduceMapper(&map[i], &masterMap);
   }
   cout << "PID: " << pid << endl;
   cout << "Master Map length: " << masterMap.size() << endl;
   cout << "Execution time: " << time+omp_get_wtime() << endl << endl;

   if (pid != 0) {
      int mapSize = masterMap.size();
      char *strMap = (char *)malloc(15 * mapSize * sizeof(char));
      int *intMap = (int *)malloc(mapSize * sizeof(int));
      int strMapLength = createArrFromMap(masterMap, strMap, intMap);

      //MPI_Request req;
      MPI_Send(&mapSize, 1, MPI_INT, 0, pid, MPI_COMM_WORLD);
      MPI_Send(&strMapLength, 1, MPI_INT, 0, pid, MPI_COMM_WORLD);
      MPI_Send(strMap, strMapLength, MPI_CHAR, 0, pid, MPI_COMM_WORLD);
      MPI_Send(intMap, masterMap.size(), MPI_INT, 0, pid, MPI_COMM_WORLD);
      free(strMap);
      free(intMap);
   } else {
      //#pragma omp parallel for
      for (int i = 1; i < numP; i++) {
         int mapSize;
         int strMapLength;
         char *strMap;
         MPI_Status req;
         MPI_Recv(&mapSize, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
         MPI_Recv(&strMapLength, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
         strMap = (char *)malloc(strMapLength * sizeof(char));
         MPI_Recv(strMap, strMapLength, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
         int *intMap = (int *)malloc(mapSize * sizeof(int));
         MPI_Recv(intMap, mapSize, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &req);

         //Print1D(strMap, strMapLength);
         
         unordered_map<string, int> map = reconstructMap(strMap, intMap, strMapLength);
         /*for (auto i : map) {
            cout << i.first << ": " << i.second << endl;
         }*/
         free(strMap);
         free(intMap);
         ReduceMapper(&map, &masterMap);
      }
   }
   MPI_Barrier(MPI_COMM_WORLD);
   MPI_Finalize();
   if (pid == 0){
      /*for (auto i : masterMap) {
         cout << i.first << ": " << i.second << endl;
      }*/
     cout << "PID: " << pid << endl;
     cout << "Final Master Map length: " << masterMap.size() << endl;
     cout << "Final Execution time: " << time+omp_get_wtime() << endl;
   
     return(EXIT_SUCCESS);
   }
}
