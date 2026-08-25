#!/usr/bin/env python3

def castingTest():
	n = 3
	print(f"n = {n}")
	print(f"n / 2 - No cast: {n/2}")
	print(f"n / 2 - Casted to int: {int(n/2)}")
	print(f"int(n / 2) + n%2 - Casted to int and adjusted up: {int(n/2) +  n%2}")

def stringEquality():
	color = "Red"
	if color != "Blue":
		return 1

import array
# https://docs.python.org/3/library/array.html
def fixedSizeArrayInitializationTest():
    pass    

if __name__ == "__main__":
	#castingTest()
	#stringEquality()
    #fixedSizeArrayInitializationTest()
