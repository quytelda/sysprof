with open('memory.txt') as f:
    f.readline()
    nums = [int(x) for x in f.readline().split()[1:7]]

print ("%f  %f  %f  %f  %f  %f" %(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]))

#for i in range(0, 6):
#        print nums[i]
