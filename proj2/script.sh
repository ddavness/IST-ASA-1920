#!/usr/bin/env bash

while [ $? -eq 0 ]
do
	python vendor/*.py -n 5 -N 5 -m 5 -M 5 -c 1 -C 10 -s 1 -S 10 > test
	./proj-asa < test > /dev/null && rm test
done

echo "Project threw error, test input is in file \"test\""