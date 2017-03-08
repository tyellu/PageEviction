#!/bin/bash

#memory array
mem[0]=50
mem[1]=100
mem[2]=150
mem[3]=200

#algorithm names
alg[0]=rand
alg[1]=fifo
alg[2]=lru
alg[3]=clock
alg[4]=opt

#tracefile
trace[0]="tr-simpleloop.ref"
trace[1]="tr-matmul.ref"
trace[2]="tr-blocked.ref"
trace[3]="tr-is_palindrome.ref"

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
