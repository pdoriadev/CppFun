#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives
#include <iostream>
#include <stdint.h>
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
		uint64_t n = disks.size() / 2;

		if (n <= 1)
		{
			// Disks is already sorted
			return 0;
		}

		uint64_t swaps = 0;
		uint64_t j = 0; // j/2 = completedMows
		uint8_t mowDirection = 1;
		for (uint64_t i = 0; j < n; )
		{
			// Swap Check.
			if (static_cast<uint8_t>(disks[i].color()) * mowDirection > static_cast<uint8_t>(disks[i+mowDirection].color()) * mowDirection)
			{
				Disk swap = disks[i].color();
				disks[i] = disks[i+mowDirection];
				disks[i+mowDirection] = swap;
				swaps+=1;
			}

			i+=mowDirection;

			// Bounds Check. If bounds is exceeded, switch mow direction.
			if (i+mowDirection <= 0 || i+mowDirection >= 2*n-1)
			{
				mowDirection *= (-1);
				j++;
				// :>3 means 3 columns, right justified.
				std::print( std::cout, "{:>3}: {::s}\n", j, disks );
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
		uint64_t n = disks.size() / 2;
		if (n <= 1)
		{
			// Disks is already sorted
			return 0;
		}

		uint64_t swaps = 0;
		uint64_t j = 0;
		uint64_t startingIndex = 0;
		for (uint64_t i = 0; j < n; )
		{
			// Swap Check.
			if (static_cast<uint8_t>(disks[i].color()) > static_cast<uint8_t>(disks[i+1].color()))
			{
				Disk swap = disks[i];
				disks[i] = disks[i+1];
				disks[i+1] = swap;
				swaps++;
			}

			i+=2;

			// Bounds Check. If bounds is exceeded, switch the starting index between
				// 0 and 1, and start the loop again.
			if (i+1 >= n-1)
			{
				startingIndex = 1 - startingIndex;
				i = startingIndex;
				j++;
				std::print( std::cout, "{:>3}: {::s}\n", j, disks );
			}
		}

		return swaps;
  /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

