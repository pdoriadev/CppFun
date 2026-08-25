#include <cstddef>                                                                      // size_t
#include <exception>                                                                    // exception, what()
#include <iostream>                                                                     // print, cout, cerr
#include <typeinfo>                                                                     // typeid()::name()

#include "Algorithm.hpp"





namespace    // unnamed, anonymous namespace
{
  // Driver function
  void run( Algorithm && algo )
  {
    try
    {
      const auto n = algo.disks.size() / 2;

      // Display the initial state
      std::print( std::cout,
                  "{:=>40}\n"
                  "{}\n"
                  "Initial Row:  {::s}\n"
                  "n =        :  {}\n\n"
                  "Starting execution\n"
                  "{:.>25}\n",
                  "", algo.description(), algo.disks, n, "" );

      // Execute the algorithm
      std::size_t m = algo.run();


      // Display the final state
      std::print( std::cout,
                  "{:.>25}\n"
                  "Execution completed\n\n"
                  "Final Results:  {::s}\n"
                  "Is sorted:  {:s}\n"
                  "n = {}\n"
                  "m = {}\n"
                  "{:=>40}\n\n\n",
                  "", algo.disks, algo.disks.is_sorted(), n, m, "");
    }
    catch (std::exception & ex)
    {
      std::print( std::cerr, "\n\n*** Error, unexpected exception caught\n{}\n{}\n\n", typeid( ex ).name(), ex.what() );
      throw;
    }
  }
}  // namespace




int main()
{
  // Uncomment if you're unable to display UTF8 characters on in your terminal window
  // std::formatter<Disk>::mode = std::formatter<Disk>::ASCII;

  // Run the 2 algorithms with various sizes, starting with n=4
  for( unsigned n : { 4, 7, 1, 2 } )
  {
    run( Algorithm_1{ n } );
    run( Algorithm_2{ n } );
  }
}
