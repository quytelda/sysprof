with open('disk.txt') as f:
    f.readline()
    f.readline()
    f.readline()
    line = f.readline()
    while line:

        if(line != '\n'):

                nums = [float(x) for x in line.split()[1:6]]

                print ("%f  %f  %f  %f  %f" %(nums[0], nums[1], nums[2], nums[3], nums[4]))

        line = f.readline()
