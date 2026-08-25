

# def colorStrToNumber(diskColorString)
	if diskColorString is "Black"
		return 1
	return 0

### n is a positive integer that = int(listLength / 2) + (listLength % 2)
	### the modulo accounts for odd-numbered lengths
# def lawnMowerSort(diskList, n) :
	if listLength <= 1 , return

	mowDirection = 1
	totalMows = 0
	# iterates through the list based on the mow direction
	while totalMows < n / 2 : 
		if colorStrToNumber(elementAtCurrentIndex)*mowDirection > colorStrToNumber(nextElementInMowDirection)*mowDirection :
			swap = nextElementInMowDirection
			nextElementInMoveDirection = elementAtCurrentIndex
			elementAtCurrentIndex = swap

		# Increment index in current direction we are "mowing".
		index += mowDirection

		# Bounds check to change mow direction
		if index+mowDirection < 0 or index+mowDirection >= listLength :
			mowDirection *= -1
			totalMows += 0.5

### n is an integer that = int(listLength / 2) + (listLength % 2)
	### the modulo accounts for odd-numbered lengths
# def alternateLawnMowerSort(diskList, n):
	if listLength <= 1, return

	index = 0
	runs = 0
	# iterates through the list
	while runs < n :
		if colorStrToNumber(elementAtCurrentIndex) > colorStrToNumber(elementAfterCurrentIndex) :
			swap = nextElement
			nextElement = element
			element = swap

		index+=2

		# Bounds check
		if 2 + the current index >= listLength:
			# shifts starting index between one and zero
			startingIndex = 1 - startingIndex
			index = startingIndex
			runs += 1

if __name__ == "__main__":
	print("=============================\nLawn Mower Sorts")
	lawnMowerSort(generalCase.copy())
	lawnMowerSort(edgeCase1.copy())
	lawnMowerSort(edgeCase2.copy())
	lawnMowerSort(edgeCase3.copy())
	lawnMowerSort(edgeCase4.copy())
	lawnMowerSort(edgeCase5.copy())
	lawnMowerSort(edgeCase6.copy())

	print("=============================\nAlternate Lawn Mower Sorts")
	alternateLawnMowerSort(generalCase.copy())
	alternateLawnMowerSort(edgeCase1.copy())
	alternateLawnMowerSort(edgeCase2.copy())
	alternateLawnMowerSort(edgeCase3.copy())
	alternateLawnMowerSort(edgeCase4.copy())
	alternateLawnMowerSort(edgeCase5.copy())
	alternateLawnMowerSort(edgeCase6.copy())

# class Disk


# class Disks


# def lawnMower(Disks n) :

