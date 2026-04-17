import array

smallCase1 = ["a", "b", "c", "d", "e", "f"]
smallCase2 = ["f", "d", "ab", "b", "aa", "e", "c", "a"]
smallCase3 = []
smallCase4 = ["a"]
smallCase5 = ["a", "b"]
smallCase6 = ["b", "a"]
smallCases = [smallCase1, smallCase2, smallCase3, smallCase4, smallCase5, smallCase6]

#In-place selection sort
# - sorts the list by dividing it into sorted and unsorted partitions. 
# - super-loop - tracks beginning of unsorted partition. 
# - nested-loop - finds index for value that belongs at beginning of unsorted partition.
# - After completion of nested loop: perform swap if necessary
# - Moves super-loop index value up. Repeat until index = maxIndex-1
def selectionSort(constCase):
    print("SELECTION_SORT")
    case = constCase.copy()
    print(f"pre-sort:  {case}")

    if (len(case) <= 1):
        return

    for i in range(0, len(case) - 1):
        best = i 
        for j in range(i+1, len(case)):
            if case[j] < case[best]:
                best = j
        if best != i:
            swap = case[i]
            case[i] = case[best]
            case[best] = swap 
    
    print(f"post-sort: {case}")

# Out-of-place merge sort
#   Sort each partition, then merge it with another partition. Since each 
#   partition is sorted, merging them comes down to checking the 
#   extreme-indices' values of each partition, then 
#   swapping them as necessary. Division is in-place. Sort is out-of-place.
# Partition function - takes in a partition. Divides the partition into right
#   and left sides. Recursively calls partition for each new partition.
#   Calls merge after the partition calls. Merges the partitions. 
# Merge function - checks the which partition's greatest value is less than the other. 
#
# NOTE: In-place for array vs linkedlist approach
# Array - pass beginning and end indices of partition (pay attention for when partition size = 1 or 2. Can run into edge-cases if not checking indices before comprison)
# LinkedList - If doubly linked, this isn't so bad. We pass the nodes at the beginning and end of the partition. If singly linked, pass the starting node and the number of nodes to traverse forward. 
def mergeSortVector(constCase):
    print("MERGE_SORT") 
    print(f"pre-sort:  {constCase}")
    #if (len(constCase) <= 1):
    #    return constCase

    sortedCase = partitionVector(constCase.copy(), 0, len(constCase) - 1) 
     
    print(f"post-sort: {sortedCase}")

# recursively calls itself. Divides the passed-in list into a left and right
#   partitionVector by recursively calling the partition function. 
#   Then merges the two partitionVectors by calling the merge function.
# Base case: partitionVector size is less than or equal to 2.
def partitionVector(case, i, m):
    #print(f"i={i}, m={m}") 
    if (m - i < 1):
        return case[i:m+1] 
    
    if (m - i == 1):
        if (case[i] > case[m]):
            swap = case[i]
            case[i] = case[m]
            case[m] = swap
            #print(f"Post-swap: {case[i:m+1]}")
        return case[i:m+1]

    sortedLeft = partitionVector(case, i, i + int((m-i)/2))
    sortedRight = partitionVector(case, i + int(((m-i)/2))+1, m)
    
    sortedLeftAndRight = sortedLeft + sortedRight
    #sortedLeftAndRight.append(sortedRight)
    mergedList = mergeVector(sortedLeftAndRight, 0, int((m-i)/2), int((m-i)/2)+1, m-i) 
    return mergedList 

def mergeVector(unMerged, i, m, j, n):
    mergedList = []
    #print(f"Partitions Unsorted: {unMerged}")
    while (i <= m and j <= n):
        if (unMerged[i] < unMerged[j]):
            mergedList.append(unMerged[i])
            i+=1
        else:
            mergedList.append(unMerged[j])
            j+=1
    while (i <= m):
        mergedList.append(unMerged[i])
        i+=1
    while (j <= n):
        mergedList.append(unMerged[j])
        j+=1
    #print(f"Partitions Sorted:  {mergedList}")
    return mergedList

def quickSort(constCase):
    print("quickSort")

if __name__ == "__main__":
    i = 1
    for case in smallCases :
        print(f"\nCase {i}")
        selectionSort(case)
        mergeSortVector(case)
        quickSort(case)
        i+=1
