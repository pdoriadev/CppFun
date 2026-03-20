#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives
#include <iostream>
#include <cstdint>
#include <print>
#include "Disk.hpp"
#include "Disks.hpp"
/////////////////////// END-TO-DO (1) ////////////////////////////


// Algorithm Interface and Abstract Base Class
struct Algorithm
{
  Algorithm( std::size_t n ) : disks( n ) {}

  virtual std::string description()     { return typeid( *this ).name(); }
  virtual std::size_t run() = 0;

  virtual ~Algorithm() = default;

  Disks disks;
};


// The Lawnmower algorithm
struct Algorithm_1 : Algorithm
{
  Algorithm_1( std::size_t n ) : Algorithm( n ) {}

  std::string description() override    { return "Algorithm 1:  The lawnmower algorithm"; }
  std::size_t run() override
  {
  ///////////////////////// TO-DO (2) //////////////////////////////
    /// Requirement:  Display the disks's contents after each pass (a completed left to right OR right to left traversal).  You can use the
    ///               statement:
    ///                   std::print( std::cout, "{:>3}: {::s}\n", j, disks );
    ///               where j is the number of traversals completed (e.g., starts with 1), for example
    ///                    1: [⚪, ⚫, ⚪, ⚫]
    ///                    2: [⚪, ⚪, ⚫, ⚫]
    const uint64_t n = disks.size() / 2;

    if (n <= 1)
    {
      // Disks is already sorted
      std::print( std::cout, "{:>3}: {::s}\n", 1, disks);
      std::print( std::cout, "{:>3}: {::s}\n", 2, disks);
      return 0;
    }

    uint64_t swaps = 0;
    uint64_t halfMows = 0; // halfMows/2 = completedMows
    short mowDirection = 1;
    for (uint64_t i = 0; halfMows < n; )
    {
      // Swap Check.
      if (disks[i].color() * mowDirection > disks[i+mowDirection].color() * mowDirection)
      {
        swap(disks[i], disks[i+mowDirection]);
        swaps += 1;
      }

      i+=mowDirection;

      // Bounds Check. If bounds is exceeded, switch mow direction.
      if (i+mowDirection <= 0 || i+mowDirection >= (2*n)-1)
      {
        mowDirection *= (-1);
        halfMows += 1;
        // :>3 - 3 columns, right justified.
	// ':' - after the first format-spec "{:>3}" means output a colon after the first argument's output.
	// "{::s}" - not sure what this means. 
        std::print( std::cout, "{:>3}: {::s}\n", halfMows, disks);
      }
    }

    return swaps;
  /////////////////////// END-TO-DO (2) ////////////////////////////
  }
};





// The alternate algorithm
struct Algorithm_2 : Algorithm
{
  Algorithm_2( std::size_t n ) : Algorithm( n ) {}

  std::string description() override { return "Algorithm 2:  The alternate algorithm"; }


  std::size_t run() override
  {
  ///////////////////////// TO-DO (3) //////////////////////////////
    /// Requirement:  Display the disks's contents after each run (a completed left to right traversal).  You can use the statement:
    ///                   std::print( std::cout, "{:>3}: {::s}\n", j, disks );
    ///               where j is the number of traversals completed (e.g., starts with 1), for example
    ///                    1: [⚪, ⚫, ⚪, ⚫]
    ///                    2: [⚪, ⚪, ⚫, ⚫]
    const uint64_t n = disks.size() / 2;
    if (n <= 1)
    {
      // Disks is already sorted
      std::print( std::cout, "{:>3}: {::s}\n", 1, disks);
      return 0;
    }

    uint64_t swaps = 0;
    uint64_t runs = 0;
    uint64_t startingIndex = 0;
    for (uint64_t i = 0; runs < n; )
    {
      // Swap Check.
      if (disks[i].color() > disks[i+1].color())
      {
        swap(disks[i], disks[i+1]);
        swaps += 1;
      }

      i+=2;

      // Bounds Check. If bounds is exceeded, switch the starting index between
        // 0 and 1, and start the loop again.
      if (i+1 >= (n*2)-1)
      {
        startingIndex = 1 - startingIndex;
        i = startingIndex;
        runs += 1;
        std::print( std::cout, "{:>3}: {::s}\n", runs, disks );
      }
    }

    return swaps;
  /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

