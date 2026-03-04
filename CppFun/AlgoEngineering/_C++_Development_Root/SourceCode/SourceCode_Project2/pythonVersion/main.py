class Items:
	def __init__(self):
		pass
	names = []
	weights = []
	calories = []


def greedySort(maxWeight):
	case1Item["apple", "cereal", "water", "coke", "bread"]
	case1Weight[15, 25, 90, 8, 8]
	case1Calories[30, 500, 0, 160, 800]
	sumWeight:int = 0
	# selection sort
	# sort items by calories/weight
	for i in range (0, list(case1Item):
		best = i
		for j in range (i, list(case1Item):
			iCalWeight = case1Calories[i] / case1Weight[i]
			jCalWeight = case1Calories[j] / case1Weight[j]
			if jCalWeight > iCalWeight:
				best = j

		# swap
		if best != i:
			itemSwap = case1Item[j]
			weightSwap = case1Weight[j]
			caloriesSwap = case1CAlories[j]

			case1Item[j] = case1Item[i]
			case1Weight[j] = case1Weight[i]
			case1Calories[j] = case1Calories[i]

			case1Item[i] = itemSwap
			case1Weight[i] = weightSwap
			case1Calories[i] = caloriesSwap

	greedyList = []
	for i in range(0, list(case1Item)):
		if sumWeight+case1Calories[i] > maxWeight:
			continue
		greedyList



if __name__ == "main":
	greedySort(300)
