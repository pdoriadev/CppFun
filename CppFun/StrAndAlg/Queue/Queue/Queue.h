#pragma once
// Implemented Queue Data Structure
/*
*   Linked List version and Circular Array version
*
    1. Peek at top of queue
    2. Look at back of queue
    3. Enqueue
    4. Dequeue
    5. Empty
*/
#include <vector>
#include <assert.h>
struct QueueItem
{
    int value = 0;
    QueueItem()
    {
        value = 0;
    }
    QueueItem(int _value)
    {
        value = _value;
    }
};

class ArrayQueue
{
private:
    int initialCapacity = 3;
    std::vector<QueueItem*> arr;
    int frontIndex = 0;
    int backIndex = 0;
    int itemsInQueue =  0;

    void resizeQueue(bool newSizeIsBigger)
    {
        if (arr.capacity() == 0)
        {
            if (newSizeIsBigger)
            {
                arr.resize(1);
            }

            return;
        }

        if (newSizeIsBigger)
        {
            int oldCap = arr.capacity();
            arr.resize(arr.capacity() * 2);
            if (frontIndex > backIndex)
            {
                int i = 0;
                while (arr[frontIndex + i] != NULL)
                {
                    arr[frontIndex + i + oldCap] = arr[frontIndex + i];
                    arr[frontIndex + i] = NULL;
                }

                frontIndex += oldCap;
            }

            return;
        }
        else if (arr.capacity() / 2 < arr.size())
        {
            arr.resize(arr.size());
        }
        else 
            arr.resize(arr.capacity() / 2);


    }

public:

    QueueItem * front() { return arr[frontIndex]; }
    QueueItem * back() { return arr[backIndex]; }
    int isEmpty() { return itemsInQueue == 0; }

    QueueItem * enqueue(int value) 
    {
        itemsInQueue++;
        if (itemsInQueue > arr.size())
        {
            resizeQueue(true);
        }

        if (arr[frontIndex] == NULL)
        {
            arr[frontIndex] = new QueueItem(value);
            return arr[frontIndex];
        }

        if (backIndex + 1 >= arr.size())
        {
            backIndex = 0;
        }
        else backIndex++;
        
        arr[backIndex] = new QueueItem(value);
        return arr[backIndex];
    }
    
    int dequeue()
    {
        assert(("Attempting to dequeue null value", arr[frontIndex] != NULL));
        itemsInQueue--;
        int dequeuedValue = arr[frontIndex]->value;
        delete arr[frontIndex];
        arr[frontIndex] = NULL;
        frontIndex++;

        if (frontIndex >= arr.size() || arr[frontIndex] == NULL)
        { 
            frontIndex = 0; 
        }
        
        if (itemsInQueue > 0)
        {
            while (arr[frontIndex] == NULL && frontIndex < arr.size())
            {
                frontIndex++;
            }
        }

        return dequeuedValue;
    }

};


struct LLNode
{
    int value;
    LLNode * next;
    LLNode(LLNode * _next, int _value)
    {
        next = _next;
        value = _value;
    }
};

class LLQueue
{
private:
    LLNode * head = NULL;
    LLNode* tail = NULL;
public:
    LLNode* front() { return head; }
    LLNode* back() { return tail; }
    bool isEmpty() { return head == nullptr; }

    LLNode * enqueue(int value)
    {
        if (head == nullptr)
        {
            head = new LLNode(nullptr, value);
            tail = head;
            return head;
        }
        
        tail->next = new LLNode(nullptr, value);
        tail = tail->next;
        return tail;
    }
    LLNode * dequeue()
    {
        if (isEmpty())
        {
            return nullptr;
        }
        
        LLNode * dequeuedNode = head;
        head = head->next;
        return dequeuedNode;
    }
};
