#!/bin/bash

export CC="/usr/bin/clang"
export CXX="/usr/bin/clang++"
cmake -S .. -B ../build/ -DUSE_QT6:BOOL=FALSE
