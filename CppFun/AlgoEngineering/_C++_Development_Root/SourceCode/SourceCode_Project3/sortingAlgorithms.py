smallCase1 = [0, 1, 2, 3, 4, 5, 6]
smallCase2 = [6, 3, 0, 1, 5, 4, 2]
smallCase3 = []
smallCase4 = [1]
smallCase5 = [0, 1]
smallCase6 = [1, 0]
smallCases = [smallCase1, smallCase2, smallCase3, smallCase4, smallCase5, smallCase6]

#In-place selection sort
# - sorts the list by dividing it into sorted and unsorted partitions. 
# - super-loop - tracks beginning of unsorted partition. 
# - nested-loop - finds index for value that belongs at beginning of unsorted partition.
# - After completion of nested loop: perform swap if necessary
# - Moves super-loop index value up. Repeat until index = maxIndex-1
def selectionSort(case):
    print("selectionSort")
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

def mergeSort(case):
    print("mergeSort")
    #print(f"pre-sort:  {case}")
    #print(f"post-sort: {case}")

def quickSort(case):
    print("quickSort")

if __name__ == "__main__":
    i = 1 
    for case in smallCases :
        print(f"\nCase {i}")
        selectionSort(case)
        mergeSort(case)
        quickSort(case)
        i+=1

