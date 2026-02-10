#!/peter/bin/env python3

generalCase = [0, 1, 0, 1, 0, 1, 0, 1]
edgeCase1 = [1, 0, 1, 0]
edgeCase2 = [0]
edgeCase3 = [0, 1]
edgeCase4 = [1, 0]
edgeCase5 = [0, 1, 1, 1, 0, 0]
cases = [list, edgeCase1, edgeCase2, edgeCase3, edgeCase4]

# print each value of list on a new line
for i in generalCase :
	print(generalCase[i])

# print list
print ("\n")
print(list[0:len(generalCase)])


# Print formatted string
print (f"\nn of generalCase list = {len(generalCase)/2}")

# uses type hint for integer list
def addList(generalCase:[int]):
	sum = 0
	for i in list:
		sum += generalCase[i]

def lawnMowerSort(unsortedList:[int]) :
	print(f"List before sort: {unsortedList[0:len(unsortedList)]}")
	n = len(unsortedList) / 2

	# starting mow direction is positive
	mowDirection:int = 1
	index:int = 0
	mows:int = 0
	swaps:int = 0

	while n >= 1 and mows <= n :
		# Swap if values differ
			# swappability depends on value. We want 0's on the left and 1's on the right.
			# 1's get moved right when mowing right. 0's get moved left when mowing left. 
			# mowDirection switches between 1 and -1. 
		if unsortedList[index]*mowDirection > unsortedList[index+mowDirection]*mowDirection :
			swap:int = unsortedList[index+mowDirection]
			unsortedList[index+mowDirection] = unsortedList[index]
			unsortedList[index] = swap

			swaps += 1

		# Increment index in current direction we are "mowing".
		index += mowDirection
		# Bounds check to change direction
		if index+mowDirection <= 0 or index+mowDirection >= len(unsortedList) :
			mowDirection *= -1
			mows += 1

		print(f"sort {index}: {unsortedList[0:len(unsortedList)]}")

	print(f"List after sort: {unsortedList[0:len(unsortedList)]}")
	print(f"Mows: {mows}\nswaps: {swaps}")

def alternateLawnMowerSort(n):
	pass

if __name__ == "__main__":
	lawnMowerSort(generalCase)
	lawnMowerSort(edgeCase1)
	lawnMowerSort(edgeCase2)
	lawnMowerSort(edgeCase3)
	lawnMowerSort(edgeCase4)


# class Disk


# class Disks


# def lawnMower(Disks n) :

