#! /usr/bin/env python3

import sys
import re

def main():
  args = ["<search_file>", "<key>"]
  if len(sys.argv) != len(args) + 1:
    print("Usage: " + " ".join([sys.argv[0]] + args))
    return
  lines = []
  ifile, key = sys.argv[1:1+len(args)]
  keyregex = re.compile(r'(^|[^a-zA-Z])%s($|[^a-zA-Z])' % key, re.IGNORECASE)
  with open(ifile, "r") as ifile:
    for l in ifile:
      if keyregex.search(l) is not None:
        lines.append(l[:-1])
  lines.sort(key=str.lower)
  print("\n".join(lines))

if __name__ == "__main__":
  main()
