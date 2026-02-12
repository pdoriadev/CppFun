#!/peter/bin/env python3

generalCase = [0, 1, 0, 1, 0, 1, 0, 1]
edgeCase1 = [1, 0, 1, 0]
edgeCase2 = [0]
edgeCase3 = [0, 1]
edgeCase4 = [1, 0]
edgeCase5 = [0, 1, 1, 1, 0, 0]
edgeCase6 = [1, 1, 1, 0, 0, 0]
edgeCase7 = [0, 1, 1, 1, 1, 0, 0, 0]
edgeCase8 = [1, 1, 1, 1, 0, 0, 0, 0]

def listPrintTests() :
	# print each value of list on a new line
	for i in generalCase :
		print(generalCase[i])

	# print list
	print ("\n")
	print (list[0:len(generalCase)])

	# Print formatted string
	print (f"\nn of generalCase list = {len(generalCase)/2}")

# uses type hint for integer list
def addList(generalCase:[int]):
	sum = 0
	for i in list:
		sum += generalCase[i]

def lawnMowerSort(unsortedList:[int]) :
	print(f"List before sort: {unsortedList[0:len(unsortedList)]}")
	n = int(len(unsortedList) / 2) + len(unsortedList)%2
	if n <= 1 :
		print("List is already sorted.", flush=True)
		return

	mowDirection:int = 1
	index:int = 0
	runs:int = 0
	swaps:int = 0
	while runs < n :
		# Swap if values differ
			# swappability depends on value. We want 0's on the left and 1's on the right.
			# 1's get moved right when mowing right. 0's get moved left when mowing left. 
			# mowDirection switches between 1 and -1. 
		if unsortedList[index]*mowDirection > unsortedList[index+mowDirection]*mowDirection :
			swap:int = unsortedList[index+mowDirection]
			unsortedList[index+mowDirection] = unsortedList[index]
			unsortedList[index] = swap
			swaps += 1
		print(f"index = {index}, {unsortedList[0:len(unsortedList)]}")
		# Increment index in current direction we are "mowing".
		index += mowDirection
		# Bounds check to change mow direction
		if index+mowDirection < 0 or index+mowDirection >= len(unsortedList) :
			mowDirection *= -1
			runs += 1

	print(f"List after sort: {unsortedList[0:len(unsortedList)]}")
	print(f"Mows: {runs/2}\nswaps: {swaps}", flush=True)

def alternateLawnMowerSort(unsortedList:[int]):
	print(f"List before sort: {unsortedList[0:len(unsortedList)]}")
	n = len(unsortedList) / 2
	if n < 1 :
		print("List is already sorted.", flush=True)
		return

	startingIndex:int = 0
	index:int = 0
	swaps:int = 0
	runs:int = 0
	while runs < n :
		# Swap up
		if unsortedList[index] > unsortedList[index+1] :
			swap:int = unsortedList[index+1]
			unsortedList[index+1] = unsortedList[index]
			unsortedList[index] = swap
			swaps += 1
		# print(f"index = {index},: {unsortedList[0:len(unsortedList)]}")

		index += 2
		# Bounds check
		if len(unsortedList) == 2:
			break
		elif index+1 >= len(unsortedList) :
			# flips starting index between one and zero
			startingIndex = 1 - startingIndex
			index = startingIndex
			runs += 1

	print(f"List after sort: {unsortedList[0:len(unsortedList)]}")
	print(f"Runs: {runs}\nswaps: {swaps}")


if __name__ == "__main__":
	print("=============================\nLawn Mower Sorts")
	lawnMowerSort(generalCase.copy())
	lawnMowerSort(edgeCase1.copy())
	lawnMowerSort(edgeCase2.copy())
	lawnMowerSort(edgeCase3.copy())
	lawnMowerSort(edgeCase4.copy())
	lawnMowerSort(edgeCase5.copy())
	lawnMowerSort(edgeCase6.copy())
	lawnMowerSort(edgeCase7.copy())
	lawnMowerSort(edgeCase8.copy())


	print("=============================\nAlternate Lawn Mower Sorts")
	alternateLawnMowerSort(generalCase.copy())
	alternateLawnMowerSort(edgeCase1.copy())
	alternateLawnMowerSort(edgeCase2.copy())
	alternateLawnMowerSort(edgeCase3.copy())
	alternateLawnMowerSort(edgeCase4.copy())
	alternateLawnMowerSort(edgeCase5.copy())
	alternateLawnMowerSort(edgeCase6.copy())
	alternateLawnMowerSort(edgeCase7.copy())
	alternateLawnMowerSort(edgeCase8.copy())

# class Disk


# class Disks


# def lawnMower(Disks n) :

