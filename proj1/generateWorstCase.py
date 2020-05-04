import os
import sys

x = eval(sys.argv[1])

print(f"{x},{x - 1}")
for i in range(x):
	print(i + 1)

for i in range(x - 1):
	print(i + 1, " ", i + 2)
