#!/usr/bins/env bash
./echo_arg csc209 > echo_out.txt
./echo_stdin < echo_stdin.c
./count 210 | wc -c
ls -S1 | ./echo_stdin
