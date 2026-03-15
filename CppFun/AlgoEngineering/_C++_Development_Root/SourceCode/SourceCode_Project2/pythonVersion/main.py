import numpy as np

class Item():
    # Instance initializer
    def __init__(self, name, weight, calories):
        print(f"Initializing item: \n\tName: {name}, \n\tWeight: {weight}, \n\tCalories: {calories}")
        self.name = name
        self.weight = weight
        self.calories = calories

def swapItems(items, i, j):
    itemSwap:int = items[j]
    items[j] = items[i]
    items[i] = items[j]

def greedyAlgo(maxWeight, items):
    sumWeight:int = 0

    print(f"Pre-sorted Items: ")
    outputItemsData(items)

    # selection sort
    # sort items by calories/weight
    for i in range (0, len(items) - 1):
        best = i
        j = i + 1
        while j < len(items):
            bestCalToWeight = items[best].calories / items[best].weight
            jCalToWeight = items[j].calories / items[j].weight
            # print(f"whileLoop: \n\tbestCalToWeight = {bestCalToWeight}\n\tjCalToWeight = {jCalToWeight}")
            if jCalToWeight > bestCalToWeight:
                # print(f"\n\tNew Best = {j}")
                best = j

            j += 1

        # swap
        if best != i:
            swapItems(items, i, best)
        # print(f"END OF LOOP ITERATION: \n\ti = {i}\n\t{names}")

    print(f"Post-sorted Items: ")
    outputItemsData(items)

    # list of indices
    greedyList = []
    print(f"maxWeight = {maxWeight}")
    for i in range(0, len(items)):
        # print(f"\tsumWeight + weight[{i}]: {sumWeight+weights[i]}")
        if (sumWeight + items[i].weight) > maxWeight:
            continue
        greedyList.append(i)
        sumWeight += items[i].weight
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
    bestWeight = 0
    i = np.uint64(0)
    print(f"MaxBinary: {bin( 2^len(items) - 1 )}")
    #print(f"i in binary: {bin(i)}")
    for i in range(0, 2 ** len(items)):
        calories = 0
        weight = 0
        bestCandidate = True
                
        maxPlaces:int = len(items)
        if (i.bit_length() < len(items)):
            maxPlaces = i.bit_length() + 1
        
        #print(f"i: {i} {bin(i)}")
        places = np.uint64(0) 
        for places in range(0, maxPlaces): 
            #print(f"\tplaces: {j}")
            k = i >> places
            #print(f"\ti shifted places times % 2: {k%2}")
            if (((i >> places) % 2) == 0):
                #print("\tNo 1 bit. Skip to next loop")
                continue
          
            #print(f"\tAdding Weight. i shifted {places} times: {bin(k)}")
            weight += items[places].weight    
            calories += items[places].calories
            if (weight > maxWeight):
                #print(f"\tInbestCandidate Weight: {weight}")
                bestCandidate = False
                break

        #print(f"\tTotal Weight of Set = {weight}")
        
        if (calories < bestCalories):
            bestCandidate = False
        
        if (calories == bestCalories):
            if (bestWeight == 0 or calories/weight < bestCalories/bestWeight):
                bestCandidate = False

        #print(f"\t\'i\' after loop: {i}")
        if (bestCandidate == False):
            continue
        
        best = i 
        bestCalories = calories
        bestWeight = weight
        #print(f"\tNew Best")
        #print(f"\t\tCalories: {calories}")
        #print(f"\t\tBinary: {bin(i)}")

    bestSet = [] 
    for i in range(0, len(items)):
        if (((best >> i) % 2) == 1):
            #print(f"Appending {items[i].name} to bestSet.")
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
    print("Item\t\tWeight\t\tCalories")
    for i in range(0, len(items)):
        print(f"{items[i].name}\t\t{items[i].weight}\t\t{items[i].calories}")

def testGreedy(maxWeight, items):
    pass

def testExhaustive(maxWeight, items, span):
    print("---------------------------------\n--- Exhaustive Optimized Tests ---")
    for i in range(0, span):
        print(f"\n------- {i+1} ITEM(S) RUN ------- ") 
        print("Pre-Exhaustive.... SET OF ALL ITEMS") 
        outputItemsData(items[0:i+1])
        best = exhaustiveOptimized(maxWeight, items[0:i+1])
        print("Post-Exhaustive... BEST SET")
        outputItemsData(best)

if __name__ == "__main__":
    case1Names = ["apple", "cereal", "water", "coke", "bread", "bananaBunch", "coughSyrup"]
    case1Weights = [15, 25, 90, 8, 8, 55, 10]
    case1Calories = [30, 500, 0, 160, 800, 400, 90]

    #case1Tests - OLD APPROACH
    #greedyAlgo(30, case1Names, case1Weights, case1Calories)
    #greedyAlgo(800, case1Names, case1Weights, case1Calories)
    #greedyAlgo(8, case1Names, case1Weights, case1Calories)
    greedyItems1 = initializeItems(case1Names, case1Weights, case1Calories)
    greedyAlgo(30, greedyItems1)
    greedyAlgo(800, greedyItems1)
    
    items = initializeItems(case1Names, case1Weights, case1Calories)
    #print(f"Main has received items. Item 3 test: {items[2].name}")

    testExhaustive(100, items, len(items))
