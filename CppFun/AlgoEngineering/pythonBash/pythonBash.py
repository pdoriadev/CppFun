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

def lawnMowerSort(diskList:[int], n:int) :
	print(f"List before sort: {diskList[0:len(diskList)]}")
	if len(diskList) <= 2 :
		print("List is already sorted.")
		return

	# starting mow direction is positive
	mowDirection:int = 1
	index:int = 1
	mows:int = 0
	swaps:int = 0

	while mows < n/2 :
		# Swap if values differ
			# swappability depends on value. We want 0's on the left and 1's on the right.
			# 1's get moved right when mowing right. 0's get moved left when mowing left. 
			# mowDirection switches between 1 and -1. 
		if diskList[index]*mowDirection > diskList[index+mowDirection]*mowDirection :
			swap:int = diskList[index+mowDirection]
			diskList[index+mowDirection] = diskList[index]
			diskList[index] = swap

			swaps += 1

		# print(f"sort {index}: {diskList[0:len(diskList)]}")
		# Increment index in current direction we are "mowing".
		index += mowDirection
		# Bounds check to change direction
		if index+mowDirection <= 0 or index+mowDirection >= len(diskList)-1 :
			mowDirection *= -1
			mows += 0.5
	print(f"List after sort: {diskList[0:len(diskList)]}")
	print(f"Mows: {mows}\nswaps: {swaps}")

	return swaps

def alternateSort(diskList:[int], n:int):
	print(f"List before sort: {diskList[0:len(diskList)]}")
	if len(diskList) <= 2 :
		print("List is already sorted.")
		return

	startingIndex:int = 0
	index:int = 0
	runs:int = 0
	swaps:int = 0
	while runs <= n/2 :
		# compare values. If left is greater, then swap.
		if diskList[index] > diskList[index+1]:
			temp:int = diskList[index]
			diskList[index] = diskList[index+1]
			diskList[index+1] = temp
			swaps +=1

		print(f"Index {index}: {diskList[0:len(diskList)]}")
		index += 2

		# Bounds check. Never going to swap when index is equal to the second to last index. 
		if index >= len(diskList)-2 :
			startingIndex = 1 - startingIndex
			index = startingIndex
			runs += 1
	print(f"List after sort: {diskList[0:len(diskList)]}")
	print(f"Swaps={swaps}\nRuns={runs}")
	return swaps


if __name__ == "__main__":
	lawnMowerSort(generalCase1.copy(), len(generalCase1) / 2)
	lawnMowerSort(generalCase2.copy(), len(generalCase2) / 2)
	lawnMowerSort(generalCase3.copy(), len(generalCase3) / 2)
	lawnMowerSort(edgeCase1.copy(), len(edgeCase1) / 2)
	lawnMowerSort(edgeCase2.copy(), len(edgeCase2) / 2)

	alternateSort(generalCase1.copy(), len(generalCase1) / 2)
	alternateSort(generalCase2.copy(), len(generalCase2) / 2)
	alternateSort(generalCase3.copy(), len(generalCase3) / 2)
	alternateSort(edgeCase1.copy(), len(edgeCase1) / 2)
	alternateSort(edgeCase2.copy(), len(edgeCase2) / 2)

# class Disk


# class Disks


# def lawnMower(Disks n) :

