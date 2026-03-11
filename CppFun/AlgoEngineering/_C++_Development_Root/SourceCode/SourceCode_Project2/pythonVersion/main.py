class Item():
    # Instance initializer
    def __init__(self, name, weight, calories):
        print(f"Initializing item: \n\tName: {name}, \n\tWeight: {weight}, \n\tCalories: {calories}")
        self.name = name
        self.weight = weight
        self.calories = calories

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
        # print(f"\tsumWeight + weight[{i}]: {sumWeight+weights[i]}")
        if (sumWeight+weights[i]) > maxWeight:
            continue
        greedyList.append(i)
        sumWeight += weights[i]
        # print(f"\tAdded {names[i]} to the greedy list.")

    print(f"Greedy Indices: {greedyList}")
    print(f"Greedy Weight: {sumWeight}")
    print(f"Completed Greedy Algo\n")


# I need all valid sets
# get the item at the bit index. Bit maps index of an item.
    # 0's/1's binary indexing.
 # include any additional information?
def exaustiveOptimized(maxWeight, items):
    # Generate candidate bitmasks
    # Store all possible combos in a tree?
    # Proof for Candidate Generation by iterating through all possible integer values for an integer of n bits. 
    #   Let the list of grocery items L be a list of n elements long.
    #   Let each element correspond to a binary bit in an unsigned integer type X of n bits long.
 #      Let us represent the max value of X with MAX of this integer type equal MAX
    #   Each unique set of binary bits in X corresponds to a unique integer between 0 and MAX (inclusive).
    #   Therefore, every value that can be represented by X corresponds to a unique set of grocery items, including the empty set.
    #   By iterating through all values of X, we can iterate through all candidates for exhaustive search.
    #   Could do it this way OR do it with a MST?

# Implementation    
    #
    # Generation
    best = 0
    bestCalorieToWeight = 0
    for i in range(0, 2 ** len(items)):
        # Verification
        # bit shift binary bit. keep track of how many shifts. If bit value is a 1, then check the index of items based on the number of shifts. 

    # return array of items based on value of best. 
            

    # Verify candidates using bitmask operations
 # Bitshift verification per set
    # Let P be a set of all possible sets of list L.
    # Let each set be denoted by P(sub i).
    # Each set P(sub i) is a binary sequence of length n.
    # Each value at index 'place' in the binary sequence corresponds to an item in list L.
    # A value of '0' at index 'place' in P(sub i) means L's item at the corresponding index 'place' is not included in set P(sub i).
    # A value of '1' at index 'place' in P(sub i) means L's item at the corresponding index 'place' is included in the set P(sub i). 
    # best = {} # empty set       +1
    # bestCalorieToWeight = 0     +1
 # Loop - For each set GENERATED_CANDIDATE P(sub i) of P:  +(2^n * (g + (n*VerificationLoop)))
    #   totalWeight = 0    +1
    #   totalCalories = 0               +1
    #   VerificationLoop - Bitshift 'y' times until a 1 is encountered:    +1
    #       If element at 'y' place in list L does NOT have a valid weight    +1
    #           continue
    #       totalWeight += L[length() - y - 1].weight               +1
    #       totalCalories += L[length() - y - 1].calories                   +1
    #       If (totalCalories / totalWeight) > bestCalorieToWeight:     +1
    #           best = P(sub i)     +1
 # return best
    pass

def initializeItems(namesArr, weightsArr, caloriesArr):
    items = []
    for i in range(0, len(namesArr)):
        items.append(Item(namesArr[i], weightsArr[i], caloriesArr[i]))
    
    print("Completed item initialization")
    print("Item\tWeight\tCalories")
    for i in range(0, len(case1Names)):
        print(f"{items[i].name}\t{items[i].weight}\t{items[i].calories}")

    return items

if __name__ == "__main__":
    case1Names = ["apple", "cereal", "water", "coke", "bread"]
    case1Weights = [15, 25, 90, 8, 8]
    case1Calories = [30, 500, 0, 160, 800]

    #case1Tests - OLD APPROACH
    #greedyAlgo(30, case1Names, case1Weights, case1Calories)
    #greedyAlgo(800, case1Names, case1Weights, case1Calories)
    #greedyAlgo(8, case1Names, case1Weights, case1Calories)

    items = initializeItems(case1Names, case1Weights, case1Calories)
    print("Main has received items. Item 3 test: {item[2].name}")
    exhaustiveOptimized(10, items)    
