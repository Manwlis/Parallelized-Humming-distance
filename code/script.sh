#!/bin/bash

# Description :	Compile and run all the variations of the algorithm witn the same input.

echo "The script starts."
echo ""

#Compile the code.
gcc -o serial serial.c
gcc -fopenmp -o OMP_char OMP_char.c
gcc -fopenmp -o OMP_string OMP_string.c
gcc -fopenmp -o OMP_line OMP_line.c
gcc pthread_line.c -o pthread_line -lpthread -Wall -lm
gcc pthread_string.c -o pthread_string -lpthread -Wall
gcc pthread_char.c -o pthread_char -lpthread -Wall

#Script Variables.
A="1000"
B="1000"
LENGTH="1000"
THREADS="4"

echo "Num of strings: $A, $B"
echo "Size of strings: $LENGTH"
echo "Threads: $THREADS"

#Serial execution.
./serial "$A" "$B" "$LENGTH"

echo ""
echo "OMP algorithms:"

#Parallel execution with OMP API.
./OMP_char "$A" "$B" "$LENGTH" "$THREADS"
./OMP_string "$A" "$B" "$LENGTH" "$THREADS"
./OMP_line "$A" "$B" "$LENGTH" "$THREADS"

echo ""
echo "Pthreads algorithms:"

#Parallel execution with pthreads.
./pthread_char "$A" "$B" "$LENGTH" "$THREADS"
./pthread_string "$A" "$B" "$LENGTH" "$THREADS"
./pthread_line "$A" "$B" "$LENGTH" "$THREADS"
