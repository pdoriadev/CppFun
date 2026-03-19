#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives

#include <string>
  // Food Item Descriptions
#include <cstdint>
  // uint_t Types
#include "FoodPantry.hpp"
#include <cstdio>
  // print()
#include <cmath>
  // pow()
/////////////////////// END-TO-DO (1) ////////////////////////////






// Algorithm Abstract Base Class and Interface
class Algorithm
{
  public:
    Algorithm( FoodItem::WeightRange const & weight_range, unsigned quantity )
    : _todays_inventory{ FoodPantry::make_inventory( weight_range, quantity ) }
    {}


    Algorithm( FoodPantry inventory )
    : _todays_inventory{ std::move(inventory) }
    {}


    virtual std::string description() const    { return typeid( *this ).name(); }

    virtual FoodPantry::SelectedFoodItems run( unsigned weight_limit ) const = 0;

    FoodPantry const & inventory() const noexcept { return _todays_inventory; }

    virtual ~Algorithm() = default;


  protected:
    const FoodPantry _todays_inventory;
};






// The Greedy Pattern
struct Algorithm_1 : Algorithm
{ using Algorithm::Algorithm;

  std::string description() const override    { return "Algorithm 1:  The Greedy Pattern"; }

  // Input:
  //   1) weight_limit - A positive total weight W (integer number of ounces); and
  //   2) Algorithm::_todays_inventory - A collection F of 0<n<64 “food item” objects where each object f=(w,c) is an integer weight w>0 in
  //      ounces and a number of calories c≥0
  //
  // Output:
  //   1) SelectedFoodItems - the selected food items, which must be a subset of the inventory and whose total weight is less than or equal
  //      to the weight limit
  FoodPantry::SelectedFoodItems run( unsigned weight_limit ) const override
  {
    ///////////////////////// TO-DO (2) //////////////////////////////
    /// Run using the Scenarios-*.txt files 
    /// - Each file has one or more scenarios
    /// - These are just starting points
    ///
    /// Scenario parameters
    /// - weight limit you can carry
    /// - min and max weight of food items in the selection pool. 
    /// - size of the selection pool
    ///
    /// Create your own scenarios to test.
    /// "Redirect standard input from a scenario file and redirect standard output to an output text file"???? Redirect input where?
    ///
    /// Running the Program 
    /// - Run the program using Scenarios-ForDelivery.txt as input
    /// - creating output-ForDelivery.txt with your output.
    ///
    ///
    ///	-------------------------------------------------------------
    ///
    /// ------------- USAGE & IMPLEMENTATION NOTES -------------------
    ///	inputFile - redirect scenario file into .exe
    ///	outputFile - redirect .exe's output to output-ForDelivery.txt
    
    // Vector to track FoodItem and ratio for sorting. 
    std::vector<std::pair<FoodItem const *, const double>> foodByRatio = {};
    foodByRatio.reserve(Algorithm::_todays_inventory.size());
    
    // Selection sort FoodItems by ratio of calories to weight
    for (uint8_t i = 0; i < Algorithm::_todays_inventory.size(); ++i)
    {
      uint8_t best = i;
      double bestCal = 0;
      unsigned bestW = 0; 
      for (uint8_t j = 0; j < Algorithm::_todays_inventory.size(); ++j) 
      {
        const double curCal = Algorithm::_todays_inventory[j]->calories();
        const unsigned curW = Algorithm::_todays_inventory[j]->weight();
         
        // check if weight exceeds weight limit 
        if (curW >= weight_limit)
        {
          continue;  
        }

        // check if calories/weight is less than the best 
        if (curCal/curW < bestCal/bestW) 
        {
          continue;
        }
        
        // edge-case checks if cal/weight ratios are equal
        // Note: Using NOT GREATER THAN because using == for two floats
        //    creates a warning: "comparing floating point with == or != is unsafe [-Werror,-Wfloat-equal]"
        if ((curCal/curW > bestCal/bestW) == false)
        {
          if (curCal < bestCal)
          {
            continue;
          }

          if (curW > bestW)
          {
            continue; 
          }
        }

        best = j;
        bestCal = curCal;
        bestW = curW; 
      }

      foodByRatio.push_back(std::pair(Algorithm::_todays_inventory[best], bestCal/bestW));
    }


    unsigned sumW = 0;  
    for (uint8_t i = 0; i < foodByRatio.size(); ++i)
    {
      // check if sumW would exceed weight limit by adding the current item
      if (sumW + std::get<0>(foodByRatio[i])->weight() > weight_limit)
      {
        continue;         
      }
      sumW += std::get<0>(foodByRatio[i])->weight();

      // Throwing error because std::print is a non-const function
      // std::print(std::cout, "{:>6}: {::s}\n", "i", "Description", "Weight", "Calories", "12345");
      // std::print(std::cout, "{:>6}: {::s}\n", i, std::get<0>(foodByRatio[i])->description(), std::get<0>(foodByRatio[i])->weight(), std::get<0>(foodByRatio[i])->calories(), "12345");
    }

    // Required by the function's return type
    return Algorithm::_todays_inventory;
    
    ///	/////////////////////// END-TO-DO (2) ////////////////////////////
  }
};






// The Exhaustive Search Pattern
struct Algorithm_2 : Algorithm
{
  using Algorithm::Algorithm;

  std::string description() const override { return "Algorithm 2:  The Exhaustive Pattern"; }

  // Input:
  //   1) weight_limit - A positive total weight W (integer number of ounces); and
  //   2) Algorithm::_todays_inventory - A collection F of 0<n<64 “food item” objects where each object f=(w,c) is an integer weight w>0 in
  //      ounces and a number of calories c≥0
  //
  // Output:
  //   1) SelectedFoodItems - the selected food items, which must be a subset of the inventory and whose total weight is less than or equal
  //      to the weight limit
  FoodPantry::SelectedFoodItems run( unsigned weight_limit ) const override
  {
    std::print(std::cout , "%d", weight_limit);
    ///////////////////////// TO-DO (3) //////////////////////////////
/* Generate candidate bitmasks - Proof for Candidate Generation by iterating through all possible integer values for an integer of n bits.
    Let the list of grocery items L be a list of n elements long.
    Let each element correspond to a binary bit in an unsigned integer type X of n bits long.
    Let us represent the max value of X with MAX. 
    Each unique set of binary bits in X corresponds to a unique integer between 0 and MAX (inclusive).
    Therefore, every value that can be represented by X corresponds to a unique binary set of 1's and 0's of length n. 
    Since we have n grocery items, each unique set of grocery items can be represented by a matching binary set of 1's and 0's of length n. Therefore, each value of X (i.e. X(0), X(1), X(...), X(n-1), X(n)) is made of a binary value that corresponds to every set of grocery items, including the empty set.
    By iterating through all values of X, we can iterate through all candidates for exhaustive search.
     
    For a grocery list of length n=2.
      X(0) = 00. Empty set. No grocery Algorithm::_todays_inventory. 
      X(1) = 01. Includes Item 1.
      X(2) = 10. Includes Item 2.
      X(2) = 11. Includes ALL grocery Algorithm::_todays_inventory. 
*/
    uint64_t numberOfSets = 0;
    // Workaround because c++ doesn't have an integer pow function.
    for (size_t place = 0; place < Algorithm::_todays_inventory.size(); ++place)
    {
      // for each place, shift a 1 onto the numberOfSets
      numberOfSets += 1;
      numberOfSets = numberOfSets << 1; 
    }  
  
    uint64_t best = 0;
    double bestCalories = 0;
    unsigned bestWeight = 0;

    for (uint64_t i = 0; i < numberOfSets; ++i)
    {
      double calories = 0;
      unsigned weight = 0;
      bool bestCandidate = true;

      // Verification Algorithm  
      size_t maxPlaces = Algorithm::_todays_inventory.size();
      for (size_t places = 0; places < maxPlaces; ++i)
      {
        // DOES BIT SHIFTING WORK THE SAME LIKE THIS IN CPP
        if (((i >> places) % 2) == 0)
            continue;
        
        weight += Algorithm::_todays_inventory[places]->weight();
        calories += Algorithm::_todays_inventory[places]->calories();
        if (weight > weight_limit)
        {
          bestCandidate = false;
          break;  
        }
      } 
     

      // edge-case checks if cal/weight ratios are equal
      if (calories < bestCalories)
      {
        bestCandidate = false;
      }
      else if((calories > bestCalories) == false)
      {
        /* Note: Using NOT GREATER THAN because using == for two floats
            creates a warning: "comparing floating point with == or != 
            is unsafe [-Werror,-Wfloat-equal]"*/
        if (weight > bestWeight)
        {
          bestCandidate = false;  
        }
      }

      if (bestCandidate == false)
        continue;

      best = i;
      bestCalories = calories;
      bestWeight = weight;
    }

    for (uint8_t i = 0; i < Algorithm::_todays_inventory.size(); ++i)
    {
      if (((best >> i) % 2) == 1)
      {
        std::print(std::cout, "%d", 1);
        std::print(std::cout, "{:>2}:\n", 1, 2);
        std::print(std::cout, "{:>3}: {::s}\n", 1, 2, 3);
       //std::print(std::cout, "{:>6}: {::s}\n", i, std::get<0>(foodByRatio[i])->description(), std::get<0>(foodByRatio[i])->weight(), std::get<0>(foodByRatio[i])->calories(), "12345");       
      }
    }
    return Algorithm::_todays_inventory;
    /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

