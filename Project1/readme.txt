Hunter Damron
Graham McDonald
Reed Segars

To compile:
  Run make from the root directory

To run:
  Part 1: run ./bin/part1 <filename> <wordtofind>
  Part 2: run ./bin/part2 <filename> <wordtofind>
  Part 3: run ./bin/part3 <filename> <wordtofind>


Team Contributions:
  Hunter Damron worked on:
    Pipe
    Unix Domain Socket
    Shared Memory
  Graham McDonald worked on:
    Unix Domain Socket
    One method in Shared Memory
    Documentation
  Reed Segars worked on:
    File IO
    Shared Memory
    Documentation

Description:
  All Parts
    In all parts, the parent reads the file, and the child searches the file for lines that contain the word that is input as the 2nd argument. The child sends those lines back to the parent. The parent outputs them in alphabetical order.
  Part 1
    Part 1 uses pipes to send and receive data between the parent and child processes.
  Part 2
    Part 2 uses Unix domain sockets, specifically socketpair() to send and receive data between the parent and child processes.
  Part 3
    Part 3 uses shared memory to allow the parent and child processes to access the file content. The child creates 4 threads.
