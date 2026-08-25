#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// Function to increment array elements
void increment_array_elements(std::vector<int>& array, int start, int end) {
    for (int i = start; i < end; ++i) {
        array[i] += 1;
    }
}

int main() {
    // Create an array with many elements
    std::vector<int> my_array(10000000, 1); // Example with 10 million elements, feel free to modify the number for testing.

    // Single-threaded approach
    auto start_time = std::chrono::high_resolution_clock::now();
    increment_array_elements(my_array, 0, my_array.size());
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration<double>(end_time - start_time).count();
    std::cout << "Total time with single thread: " << total_time << " seconds\n";

    // Reset the array for multi-threading
    std::fill(my_array.begin(), my_array.end(), 1);

    // Multi-threaded approach
    start_time = std::chrono::high_resolution_clock::now();
    int midpoint = my_array.size() / 2;

    std::thread thread1(increment_array_elements, std::ref(my_array), 0, midpoint);
    std::thread thread2(increment_array_elements, std::ref(my_array), midpoint, my_array.size());
	// thread1.join();
    // thread2.join();

    end_time = std::chrono::high_resolution_clock::now();
    total_time = std::chrono::duration<double>(end_time - start_time).count();
    std::cout << "Total time with multi-threading: " << total_time << " seconds\n";

	return 0;
}
