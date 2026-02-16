#!/peter/bin/env python3

# algorithm inputs:
	# Always start with a light disk on the left
	# Always of length 2n, where n is the number of light or dark disks. 
generalCase1 = [0, 1, 0, 1, 0, 1, 0, 1]
generalCase2 = [0, 1, 0, 1]
generalCase3 = [0, 1, 0, 1 , 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1]
edgeCase1 = []
edgeCase2 = [0, 1]


# print each value of list on a new line
for i in generalCase1 :
	print(generalCase1[i])

# print list
print ("\n")
print(list[0:len(generalCase1)])


# Print formatted string
print (f"\nn of generalCase1 list = {len(generalCase1)/2}")

# uses type hint for integer list
def addList(generalCase1:[int]):
	sum = 0
	for i in list:
		sum += generalCase1[i]

class Disk(object):
	type =""
	def __init__(self, type):
		if int(type) <= 0:
			self.type="white"
		else:
			self.type = "black"
	def __iter__(self):
		return self.type

diskCase1 = [Disk(0), Disk(1), Disk(0), Disk(1), Disk(0), Disk(1), Disk(0), Disk(1)]
diskCase2 = [Disk(0), Disk(1), Disk(0), Disk(1)]
diskEdgeCase1 = []
diskEdgeCase2 = [Disk(0), Disk(1)]

def colorToNumber(disk):
	if disk.type == "white":
		return 0
	return 1


# Summation: 4 ||  2 + 4 + (n) * ( (n/2)((2*cToN + 1) + (2cToN + 5) +  1 + 3n) + 1
#		   6 + (2n*cToN + 5n + n + 4(n^2/2)) = 6 + 2n*cToN + 6n + (4n^2)/2 = (4n^2)/2 + n(6 + 2*cToN) + 6
# If length = 8, n = 4, therefore: 4(16)/2 + 24 + 8*cToN + 6 = 62 + 8*cToN
def lawnMowerSort(diskList:[int], n:int) :
	print(f"List before sort: [", end='')
	for i in diskList:
		print(f"{i.type}", end='')
		if i != diskList[len(diskList)-1]:
			print(", ", end='')
	print("]")

	# 2 || 4
	if len(diskList) <= 2 : # 2 || 2 + 2
		print("List is already sorted.")
		return

	# 4
	# starting mow direction is positive
	mowDirection:int = 1 # 1
	index:int = 1 # 1
	mows:int = 0 # 1
	swaps:int = 0 # 1

	# (n/2)((2cToN + 5) + 1 + 4)
	while mows < n/2 : # (n/2)*LoopSteps
		# Swap if values differ
			# swappability depends on value. We want 0's on the left and 1's on the right.
			# 1's get moved right when mowing right. 0's get moved left when mowing left.
			# mowDirection switches between 1 and -1.
		if colorToNumber(diskList[index])*mowDirection > colorToNumber(diskList[index+mowDirection])*mowDirection : # (n/2)*(2cToN + 1) + (n/2)(2cToN + 1 + 4)
			swap:int = diskList[index+mowDirection] # 1
			diskList[index+mowDirection] = diskList[index] # 1
			diskList[index] = swap # 1
			swaps += 1 # 1

		# print(f"sort {index}: {diskList[0:len(diskList)]}")
		# Increment index in current direction we are "mowing".
		index += mowDirection # n*1
		# Bounds check to change direction
		if index+mowDirection <= 0 or index+mowDirection >= len(diskList)-1 : # (n/2)2 + (n/2)(2+1+1) - (n/2) = times mow direction changes - includes len() func call
			mowDirection *= -1 # 1
			mows += 0.5 # 1

	print(f"List after sort: [", end='')
	for i in diskList:
		print(f"{i.type}", end='')
		if i != diskList[len(diskList)-1]:
			print(", ", end='')
	print("]")
	print(f"Mows: {mows}\nswaps: {swaps}")
	return swaps # 1

# Starts with leftmost disk. Goes to the right every 2 indexes.
# When it reaches the end starts at the first or second-most left index.
# Checks adjacent disks for swaps (i.e. k, k+1)
# Total of n runs
def alternateSort(diskList:[int], n:int):
	print(f"List before sort: [", end='')
	for i in diskList:
		print(f"{i.type}", end='')
		if i != diskList[len(diskList)-1]:
			print(", ", end='')
	else: print("]")
	print(f"n = {n}")

	# 4 || 2
	if len(diskList) <= 2 :
		print("List is already sorted.")
		return

	# 4
	startingIndex:int = 0
	index:int = 0
	runs:int = 0
	swaps:int = 0
	steps:int = 6
	# n
	while runs < n:
		# (n/2)[(2cToN + 1) + (2cToN + 5)]
		# compare values. If left is greater, then swap.
		if colorToNumber(diskList[index]) > colorToNumber(diskList[index+1]):
			temp:int = diskList[index]
			diskList[index] = diskList[index+1]
			diskList[index+1] = temp
			swaps +=1
			steps +=8
			print(f"Swapped. steps={steps}")
		else:
			steps +=4
			print(f"NO swap. steps={steps}")

		# 1
		# print(f"Index {index}: {diskList[0:len(diskList)]}")
		index += 2
		steps+=1

		# (n/2)2 + (n/2)(5)
		# Bounds check. Never going to swap when index is equal to the second to last index.
		if index >= len(diskList)-2 :
			startingIndex = 1 - startingIndex
			index = startingIndex
			runs += 1
			steps+=5
			print(f"Looped. steps={steps}")
		else:
			steps+=2
			print(f"NOloop. steps={steps}")

	print(f"List before sort: [", end='')
	for i in diskList:
		print(f"{i.type}", end='')
		if i != diskList[len(diskList)-1]:
			print(", ", end='')
	else: print("]")
	print(f"Swaps={swaps}\nRuns={runs}")
	steps+=1
	print(f"Steps={steps}")
	# 1
	return swaps



if __name__ == "__main__":
	# lawnMowerSort(generalCase1.copy(), len(generalCase1) / 2)
	# lawnMowerSort(generalCase2.copy(), len(generalCase2) / 2)
	# lawnMowerSort(generalCase3.copy(), len(generalCase3) / 2)
	# lawnMowerSort(edgeCase1.copy(), len(edgeCase1) / 2)
	# lawnMowerSort(edgeCase2.copy(), len(edgeCase2) / 2)

	# alternateSort(generalCase1.copy(), len(generalCase1) / 2)
	# alternateSort(generalCase2.copy(), len(generalCase2) / 2)
	# alternateSort(generalCase3.copy(), len(generalCase3) / 2)
	# alternateSort(edgeCase1.copy(), len(edgeCase1) / 2)
	# alternateSort(edgeCase2.copy(), len(edgeCase2) / 2)

	lawnMowerSort(diskCase1.copy(), len(diskCase1) / 2)
	lawnMowerSort(diskCase2.copy(), len(diskCase2) / 2)
	lawnMowerSort(diskEdgeCase1.copy(), len(diskEdgeCase1) / 2)
	lawnMowerSort(diskEdgeCase2.copy(), len(diskEdgeCase2) / 2)

	alternateSort(diskCase1.copy(), len(diskCase1) / 2)
	alternateSort(diskCase2.copy(), len(diskCase2) / 2)
	alternateSort(diskEdgeCase1.copy(), len(diskEdgeCase1) / 2)
	alternateSort(diskEdgeCase2.copy(), len(diskEdgeCase2) / 2)

# class Disk


# class Disks


# def lawnMower(Disks n) :

