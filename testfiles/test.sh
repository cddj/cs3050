#!/bin/bash

failures=0

INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS=1
INPUT_FILE_FAILED_TO_OPEN=2
INPUT_FILE_FAILED_TO_CLOSE=3
PARSING_ERROR_EMPTY_INPUT_FILE=4
OUTPUT_FILE_FAILED_TO_OPEN=5
OUTPUT_FILE_FAILED_TO_CLOSE=6
INTEGER_IS_NOT_A_VERTEX=7
PARSING_ERROR_INVALID_FORMAT=8

# success(status, expectedStatus)
function success() {
  if [[ $1 == $2 ]]; then 
    printf "\x1b[32m[succeeded with code $1]\x1b[0m"
  else
    printf "\x1b[31m[failed with code $1]\x1b[0m"
    let "failures++"
  fi
}


# test(testName, testFile, expectedStatus)
function test() {
  printf "$1\n----------------------------------------------\n"
  time ../a.out -v $2 out.txt
  success "$?" "$3"
  printf "\n----------------------------------------------\n\n"
}


printf "Testing ../a.out (HW3)\n\n"

rm out.txt

printf "reference\n----------------------------------------------\n"
time ../a.out -v input_reference.txt out.txt
succeeded=$?
printf "execution: "
success "$succeeded" "0"
diff out.txt output_reference.txt
printf "\ndiff:      "
success "$?" "0"
printf "\n----------------------------------------------\n\n"

test "1 thousand verticies" "test-1k.txt" 0
test "10 thousand verticies" "test-10k.txt" 0
test "1 million verticies" "test-1m.txt" 0
test "Invalid edge" "test-invalid-edge.txt" $INTEGER_IS_NOT_A_VERTEX
test "Zero edge" "test-invalid-edge-2.txt" $INTEGER_IS_NOT_A_VERTEX
test "Zero" "test-zero.txt" 0
test "Empty files" "test-empty.txt" $PARSING_ERROR_EMPTY_INPUT_FILE
test "Bad input" "test-bad.txt" $PARSING_ERROR_INVALID_FORMAT
test "Bad input file" "dskfjn.txt" $INPUT_FILE_FAILED_TO_OPEN

if [[ $failures == 0 ]]; then
  printf "\x1b[32m[ALL TESTS PASSED]\x1b[0m\n\n";
else
  printf "\x1b[31m[$failures TESTS FAILED]\x1b[0m\n\n";
fi