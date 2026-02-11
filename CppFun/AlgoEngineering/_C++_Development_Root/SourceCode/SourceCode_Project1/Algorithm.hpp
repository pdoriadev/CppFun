#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives
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

  /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

