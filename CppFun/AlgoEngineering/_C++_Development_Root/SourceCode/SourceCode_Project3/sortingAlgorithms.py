import array

smallCase1 = ["a", "b", "c", "d", "e", "f"]
smallCase2 = ["f", "d", "ab", "b", "aa", "e", "c", "a"]
smallCase3 = []
smallCase4 = ["a"]
smallCase5 = ["a", "b"]
smallCase6 = ["b", "a"]
smallCase7 = ["axe", "bear", "washer", "z", "y"]
smallCases = [smallCase1, smallCase2, smallCase3, smallCase4, smallCase5, smallCase6, smallCase7]

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

    mid = i + int((m-i)/2)
    sortedLeft = partitionVector(case, i, mid)
    sortedRight = partitionVector(case, mid+1, m)
    
    #sortedLeftAndRight.append(sortedRight)
    mergedVectorOut = [] 
    mergeVector(sortedLeft, sortedRight, mergedVectorOut)
    return mergedVectorOut 

def mergeVector(left, right, mergedVectorOut):
    i = 0
    j = 0
    #print(f"Partitions Unsorted: {unMerged}")
    while (i < len(left) and j < len(right)):
        if (left[i] < right[j]):
            mergedVectorOut.append(left[i])
            i+=1
        else:
            mergedVectorOut.append(right[j])
            j+=1
    while (i < len(left)):
        mergedVectorOut.append(left[i])
        i+=1
    while (j < len(right)):
        mergedVectorOut.append(right[j])
        j+=1
    #print(f"Partitions Sorted:  {mergedVectorOut}")


##################
class Node:
    def __init__(self, prevNode, nextNode, value):
        self.prevNode = prevNode
        self.nextNode = nextNode
        self.value = value

########################
class LinkedList:
    count = 0
    head = None 
    tail = None 
    
    def __init__(self, head, tail):
        if (head is not None):
            insertNodeAfter(None, head)
            self.count+=1
        if (tail is not None):
            insertNodeAfter(self.tail, tail)
            self.count+=1
    
    def newList(self, head, tail, count):
        self.head = head
        self.tail = tail
        self.count = count


    # prevNode = None to make a new head.
    # For any other node, insert the node you want to insert after 
    def insertNodeAfter(self, prevNode, node):
        if self.head == None: # count = 0
            self.head = node
            self.tail = node
        elif self.count == 1:
            self.tail = node
            self.tail.prevNode = self.head
            self.head.nextNode = self.tail
        elif prevNode == None: # insert new head
            node.prevNode = None
            self.head.prevNode = node
            node.nextNode = self.head
            self.head = node
        elif prevNode == self.tail: # insert new tail
            prevNode.nextNode = node
            node.prevNode = prevNode
            node.nextNode = None
            self.tail = node
        else: # insert new node in middle of list
            node.nextNode = prevNode.nextNode
            node.prevNode = prevNode
            node.nextNode.prevNode = node
            prevNode.nextNode = node

        self.count+=1

    def removeNode(self, node):
        node.prevNode.nextNode = node.nextNode
        node.nextNode.prevNode = node.prevNode
        node.prevNode = None
        node.nextNode = None
        self.count-=1
        return node

    def moveNodeAfter(self, prevNode, node):
        self.removeNode(node)
        self.insertNodeAfter(prevNode, node)

    def swapNodes(self, nodeA, nodeB):
        prevA = nodeA.prevNode
        isAGood = nodeA is not None
        isBGood = nodeB is not None
        if (isAGood && isBGood):
            self.moveNodeAfter(nodeB.prevNode, nodeA)
            self.moveNodeAfter(prevA, nodeB)
            return
        elif (isAGood):
            self.moveNodeAfter(nodeB, nodeA)
            return
        elif (isBGood):
            self.moveNodeAfter(nodeA, nodeB)
            return
        
        print("Both swap nodes are None")

def printHeadToTail(linkedList):
    print("Print List: Head to Tail")
    print(f"Count = {linkedList.count}")
    node = linkedList.head
    while node is not None:
        print(node.value, sep=' ', end=None)
        node = node.nextNode
    print()

def printTailToHead(linkedList):
    print("Print List: Tail to Head")
    print(f"Count = {linkedList.count}")
    node = linkedList.tail
    while node is not None:
        print(node.value, sep=' ', end=None)
        node = node.prevNode
    print()

##################
def mergeSortLinkedList(constCase):
    linkedList = LinkedList(None, None)  
    # Construct the linked list
    prev = None
    for i in range(0, len(constCase)):
        node = Node(None, None, constCase[i])
        linkedList.insertNodeAfter(prev, node)
        prev = node
    printHeadToTail(linkedList)
    printTailToHead(linkedList)
    
    partitionLinkedList(linkedList, linkedList.head, linkedList.tail, linkedList.count)
    printHeadToTail(linkedList)

def partitionLinkedList(linkedList, start, end, partSize):
    # base cases
        # n < 2 
        # n == 2
    if (partSize < 2):
        return 
    if (partSize == 2):
        if (start.value > end.value):
            linkedList.swapNodes(start, end)
        return 
    
    # split into left
    leftEnd = start
    for i in range(0, int(partSize / 2)):
        leftEnd = leftEnd.nextNode
    partitionLinkedList(linkedList, start, leftEnd, int((partSize / 2)) + 1) 
    # split into right
    partitionLinkedList(linkedList, leftEnd, end, int(partSize / 2)) 
    # merge left and right. In-place?
    mergeLinkedList(linkedList, start, leftEnd.nextNode, end)

def mergeLinkedList(linkedList, leftStart, rightStart, rightEnd, count):
    left = leftStart
    right = rightStart
    while (left is not right and right is not None):
        if (right.value < left.value):
            node = right.nextNode 
            linkedList.swapNodes(left.prevNode, right)
            right = node 
        else:
            left = left.nextNode

    insertionPoint = head
    groupStart = rightStart
    groupEnd = groupStart 
    while (insertionPoint is not rightStart):
        
        if (insertionPoint.value < groupEnd):
            if (groupEnd.nextNode is None):
                # insert group
                break
            if (groupEnd.nextnode >= insertionPoint.value):
                #insert group
                insertionPoint = insertionPoint.nextNode
                continue
            # groupEnd.nextNode < insertionPoint.value
            groupEnd = groupEnd.nextNode
    for i in range(0, count):
        if (groupStart.value  
    # do something like selection sort but in one go?
    while (left is not rightStart and right is not rightEnd):
        if (left.value > right.value):
            nextLeft = left.nextNode
            nextRight = right.nextNode
            linkedList.swapNodes(left, right)
        left = left.nextNode
        right = right.nextNode
    # Don't need to do any more adding/swapping because we've swapped in-place?
# QUICK-SORT
def quickSort(constCase):

    print("quickSort")

if __name__ == "__main__":
    i = 1
    for case in smallCases :
        print(f"\nCase {i}")
        selectionSort(case)
        mergeSortVector(case)
        mergeSortLinkedList(case) 
        quickSort(case)
        i+=1
