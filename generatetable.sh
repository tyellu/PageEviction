#!/bin/bash

#memory array
mem[0]=50
mem[1]=100
mem[2]=150
mem[3]=200

#algorithm names
alg[0]=rand
alg[1]=fifo
alg[2]=clock
alg[3]=lru
alg[3]=opt

#tracefile
trace[0]="tr-is_palindrome.ref"

for t in "${trace[@]}"
do
  for m in "${mem[@]}"
  do
    for a in "${alg[@]}"
    do
      printf "  Mem size: $m"
      printf "  Trace file: $t"
      printf "  Algorithm: $a\n"
      printf "  Command: ./sim -f traceprogs/$t -m $m -s 100000 -a $a"
      ./sim -f "traceprogs/$t" -m $m -s 100000 -a $a
      printf "\n\n"
    done
  done
done
