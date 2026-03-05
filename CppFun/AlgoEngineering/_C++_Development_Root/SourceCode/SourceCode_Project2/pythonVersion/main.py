class Items:
	def __init__(self):
		pass
	names = []
	weights = []
	calories = []


def greedyAlgo(maxWeight):
	case1Item = ["apple", "cereal", "water", "coke", "bread"]
	case1Weight = [15, 25, 90, 8, 8]
	case1Calories = [30, 500, 0, 160, 800]
	sumWeight:int = 0

	print(f"Pre-sorted Items: {case1Item}")

	# selection sort
	# sort items by calories/weight
	for i in range (0, len(case1Item) - 1):
		best = i
		j = i + 1
		while j < len(case1Item):
			bestCalToWeight = case1Calories[best] / case1Weight[best]
			jCalToWeight = case1Calories[j] / case1Weight[j]
			# print(f"whileLoop: \n\tbestCalToWeight = {bestCalToWeight}\n\tjCalToWeight = {jCalToWeight}")
			if jCalToWeight > bestCalToWeight:
				# print(f"\n\tNew Best = {j}")
				best = j

			j += 1

		# swap
		if best != i:
			itemSwap = case1Item[best]
			weightSwap = case1Weight[best]
			caloriesSwap = case1Calories[best]

			case1Item[best] = case1Item[i]
			case1Weight[best] = case1Weight[i]
			case1Calories[best] = case1Calories[i]

			case1Item[i] = itemSwap
			case1Weight[i] = weightSwap
			case1Calories[i] = caloriesSwap

		# print(f"END OF LOOP ITERATION: \n\ti = {i}\n\t{case1Item}")

	print(f"Post-sorted Items: {case1Item}")

	# list of indices
	greedyList = []
	print(f"maxWeight = {maxWeight}")
	for i in range(0, len(case1Item)):
		print(f"\tsumWeight + weight[{i}]: {sumWeight+case1Weight[i]}")
		if (sumWeight+case1Weight[i]) > maxWeight:
			continue
		greedyList.append(i)
		sumWeight += case1Weight[i]
		print(f"\tAdded {case1Item[i]} to the greedy list.")

	print(f"Greedy Indices: {greedyList}")
	print(f"Greedy Weight: {sumWeight}")
	print("Completed Greedy Algo")

if __name__ == "__main__":
	greedyAlgo(100)
	greedyAlgo(800)
	greedyAlgo(8)
