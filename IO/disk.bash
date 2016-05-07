
#!/bin/bash

for i in {1..100}
do
        #sar -B 1 1 > disk.txt
        iostat -d > disk.txt
        python disk.py
        echo ""
        sleep 15
done
