#pragma once

///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  ///
  /// Do not put anything else in this section, i.e. comments, classes, functions, etc.  Only #include directives

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
		///	-------------------------------------------------------------
		/// -------------------- IMPLEMENTATION NOTES -------------------
		///	inputFile - file name is redirected i: 
		///	  
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
    ///////////////////////// TO-DO (3) //////////////////////////////

    /////////////////////// END-TO-DO (3) ////////////////////////////
  }
};

