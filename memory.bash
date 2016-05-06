#!/bin/bash

for i in {1..100}
do
        free -m > memory.txt
        python memory.py
        echo ""
        sleep 15
done
