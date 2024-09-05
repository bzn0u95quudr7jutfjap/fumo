#!/bin/sh

DIR="./build/"

[[ -f "$DIR" ]] && mkdir "$DIR" && cd "$DIR" && cmake .. && cd -

make -C "$DIR"

find "$DIR" -type f -name '*test*' -executable -execdir '{}' \;
