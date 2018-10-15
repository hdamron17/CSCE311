#! /usr/bin/env python3

import sys
import re

def main():
  if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " <search_file>")
    return
  lines = []
  key = input()  # Possibly print an input message
  keyregex = re.compile(r'(^|\s)%s($|\s)' % key)
  with open(sys.argv[1], "r") as ifile:
    for l in ifile:
      # if key in l:
      if keyregex.search(l) is not None:
        lines.append(l[:-1])
  lines.sort(key=str.lower)
  print("\n".join(lines))

if __name__ == "__main__":
  main()
