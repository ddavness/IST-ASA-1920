import os

min = 5
max = 100
step = 5

for i in range(min, max + step, step):
	command = "./a.out " + str(i) + "0000 " + str(i) + "00000 1 1 " + str(i) + "0000 > test" + str(i) + "0k"
	print(command)
	os.system(command)