#! /bin/bash

ARRAY_SIZE=$1
STRIDE=1

while [[ $ARRAY_SIZE -ne 0 ]]
do
    echo "$RANDOM" >> $2
    ARRAY_SIZE=$(( ARRAY_SIZE - STRIDE ))
done
