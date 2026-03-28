#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include <mutex>
    // mutex, locking and unlocking.
#include <sys/mman.h>
    // mmap() function: https://www.man7.org/linux/man-pages/man2/mmap.2.html
std::mutex mtx;

class BankAccount {
private:
    int balance;
public:
    BankAccount(int initial_balance) : balance(initial_balance) {}

    void deposit(int amount) {
        mtx.lock(); 
        balance += amount;
        mtx.unlock();
    }

    void withdraw(int amount) {
        mtx.lock();
        balance -= amount;
        mtx.unlock();
    }

    int get_balance() const {
        return balance;
    }
};

void user_transactions(BankAccount& account, int thread_id, int num_transactions, 
        std::vector<std::string>& log) {
    
    // Random number generator for ints across uniform distribution. Used as the seed source for the unsigned int generator.
    std::random_device rd;
    // 32-bit Unsigned int generator using the std::mersenne_twister_engine: https://cplusplus.com/reference/random/mersenne_twister_engine/
        // Alters internal state by "twisting" with an xor mask on a mis of bits. Neat!
    std::mt19937 gen(rd());
    // Produces a uniform distribution of ints constrained to a range.
    std::uniform_int_distribution<> amount_dist(1, 100);
    std::uniform_int_distribution<> action_dist(0, 1);

    // Open file for this thread
    std::ofstream logfile("log_thread_" + std::to_string(thread_id) + ".txt");

    for (int i = 0; i < num_transactions; i++) {
        // Get next random amount and action (deposit or withdraw). 
            // Passes a generator reference to the int distribution object.
        int amount = amount_dist(gen);
        int action = action_dist(gen);

        if (action == 0) {
            account.deposit(amount);
            std::string entry = "Thread " + std::to_string(thread_id) + 
                               ": Deposit $" + std::to_string(amount);
            log.push_back(entry);
            logfile << entry << std::endl;  // Write to file
        } else {
            account.withdraw(amount);
            std::string entry = "Thread " + std::to_string(thread_id) + 
                               ": Withdraw $" + std::to_string(amount);
            log.push_back(entry);
            logfile << entry << std::endl;  // Write to file
        }
    }

    logfile.close();
}

int main() {
    const int STARTING_BALANCE = 1000;
    const int NUM_THREADS = 4;
    const int TRANSACTIONS_PER_THREAD = 100;
    const int NUM_RUNS = 10;

    for (int run = 0; run < NUM_RUNS; run++) {
        std::cout << "\n===== RUN #" << (run + 1) << " =====" << std::endl;

        // https://www.man7.org/linux/man-pages/man2/mmap.2.html
        BankAccount account = (*(BankAccount*) mmap(
                        NULL,       // let system choose address
                        sizeof(BankAccount),
                        PROT_READ | PROT_WRITE, // can read and write to file.
                        MAP_SHARED | MAP_ANONYMOUS, // shared between processess. Not backed by any file.
                        -1, // file descriptor arg. "-1" is required for MAP_ANONYMOUS portability.
                        0)); // offset argument - No offset since we the mapping is not backed up by a file we can offset.
        
        // Initialize value
        account = BankAccount(STARTING_BALANCE);

        std::vector<std::vector<std::string>> thread_logs(NUM_THREADS);
        std::vector<std::thread> threads;

        for (int i = 0; i < NUM_THREADS; i++) {
            threads.push_back(std::thread(user_transactions, 
                                         std::ref(account), 
                                         i + 1, 
                                         TRANSACTIONS_PER_THREAD, 
                                         std::ref(thread_logs[i])));
        }

        int expected_change = 0;
for (auto& t : threads) {
            t.join();
        }

        // Calculate expected balance from logs
        for (const auto& log : thread_logs) {
            for (const auto& entry : log) {
                size_t dollar_pos = entry.find('$');
                std::string amount_str = entry.substr(dollar_pos + 1);
                int amount = std::stoi(amount_str);
                
                if (entry.find("Deposit") != std::string::npos) {
                    expected_change += amount;
                } else {
                    expected_change -= amount;
                }
            }
        }

        int expected_balance = STARTING_BALANCE + expected_change;
        int actual_balance = account.get_balance();

        std::cout << "Starting Balance: $" << STARTING_BALANCE << std::endl;
        std::cout << "Expected Balance: $" << expected_balance << std::endl;
        std::cout << "Actual Balance:   $" << actual_balance << std::endl;
        
        if (actual_balance != expected_balance) {
            std::cout << "⚠️  RACE CONDITION DETECTED! Difference: $" 
                     << (expected_balance - actual_balance) << std::endl;
        } else {
            std::cout << "✓ No race condition (this time)" << std::endl;
        }

        // Print merged transaction logs
        std::cout << "\n--- Merged Transaction Logs ---" << std::endl;
        for (int i = 0; i < NUM_THREADS; i++) {
            std::cout << "Thread " << (i + 1) << " completed " 
                     << thread_logs[i].size() << " transactions" << std::endl;
        }
        std::cout << "Total transactions: " 
                 << (NUM_THREADS * TRANSACTIONS_PER_THREAD) << std::endl;
    }

    return 0;
}
