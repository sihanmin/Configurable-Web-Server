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
sleep 1 # wait for the server to be set up

# Send incomplete request by waiting 1 sec per line
printf '%s\r\n%s\r\n%s\r\n%s\r\n\r\n' \
"GET / HTTP/1.1" \
"User-Agent: nc" \
"Host: 127.0.0.1" \
"Accept: */*" \
| nc -i 1 127.0.0.1 8080 > incomplete.txt &
nc_pid=$!

# Send complete request. This should finish before the incomplete one.
curl "localhost:8080" > complete.txt

sleep 10 # wait for both request to be handled

# Check results
diff ${PATH_TO_TESTS}/expected_incomplete.txt incomplete.txt
if [ ! $? == 0 ]
then
  echo Test failed: The partial request is not handled properly.
  exit_status=1
fi

diff ${PATH_TO_TESTS}/expected_complete.txt complete.txt
if [ ! $? == 0 ]
then
  echo Test failed: The complete request is not handled properly.
  exit_status=1
fi

incomplete_time=`stat -c "%Y" incomplete.txt`
complete_time=`stat -c "%Y" complete.txt`
if [ ! $(($incomplete_time-$complete_time)) -gt 0 ]
then
  echo Test failed: The order of request handling is wrong, i.e. the complete request blocks.
  exit_status=1
fi

# Finished all tests. Clean-up & kill processes.
rm -f incomplete.txt complete.txt file_0000000000.log # Can comment out this line to inspect test results.
kill -9 $nc_pid
kill -9 $pid

if [ $exit_status == 0 ]
then
  echo Passed all tests.
fi

exit $exit_status
