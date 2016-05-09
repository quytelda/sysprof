
#!/bin/bash

for i in {1..100}
do
        sar 1 1 > CPU.txt
        python CPU.py >> CPU_output.txt
        echo "" >> CPU_output.txt

        iostat -d > disk.txt
        python disk.py >> disk_output.txt
        echo "" >> disk_output.txt

        free -m > memory.txt
        python memory.py >> memory_output.txt
        echo "" >> memory_output.txt

        sleep 60
done
