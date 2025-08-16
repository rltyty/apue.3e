#!/bin/sh

printf "I'm a.sh, here are my argv:\n"

i=0;
for arg in $@; do
  printf "argv[$i] = $arg\n"
  i=$((i+1))
done
printf "\n"
printf "I'm a.sh, here is my environment.\n"

/usr/bin/env

printargv a b c
