#!/bin/bash

for i in {1..100}
do
        sar 1 1 > CPU.txt
        python CPU.py >> CPU_output.txt
        echo "" >> CPU_output.txt
        sleep 15
done
