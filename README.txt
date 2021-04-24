How to run the code
Set the valariable  NUM_FILES to however many files you want to run. In our case, we used 1000 for all the tests

Sequential:
icc -std=c++11 -qopenmp sequential.cpp -o sequential
sbatch sequential.sub

OpenMP:
Set the number of threads in omp.sub by setting --ntasks parameter

icc -std=c++11 -qopenmp omp.cpp -o omp
sbatch omp.sub

MPI:
Set the number of threads per task in mpi.cpp by setting the variable threads.

Set the number of threads per task is mpi.sub by setting --ntasks-per-node. This should be the same value as in the threads variable in mpi.cpp

Set the number of nodes by setting the --nodes parameter in mpi.sub. This should also be the same value after the -n flag in the run command.

mpiicc -qopenmp -std=c++11 mpi.cpp -o mpi
sbatch mpi.sub
