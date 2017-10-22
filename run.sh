#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$(pwd)/bin/debug

test/bin/debug/testbin $1


