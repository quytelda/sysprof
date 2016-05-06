#!/bin/bash

for i in {1..100}
do
        sar 1 1 > CPU.txt
        python CPU.py
        echo ""
        sleep 15
done
