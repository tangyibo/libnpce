#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$(pwd)/lib

bin/testbin $1


