#!/bin/bash

find ../src/ -iname "*.h" -o -iname "*.cpp" -print0 | xargs -0 clang-format --verbose -i --style=file

