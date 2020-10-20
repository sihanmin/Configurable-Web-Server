#!/bin/bash

exit_status=0
PATH_TO_TESTS=../tests
PATH_TO_SERVER="bin/server"

if [[ ! -x $PATH_TO_SERVER ]]
then
  echo Executable server does not exist.
  exit_status=1
fi

$PATH_TO_SERVER ${PATH_TO_TESTS}/test_config/test_config &
pid=$!
sleep 1

# Test 1: Handle echo requests
curl -s "http://localhost:8080/echo" > test_result_1.txt
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server using curl.
  exit_status=1
fi

diff test_result_1.txt ${PATH_TO_TESTS}/expected_result_1.txt
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of echo requests.
  exit_status=1
fi


# Test 2: Handle invalid requests
echo "invalid request" | nc -q 1 localhost 8080 > test_result_2.txt
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server using nc.
  exit_status=1
fi

diff test_result_2.txt ${PATH_TO_TESTS}/expected_result_2.txt
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of invalid requests.
  exit_status=1
fi

# Test 3: Handle static requests
curl -s "http://localhost:8080/static/no_extension" > test_result_3.txt
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server using curl.
  exit_status=1
fi

diff test_result_3.txt ${PATH_TO_TESTS}/static/no_extension
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of static requests.
  exit_status=1
fi

# Test 4: Handle static requests
curl -s "http://localhost:8080/static" > test_result_4.txt
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server using curl.
  exit_status=1
fi

diff test_result_4.txt ${PATH_TO_TESTS}/static/index.html
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of static requests.
  exit_status=1
fi

Test 5: Handle status requests
curl -s "http://localhost:8080/status" > test_result_5.txt
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server using curl.
  exit_status=1
fi

diff test_result_5.txt ${PATH_TO_TESTS}/expected_result_5.txt
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of status requests.
  exit_status=1
fi

# Test 6: Handle proxy requests
$PATH_TO_SERVER ${PATH_TO_TESTS}/test_config/test_proxy_config &
proxy_pid=$!
sleep 1

curl -s "http://localhost:5000/static/test.html" --output test_expected_result_6.html
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server http://localhost:5000/static/test.html using curl.
  kill -9 $pid
  kill -9 $proxy_pid
  exit_status=1
fi

curl -s "http://localhost:8080/another_server/static/test.html" --output test_result_6.html
if [ ! $? == 0 ]
then
  echo Test failed: Cannot connect to server http://localhost:8080/another_server/static/test.html using curl.
  kill -9 $pid
  kill -9 $proxy_pid
  exit_status=1
fi

diff test_expected_result_6.html test_result_6.html
if [ ! $? == 0 ]
then
  echo Test failed: Incorrect handling of proxy requests.
  kill -9 $pid
  kill -9 $proxy_pid
  exit_status=1
fi

# Finished all tests. Clean-up & kill server.
rm -f test_result_1.txt test_result_2.txt test_result_3 test_result_4.txt file_0000000000.log test_expected_result_6.txt test_result_6.txt # Can comment out this line to inspect test results.
kill -9 $pid
kill -9 $proxy_pid

if [ $exit_status == 0 ]
then
  echo Passed all tests.
fi

exit $exit_status
