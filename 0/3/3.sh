#!/usr/bin/env bash

# sed -r "s/^/"

vars=$(cat compile.conf | sed -r -n "s/[a-zA-Z0-9_]*=([a-zA-Z0-9_\ ]*)/\1/p")
sum=0
for int in $(echo "$vars" | sed -r -n '/[0-9]+/p')
do
    sum=$(($sum+$int))
done
vars="$(echo "$vars" | sed -r -n '/[a-zA-Z_\ ]+/p')
$sum"
echo "$vars"