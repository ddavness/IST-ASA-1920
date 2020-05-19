#!/usr/bin/env sh

if ! make; then
    echo "Make had some errors there..."
    exit 1
fi

for i in $(ls tests/*.in)
do
    echo "Executing ${i}..."
    ./proj-asa < $i > /tmp/asa.out
    diff --brief -y --color $(echo "${i}" | sed "s/.in/.out/g") /tmp/asa.out
done
