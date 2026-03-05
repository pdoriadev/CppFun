def swapItem(names, weights, calories, i, j):
			itemSwap:int = names[j]
			weightSwap:int = weights[j]
			caloriesSwap:int = calories[j]

			names[j] = names[i]
			weights[j] = weights[i]
			calories[j] = calories[i]

			names[i] = itemSwap
			weights[i] = weightSwap
			calories[i] = caloriesSwap


def greedyAlgo(maxWeight, names, weights, calories):
	sumWeight:int = 0

	print(f"Pre-sorted Items: {names}")

	# selection sort
	# sort items by calories/weight
	for i in range (0, len(names) - 1):
		best = i
		j = i + 1
		while j < len(names):
			bestCalToWeight = calories[best] / weights[best]
			jCalToWeight = calories[j] / weights[j]
			# print(f"whileLoop: \n\tbestCalToWeight = {bestCalToWeight}\n\tjCalToWeight = {jCalToWeight}")
			if jCalToWeight > bestCalToWeight:
				# print(f"\n\tNew Best = {j}")
				best = j

			j += 1

		# swap
		if best != i:
			swapItem(names, weights, calories, i, best)
		# print(f"END OF LOOP ITERATION: \n\ti = {i}\n\t{names}")

	print(f"Post-sorted Items: {names}")

	# list of indices
	greedyList = []
	print(f"maxWeight = {maxWeight}")
	for i in range(0, len(names)):
		print(f"\tsumWeight + weight[{i}]: {sumWeight+weights[i]}")
		if (sumWeight+weights[i]) > maxWeight:
			continue
		greedyList.append(i)
		sumWeight += weights[i]
		print(f"\tAdded {names[i]} to the greedy list.")

	print(f"Greedy Indices: {greedyList}")
	print(f"Greedy Weight: {sumWeight}")
	print("Completed Greedy Algo")

if __name__ == "__main__":
	case1Names = ["apple", "cereal", "water", "coke", "bread"]
	case1Weights = [15, 25, 90, 8, 8]
	case1Calories = [30, 500, 0, 160, 800]

	#case1Tests
	greedyAlgo(30, case1Names, case1Weights, case1Calories)
	greedyAlgo(800, case1Names, case1Weights, case1Calories)
	greedyAlgo(8, case1Names, case1Weights, case1Calories)
