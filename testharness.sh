#!/usr/bin/env bash


test_result() {
   if [ "$1" -eq "0" ]; then
       echo "$2 (PASSED)"
   else
       echo "$2 (FAILED)"
   fi
}


# -------------------------------
# Number of test cases.
# update this number as needed.
# -------------------------------
N=18


for ((i = 1; i < N+1; i++)); do
   echo "----------------------------"
   ./testcase "tc$i"
   ec=$?
   test_result "$ec" "tc$i"
   sleep 1
done


# ----------------------------------------------
# Honors section will included test cases below
# ----------------------------------------------


# test for memory leaks
echo "----------------------------"
echo "Memory leak test"


MEM_FAIL=0


for ((i = 1; i < N+1; i++)); do
   valgrind --leak-check=full \
        --error-exitcode=1 \
       ./testcase "tc$i" > /dev/null 2>&1


   ec=$?
   test_result "$ec" "tc$i memory leak test"


   if [ "$ec" -ne "0" ]; then
       MEM_FAIL=1
   fi
done


if [ "$MEM_FAIL" -eq "0" ]; then
   echo "Memory leak test (PASSED)"
else
   echo "Memory leak test (FAILED)"
fi




# check for zombies
# echo "----------------------------"
# echo "Zombie process test"
# sim.c doesn't have any child processes



