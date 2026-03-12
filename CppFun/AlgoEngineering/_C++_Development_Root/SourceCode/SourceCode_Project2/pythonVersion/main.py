import numpy as np

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


# Generate candidate bitmasks
    # Proof for Candidate Generation by iterating through all possible integer values for an integer of n bits. 
        # Let the list of grocery items L be a list of n elements long.
        # Let each element correspond to a binary bit in an unsigned integer type X of n bits long.
        # Let us represent the max value of X with MAX of this integer type equal MAX
        # Each unique set of binary bits in X corresponds to a unique integer between 0 and MAX (inclusive).
        # Therefore, every value that can be represented by X corresponds to a unique set of grocery items, including the empty set.
        # By iterating through all values of X, we can iterate through all candidates for exhaustive search.
# Generation Loop    
   # Bitshift verification per set
    # Let P be a set of all possible sets of list L's elements.
    # Let each set be denoted by P(sub i).
    # Each set P(sub i) is a binary sequence of length n.
    # Each value at index 'place' in the binary sequence corresponds to an item in list L.
        # A value of '0' at index 'place' in P(sub i) means P(sub i) does not contain the item at L's 'place' index.
        # A value of '1' at index 'place' in P(sub i) means P(sub i) does not contain the item at L's 'place' index.
    # best = {} # empty set       +1
    # bestCalorieToWeight = 0     +1 
# GenerationLoop - For each set GENERATED_CANDIDATE P(sub i) of P:  +(2^n * (g + (n*VerificationLoop)))
    #   totalCalories = 0               +1
    #   VerificationLoop - Bitshift 'y' times until a 1 is encountered:    +1
    #       If element at 'y' place in list L does NOT have a valid weight    +1
    #           continue
    #       totalWeight += L[length() - y - 1].weight               +1
    #       totalCalories += L[length() - y - 1].calories                   +1
    #       If (totalCalories / totalWeight) > bestCalorieToWeight:     +1
    #           best = P(sub i)     +1
def exhaustiveOptimized(maxWeight, items):
    print(f"maxWeight: {maxWeight}")
    best = 0
    bestCalories = 0
    i = np.uint64(0)
    for i in range(0, 2 ** len(items)):
        j = np.uint64(0) 
        calories = 0
        weight = 0
        valid = True
        for j in range(0, len(items)): 
            print(f"i: {i}")
            print(f"j: {j}")

            if ((i << j & 0)):
               continue

            weight += items[j].weight    
            calories += items[j].calories
            if (weight > maxWeight):
                print(f"Invalid Weight: {weight}")
                valid = False
                break

        if (valid == False):
            continue

        if (calories > bestCalories):
            best = j
            bestCalories = calories

    bestSet = [] 
    for i in range(0, len(items)):
        if (best << i & 1):
            print(f"Appending {items[i]} to bestSet.")
            bestSet.append(items[i])

    return bestSet
            

# return best
    pass
# bit shift binary bit. keep track of how many shifts. If bit value is a 1, then check the index of items based on the number of shifts. 


def initializeItems(namesArr, weightsArr, caloriesArr): 
    items = []
    for i in range(0, len(namesArr)):
        items.append(Item(namesArr[i], weightsArr[i], caloriesArr[i]))
    
    print("Completed item initialization")
    outputItemsData(items)

    return items

def outputItemsData(items):
    print("Item\tWeight\tCalories")
    for i in range(0, len(items)):
        print(f"{items[i].name}\t{items[i].weight}\t{items[i].calories}")



if __name__ == "__main__":
    case1Names = ["apple", "cereal", "water", "coke", "bread"]
    case1Weights = [15, 25, 90, 8, 8]
    case1Calories = [30, 500, 0, 160, 800]

    #case1Tests - OLD APPROACH
    #greedyAlgo(30, case1Names, case1Weights, case1Calories)
    #greedyAlgo(800, case1Names, case1Weights, case1Calories)
    #greedyAlgo(8, case1Names, case1Weights, case1Calories)

    items = initializeItems(case1Names, case1Weights, case1Calories)
    print(f"Main has received items. Item 3 test: {items[2].name}")
    best = exhaustiveOptimized(1000, items) 
    print(f"\nexhaustiveOptimized returned array...")
    outputItemsData(best)
