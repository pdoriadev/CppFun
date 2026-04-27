#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives
#include <vector> 
  // std:: vector
#include <string> 
  // std::string
#include <random> 
  // random uniform distribution for quicksort
#include <algorithm> 
  // std::swap() on same vector
/////////////////////// END-TO-DO (1) ////////////////////////////












// Algorithm Abstract Base Class and Interface
template<class Collection>
struct Algorithm
{
  using Collection_Type = Collection;
  virtual std::string description() const    { return typeid( *this ).name(); }

  // Input:
  //	A collection C of n≥2 sanitized words w consisting of non-whitespace characters c
  // Output:
  //	C modified such that w(i) ≤ w(i+1) for i in 0..n-2
  virtual void run( Collection_Type & ) const = 0;

  virtual ~Algorithm() = default;
};  // struct Algorithm












// In-Place Selection Sort
struct Algorithm_1 : Algorithm<std::vector<std::string>>              // Collection_Type binds to std::vector<std::string>
{
  std::string description() const override { return "Algorithm 1:  In-Place Selection Sort"; }



  void run( Collection_Type & words ) const override
  {
    //                i          min
    //  +---+---+---+---+---+---+---+---+---+---+
    //  | A | B | C | I | E | H | D | J | F | G |
    //  +---+---+---+---+---+---+---+---+---+---+
    //  |<--sorted->|<-------unsorted---------->|
    //
    //  Find the smallest element in the unsorted portion of the vector (i .. size-1) and swap
    //  it with the first element (i) of the unsorted portion.  Increment i and repeat.
    ///////////////////////// TO-DO (2) //////////////////////////////
    if (words.size() <= 1)
    {
      return;
    }

    for (std::size_t i = 0; i < words.size() - 1; ++i)
    {
     std::size_t lower = i;
      for (std::size_t r = i+1; i < words.size(); ++r)
      {
        if (words[i] > words[r])
        {
          lower = r;
        } 
      }

      if (lower != i)
      {
        std::string swap = words[i];
        words[i] = words[lower];
        words[lower] = swap;
      }
    }
    /////////////////////// END-TO-DO (2) ////////////////////////////
  }
};  // struct Algorithm_1












// Merge Sort on Vectors
struct Algorithm_2 : Algorithm<std::vector<std::string>>                        // Collection_Type binds to std::vector<std::string>
{
  std::string description() const override { return "Algorithm 2:  Merge Sort on Vectors"; }



  void run( Collection_Type & words ) const override
  { words = merge_sort( words ); }



  Collection_Type merge_sort( const Collection_Type & words ) const
  {
    //              +---+---+---+---+---+---+---+---+---+---+              //
    //              | A | B | C | I | E | H | D | J | F | G |              //
    //              +---+---+---+---+---+---+---+---+---+---+              //
    //                     /                         \                     //
    //                    /                           \                    //
    //                   /                             \                   //
    //                  v                               v                  //
    //    +---+---+---+---+---+                   +---+---+---+---+---+    //
    //    | A | B | C | I | E |                   | H | D | J | F | G |    //
    //    +---+---+---+---+---+                   +---+---+---+---+---+    //
    //                                 ...                                 //
    //    +---+---+---+---+---+                   +---+---+---+---+---+    //
    //    | A | B | C | E | I |                   | D | F | G | H | J |    //
    //    +---+---+---+---+---+                   +---+---+---+---+---+    //
    //                  \                               /                  //
    //                   \                             /                   //
    //                    \                           /                    //
    //                     v                         v                     //
    //              +---+---+---+---+---+---+---+---+---+---+              //
    //              | A | B | C | D | E | F | G | H | I | J |              //
    //              +---+---+---+---+---+---+---+---+---+---+              //
    ///////////////////////// TO-DO (3) //////////////////////////////
    if (words.size() <= 1)
    {
      return words;
    }  

    if (words.size() == 2)
    {
      if (words[0] > words[1])
      {
        Collection_Type sorted;
        sorted.push_back(words[1]);
        sorted.push_back(words[0]);
        return sorted;
      }
      
      return words;
    }
    
    std::size_t mid = words.size() / 2;
    Collection_Type left(words.begin(), words.begin() + mid);
    Collection_Type right(words.begin() + mid, words.end() - 1); 
    left = merge_sort(left); 
    right = merge_sort(right); 
    
    return merge(left, right);
    /////////////////////// END-TO-DO (3) ////////////////////////////
  }



  Collection_Type merge( const Collection_Type & left, const Collection_Type & right ) const
  {
    ///////////////////////// TO-DO (4) //////////////////////////////
    std::size_t i = 0;
    std::size_t j = 0;
    Collection_Type merged;
    merged.reserve(left.size() + right.size());
    while(i < left.size() && j < right.size())
    {
      if (left[i] <= right[j])
      {
        merged.emplace_back(left[i]);
        ++i; 
      } 
      else
      {
        merged.emplace_back(right[j]);
        ++j;
      }
    }
    while(i < left.size())
    {
      merged.emplace_back(left[i]);
      ++i;
    }
    while(j < right.size())
    {
      merged.emplace_back(right[j]);
      ++j;
    }

    return merged;
    /////////////////////// END-TO-DO (4) ////////////////////////////
  }
};  // struct Algorithm_2












// Merge Sort on Linked Lists
struct Algorithm_3 : Algorithm<std::list<std::string>>                          // Collection_Type binds to std::list<std::string>
{
  std::string description() const override { return "Algorithm 3:  Merge Sort on Linked Lists"; }



  void run( Collection_Type & words ) const override
  { words = merge_sort( words ); }



  Collection_Type merge_sort( const Collection_Type & words ) const
  {
    //           +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+          //
    //           | A |<->| B |<->| C |<->| I |<->| E |<->| H |<->| D |<->| J |<->| F |<->| G |          //
    //           +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+          //
    //                           /                                           \                          //
    //                          /                                             \                         //
    //                         /                                               \                        //
    //                        v                                                 v                       //
    //    +---+   +---+   +---+   +---+   +---+                +---+   +---+   +---+   +---+   +---+    //
    //    | A |<->| B |<->| C |<->| I |<->| E |                | H |<->| D |<->| J |<->| F |<->| G |    //
    //    +---+   +---+   +---+   +---+   +---+                +---+   +---+   +---+   +---+   +---+    //
    //                                               ...                                                //
    //    +---+   +---+   +---+   +---+   +---+                +---+   +---+   +---+   +---+   +---+    //
    //    | A |<->| B |<->| C |<->| E |<->| I |                | D |<->| F |<->| G |<->| H |<->| J |    //
    //    +---+   +---+   +---+   +---+   +---+                +---+   +---+   +---+   +---+   +---+    //
    //                         \                                                /                       //
    //                          \                                              /                        //
    //                           \                                            /                         //
    //                            v                                          v                          //
    //           +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+          //
    //           | A |<->| B |<->| C |<->| D |<->| E |<->| F |<->| G |<->| H |<->| I |<->| J |          //
    //           +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+   +---+          //
    ///////////////////////// TO-DO (5) //////////////////////////////
   
    return words;  
    // base case
    // create new list of words for left
    //   - leftSize = size / 2
    //   - head = head. tail = words.begin + words.size() / 2
    // create new list of words for right
    if (words.size() == 1)
    {
      return words;
    }
    
    if (words.size() == 2)
    {
      if(words.front() > words.back())
      {
        Collection_Type sorted;
        sorted.emplace_back(words.back());
        sorted.emplace_back(words.front());
        
        return sorted;
      }

      return words; 
    }

    // Creating and advancing iterator to midpoint.
    auto mid = words.begin();
    std::advance(mid, words.size() / 2); 
    Collection_Type left(words.begin(), mid);

    auto end = words.begin();
    std::advance(end, words.size() - 1);
    std::advance(mid, 1);
    // This may be wrong. Should use words.end() instead?
    Collection_Type right(mid, end);
    left = merge_sort(left);
    right = merge_sort(right);

    return merge(left, right);
    /////////////////////// END-TO-DO (5) ////////////////////////////
  }



  Collection_Type merge( const Collection_Type & left, const Collection_Type & right ) const
  {
    ///////////////////////// TO-DO (6) //////////////////////////////
    Collection_Type merged;
    // check if left is greater than right. if so, swap. 
    auto left_it = left.begin();
    auto right_it = right.begin();
    while (left_it != left.end() && right_it != right.end())
    {
      if (*left_it < *right_it)
      {
        merged.push_front(*left_it);
        ++left_it; 
      }
      else
      {
        merged.push_front(*right_it);
        ++right_it;
      }
    }

    while (left_it != left.end())
    {
      merged.push_front(*left_it);
      ++left_it;
    }

    while (right_it != right.end())
    {
      merged.push_front(*right_it);
      ++right_it;
    }

    return merged; 
    /////////////////////// END-TO-DO (6) ////////////////////////////
  }
};  // struct Algorithm_3












// Quick Sort
struct Algorithm_4 : Algorithm<std::vector<std::string>>                        // Collection_Type binds to std::vector<std::string>
{
  std::string description() const override { return "Algorithm 4:  Quick Sort"; }



  void run( Collection_Type & words ) const override
  {
    ///////////////////////// TO-DO (7) //////////////////////////////
    // calls qs_range with the whole range of elements
    qs_range(words, 0, words.size() - 1);
    /////////////////////// END-TO-DO (7) ////////////////////////////
  }



  // The elements to be sorted are indexed by the open range [start..end).
  void qs_range( Collection_Type & words, std::size_t start, std::size_t end ) const
  {
    ///////////////////////// TO-DO (8) //////////////////////////////
    // sorts the vector into two sorted partitions
    // recursively calls qs_range until partition size <= 2 and is confirmed sorted.
    // partition based on a pivot value. 
    // call qs_range for left and right sides the words collection, using the pivot index as the separator between left and right sides. 
    
    if (end - start < 1)
    {
      return;
    }
    
    if (end - start == 1)
    {
      if (words[start] > words[end])
      {
        std::string swap = words[start];
        words[start] = words[end];
        words[end] = swap;
      }

      return;
    }
    
    std::size_t p = qs_partition(words, start, end);

    if (p > start)
    {
      qs_range(words, start, p);
    }
    if (p < end)
    {
      return;
      qs_range(words, p+1, end);
    }
    
    /////////////////////// END-TO-DO (8) ////////////////////////////
  }



  // The elements to be partitioned are indexed by the open range [start..end).
  std::size_t qs_partition( Collection_Type & words, std::size_t start, std::size_t end ) const
  {
    //    s               i       j                       p   e
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+...+
    //  | A | D | B | C | H | F | G | N | P | W | M | O | M |   :
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+...+
    //  |<-L (≤ pivot)->|<---todo-->|<---H (≥ pivot)--->|   |
    //  |     [s..i)    |   [i..j]  |     [j+1..e-1)    |   |
    //
    //  i is the start (unsigned) index of the todo zone
    //  j is the last (unsigned) index of the todo zone
    //  s is the start index of the slice to partition
    //  e is one past the end index of the slice to partition
    //  p is the location where the pivot value is temporary stored

    // Requirement:  The pivot element must be chosen randomly
    // Programming Note:  The C random number tools (rand(), srand()) have been deprecated in favor of the C++ random number library, which
    //     provides better randomness and more features.  Do not use rand() or srand() in your implementation.  See the C++ reference
    //     documentation (https://en.cppreference.com/w/cpp/numeric/random.html) for std::default_random_engine, std::random_device, and
    //     std::uniform_int_distribution for more information on how to use the C++ random number library.

    ///////////////////////// TO-DO (9) //////////////////////////////
    // RANDOM GENERATION SETUP
    // Random number generator for ints across uniform distribution. Used as the seed source for the unsigned int generator.
    std::random_device rd;
    // 32-bit Unsigned int generator using the std::mersenne_twister_engine: https://cplusplus.com/reference/random/mersenne_twister_engine/
        // Alters internal state by "twisting" with an xor mask on a mis of bits. Neat!
    std::mt19937 gen(rd());
    // Produces a uniform distribution of ints constrained to a range. 
        // https://en.cppreference.com/cpp/numeric/random/uniform_int_distribution    
    std::uniform_int_distribution<std::size_t> distribution(start, end);
    // select random pivot. swap to end. 
    std::size_t pivot = distribution(gen);
    if (pivot != end)
    {
      std::swap(words[pivot], words[end]);
    }

    std::size_t i = start;
    std::size_t j = end - 1;
    // shrink to-do zone. swap as needed.
    while (i < j && j < end)
    {
      if (words[i] <= words[end])
      {
        ++i;
      }
      else if (words[j] > words[end])
      {
        --j;
      }
      else
      {
        std::swap(words[i], words[j]);
        ++i;
        --j;
      }
    }

    if (words[i] > words[end])
    {
      std::swap(words[i], words[end]);
    }

    return i;
    /////////////////////// END-TO-DO (9) ////////////////////////////
  }
};    // struct Algorithm_4
