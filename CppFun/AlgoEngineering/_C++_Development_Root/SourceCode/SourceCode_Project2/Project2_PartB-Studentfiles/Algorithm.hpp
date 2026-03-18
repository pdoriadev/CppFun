#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives

// Food Item Descriptions
#include <string>
// uint#_t Types
#include <cstdint>
#include "FoodPantry.hpp"
#include <cstdio>
  // print
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
    
    // selection sort FoodItems by ratio of calories to weight
    std::vector<FoodItem *> inv = {};

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

      if (best == i)
      {
        continue;
      }

      FoodItem const* swap = Algorithm::_todays_inventory[i];
      //std::swap(Algorithm::_todays_inventory[i], Algorithm::_todays_inventory[best]);
      //Algorithm::_todays_inventory[i] = Algorithm::_todays_inventory[best];
      //Algorithm::_todays_inventory[best] = swap; 
    }

    std::vector<uint8_t> greedyItems = {}; 
    unsigned sumW = 0;  
    for (uint8_t i = 0; i < Algorithm::_todays_inventory.size(); ++i)
    {
      // check if sumW would exceed weight limit by adding the current item
      if (sumW + Algorithm::_todays_inventory[i]->weight() > weight_limit)
      {
        continue;         
      }
      
      greedyItems.push_back(i);
      std::print(std::cout, "{:>5}: {::s}\n", i, Algorithm::_todays_inventory[i]->description(), Algorithm::_todays_inventory[i]->weight(), Algorithm::_todays_inventory[i]->calories(), Algorithm::_todays_inventory[i]->calories() / Algorithm::_todays_inventory[i]->weight());
    }
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
    return Algorithm::_todays_inventory;
    /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

