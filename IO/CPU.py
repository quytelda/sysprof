with open('CPU.txt') as f:
    f.readline()
    f.readline()
    f.readline()
    nums = [float(x) for x in f.readline().split()[3:9]]

print "%f  %f  %f  %f  %f  %f" %(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5])

#for i in range(0, 6):
#        print nums[i]
