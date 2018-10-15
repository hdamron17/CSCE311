#! /bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <executable>"
  exit 1
fi

dir=$(dirname $0)
prog=$1

bigtotalstr=""
results=""
for ifile in $dir/*.in; do
  n=$(basename $ifile .in)
  for ofile in $n.*.out; do
    m=$(basename $ofile .out)
    totalstr=""
    key=$(head -n1 $ofile)
    diffstr=$(diff <(tail -n +2 $ofile) <(echo "$key" | $prog $ifile))
    if [ ! -z "$diffstr" ]; then
      echo "$diffstr"
      totalstr="$totalstr$diffstr"
      bigtotalstr="$bigtotalstr$diffstr"
      results=$(printf "$results\nTest $m failed")
    else
      results=$(printf "$results\nTest $m passed")
    fi
  done
done
echo "$results"
if [ ! -z "$bigtotalstr" ]; then
  echo "Some test(s) failed"
else
  echo "All tests passed"
fi
