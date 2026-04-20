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

def printHeadToTail(linkedList):
    print("Print List: Head to Tail")
    print(f"Count = {linkedList.count}")
    node = linkedList.head
    while node is not None:
        print(f"{node.value}", sep=None, end=None)
        node = node.nextNode
    print()

def printTailToHead(linkedList):
    print("Print List: Tail to Head")
    print(f"Count = {linkedList.count}")
    node = linkedList.tail 
    #copyNode(node, linkedList.tail)
    while node is not None:
        print(f"{node.value}", sep=' ', end=None)
        node = node.prevNode
    print()

def printListRange(linkedList, start, end):
    print("Print List Range")
    node = start 
    while node is not end.nextNode: 
        print(f"{node.value}", sep=' ', end=None)
        node = node.nextNode

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
            self.tail.nextNode = None
            self.head.nextNode = self.tail
            print(f"Inserting new tail {node.value} after {node.prevNode.value}")
        elif prevNode == None: # insert new head
            print(f"Old head value: {self.head.value}")
            node.prevNode = None
            node.nextNode = self.head
            self.head.prevNode = node
            self.head = node
            print(f"Inserting new head {node.value} before {node.nextNode.value}")
        elif prevNode == self.tail: # insert new tail
            prevNode.nextNode = node
            node.prevNode = prevNode
            node.nextNode = None
            self.tail = node
            print(f"Inserting new tail {node.value} after {node.prevNode.value}")
        else: # insert new node in middle of list
            if (node is not prevNode.nextNode):
                node.nextNode = prevNode.nextNode
                node.prevNode = prevNode
                prevNode.nextNode = node
                node.nextNode.prevNode = node
            else:
                prevNode.nextNode = node.nextNode
            node.prevNode = prevNode
            if(node.nextNode is not None):
                node.nextNode.prevNode = node
            prevNode.nextNode = node
            print(f"Inserting new node {node.value} after {node.prevNode.value}")

        self.count+=1

    def removeNode(self, node):
        if (node.prevNode is not None):
            node.prevNode.nextNode = node.nextNode
        if (node.nextNode is not None):
            node.nextNode.prevNode = node.prevNode
        node.prevNode = None
        node.nextNode = None
        self.count-=1
        return node

    def moveNodeAfter(self, prevNode, node):
        self.removeNode(node)
        self.insertNodeAfter(prevNode, node)

    def swapNodes(self, nodeA, nodeB):
        isAGood = nodeA is not None
        isBGood = nodeB is not None
        if (isAGood and isBGood):
            prevA = nodeA.prevNode
            self.moveNodeAfter(nodeB.prevNode, nodeA)
            self.moveNodeAfter(prevA, nodeB)
            return
        elif (isAGood):
            self.moveNodeAfter(None, nodeA)
            return
        elif (isBGood):
            self.moveNodeAfter(None, nodeB)
            return
        
        print("Both swap nodes are None")

def copyNode(copierNode, node):
    copierNode = Node(node.prevNode, node.nextNode, node.value)

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
    #printListRange(linkedList, linkedList.head, linkedList.tail) 
    partitionLinkedList(linkedList, linkedList.head, linkedList.tail, linkedList.count)
    printHeadToTail(linkedList)

def partitionLinkedList(linkedList, start, end, partSize):
    print(f"{partSize}")
    #printListRange(linkedList, start, end)
    # base cases
        # n < 2 
        # n == 2
    if (partSize < 2):
        return 
    if (partSize == 2):
        if (start.value > end.value):
            print(f"start: {start.value}, end: {end.value}") 
            linkedList.swapNodes(start, end)
        return 
    
    leftEnd = None 
    halfSize = int(partSize / 2)
    for i in range(0, (halfSize + partSize % halfSize)):
        if (leftEnd is None):
            leftEnd = start
        else:
            leftEnd = leftEnd.nextNode
        print(leftEnd)
        print(leftEnd.value)
        print(f"i in range: {i}")
    partitionLinkedList(linkedList, start, leftEnd, halfSize + partSize % halfSize) 
    # split into right
    partitionLinkedList(linkedList, leftEnd.nextNode, end, halfSize) 
    # merge left and right. In-place?
    mergeLinkedList(linkedList, start, leftEnd.nextNode, end)

def mergeLinkedList(linkedList, leftStart, rightStart, rightEnd):
    left = leftStart
    right = rightStart
    rightIsGood = right is not None and right is not rightEnd.nextNode
    print("MERGING LISTS")
    printListRange(linkedList, leftStart, rightEnd) 
    while (left is not rightStart and rightIsGood and left is not right): 
        print(f"Right Val: {right.value}, Left Val: {left.value}")
        # is something weird happening to 'right'?
        if (right.value < left.value):
            node = right.nextNode 
            print(right.nextNode.value)
            linkedList.insertNodeAfter(left.prevNode, right)
            right = node 
            print(right.nextNode.value)
            rightIsGood = right is not None and right is not rightEnd.nextNode
        else:
            left = left.nextNode
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
