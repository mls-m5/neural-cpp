#!/usr/bin/env bash

mkdir -p fixed/cats

rm -f list.txt

( cd raw/ && find . -type f -iname "*.*" -print0) | while IFS= read -r -d $'\0' line; do
    echo "$line"
    echo "$line" >> list.txt
    #ls -l "raw/$line"
    mkdir -p $(dirname "fixed/$line")
    convert "raw/$line" -define jpeg:size=200x200 -gravity center -thumbnail 100x100^ -extent 100x100 "fixed/$line"
done
