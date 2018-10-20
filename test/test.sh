#! /bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <executable>"
  exit 1
fi
echo "Testing $(basename $1)"
echo "------"

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
    outputkey="$(tail -n +2 $ofile)"
    output="$($prog $ifile $key)"
    diffstr="$(diff <(echo -n "$outputkey") <(echo -n "$output"))"
    if [ ! -z "$results" ]; then
      results=$(printf "${results}\nTest $m ")
    else
      results=$(printf "${results}Test $m ")
    fi
    if [ ! -z "$diffstr" ]; then
      echo "~$m Failure Diff"
      echo ">~~~~~"
      # echo "$diffstr"
      echo "$(diff -y <(echo -n "$outputkey") <(echo -n "$output"))"
      echo "~~~~~<"
      totalstr="$totalstr$diffstr"
      bigtotalstr="$bigtotalstr$diffstr"
      results="$results failed"
    else
      results="$results passed"
    fi
  done
done
echo "$results"
if [ ! -z "$bigtotalstr" ]; then
  echo "Some test(s) for $(basename $1) failed"
  exit 1
else
  echo "All tests for $(basename $1) passed"
fi
echo "======"
