#!/bin/bash
pushd .
cd ../src
mkdir -p ../build/include-dependency-graph/
perl -w ../script/perl/create-include-dependency-graph.pl > ../build/include-dependency-graph/src-all.dot
cd ../build/include-dependency-graph
dot -Tpdf ./src-all.dot -o src-all.pdf
#dot -Tps ./src-all.dot -o src-all.ps
popd
