#!/bin/bash

rm -r build
mkdir build
cd build
cmake ..
make
make test
cd ../tests && ../build/bin/request_handler_test && cd ..
