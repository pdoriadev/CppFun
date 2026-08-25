#include <iostream>
#include <thread>
#include <mutex>

const int LIMIT =10000;
int x =1;
std::mutex mtx;

void increment() {
    for (int i = 0; i < LIMIT; ++i) {
	mtx.lock();
        x++;
        std::cout << "increment x:" << x << std::endl;
	mtx.unlock();
       }
}

void decrement() {
    for (int i = 0; i < LIMIT; ++i) {
      mtx.lock();
      x--;
      std::cout << "decrement x:" << x << std::endl;
      mtx.unlock(); 
    }
}
int main() {
        
    // Create threads
    std::thread thread1(increment);
    std::thread thread2(decrement);

    // Join the threads with the main thread
    thread1.join();
    thread2.join();
     
     std::cout << "Modified x:" << x;
   

    return 0;
}

