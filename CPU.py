with open('CPU.txt') as f:
    f.readline()
    f.readline()
    f.readline()
    nums = [float(x) for x in f.readline().split()[3:9]]

for i in range(0, 6):
        print nums[i]
