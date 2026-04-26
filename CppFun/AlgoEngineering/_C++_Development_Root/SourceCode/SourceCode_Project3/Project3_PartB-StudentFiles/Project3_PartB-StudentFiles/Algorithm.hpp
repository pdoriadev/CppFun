pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives
#include <vector>
#include <string>
#include <stdint>
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

    for (uint32_t i = 0; i < words.size() - 1; i++)
    {
      lower = i
      for (uint32_t r = i+1; i < words.size(); j++)
      {
        if (words[i] > words[r])
        {
          lower = r
        } 
      }

      if (lower != i)
      {
        Collection_Type swap = words[i]
        words[i] = words[lower]
        words[lower] = swap
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

    /////////////////////// END-TO-DO (3) ////////////////////////////
  }



  Collection_Type merge( const Collection_Type & left, const Collection_Type & right ) const
  {
    ///////////////////////// TO-DO (4) //////////////////////////////

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

    /////////////////////// END-TO-DO (5) ////////////////////////////
  }



  Collection_Type merge( const Collection_Type & left, const Collection_Type & right ) const
  {
    ///////////////////////// TO-DO (6) //////////////////////////////

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

    /////////////////////// END-TO-DO (7) ////////////////////////////
  }



  // The elements to be sorted are indexed by the open range [start..end).
  void qs_range( Collection_Type & words, std::size_t start, std::size_t end ) const
  {
    ///////////////////////// TO-DO (8) //////////////////////////////

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

    /////////////////////// END-TO-DO (9) ////////////////////////////
  }
};    // struct Algorithm_4
