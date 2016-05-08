#!/bin/bash

for i in {1..100}
do
        free -m > memory.txt
        python memory.py >> memory_output.txt
        echo "" >> memory_output.txt
        sleep 15
done
