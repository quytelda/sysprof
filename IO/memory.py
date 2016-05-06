with open('memory.txt') as f:
    f.readline()
    nums = [int(x) for x in f.readline().split()[1:7]]

for i in range(0, 6):
        print nums[i]
