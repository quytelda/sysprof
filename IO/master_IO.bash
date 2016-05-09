#!/bin/bash

if [ ! -f CPU_pipe ]; then
        mkfifo CPU_pipe
fi

if [ ! -f disk_pipe ]; then
        mkfifo disk_pipe
fi

if [ ! -f memory_pipe ]; then
        mkfifo memory_pipe
fi

for i in {1..100}
do
        sar 1 1 > CPU.txt
        python CPU.py >> CPU_pipe
        echo "Wrote to cpu"

        iostat -d > disk.txt
        python disk.py >> disk_pipe
        echo "Wrote to disk"

        free -m > memory.txt
        python memory.py >> memory_pipe
        echo "Wrote to memory"

        sleep 60
done
