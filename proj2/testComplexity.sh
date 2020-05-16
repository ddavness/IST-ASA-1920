#!/usr/bin/env bash

min=100
max=1000
step=100

executableName="./proj-asa"

for (( c=$min; c<=$max; c+=$step ))
do
	echo "-----------------"
	echo "Value: $c"
	input=$(python vendor/testgen.py -n $c -N $c -m $c -M $c -c $(( $c/4 )) -C $(( $c/4 )) -s $(( $c/4 )) -S $(( $c/4 )))
	time echo $input | $executableName > /dev/null
done

echo "-----------------"