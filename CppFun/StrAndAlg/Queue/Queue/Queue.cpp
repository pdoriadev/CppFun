
#include <iostream>
#include "Queue.h"

int main()
{
    LLQueue llqueue;
    for (int i = 0; i < 6; i++)
    {
        llqueue.enqueue(i);
        std::cout << "Enqueued " << i << '\n';
    }
    std::cout << '\n';
    while (!llqueue.isEmpty())
    {
        std::cout << "Dequeued: " << llqueue.dequeue()->value << '\n';
    }


    ArrayQueue arrQueue; 
    for (int i = 0; i < 2; i++)
    {
        std::cout << "Enqueued " << arrQueue.enqueue(i)->value << '\n';
    }
    for (int i = 0; i < 1; i++)
    {
        std::cout << "Dequeued: " << arrQueue.dequeue() << '\n';
    }
    for (int i = -3; i < 0; i++)
    {
        std::cout << "Enqueued " << arrQueue.enqueue(i)->value << '\n';
    }
    while (!arrQueue.isEmpty())
    {
        std::cout << "Dequeued: " << arrQueue.dequeue() << '\n';
    }


}
