#pragma once

#include <cstddef>      // size_t
#include <format>       // formatter, format(), format_to()
#include <fstream>      // ifstream
#include <iostream>     // print(), cin
#include <iterator>     // istream_iterator
#include <stdexcept>    // runtime_error
#include <utility>      // move()
#include <vector>       // vector

#include "FoodItem.hpp"


class FoodPantry : public std::vector<FoodItem const*>
{
  public:
    using SelectedFoodItems = FoodPantry;                 // Type alias for FoodPantry
    using std::vector<FoodItem const*>::vector;           // Inherit constructors



    static SelectedFoodItems make_inventory( FoodItem::WeightRange const & weight_range, unsigned quantity )
    {
        static const std::vector<FoodItem> all_foods{ []
                                                      {
                                                        std::ifstream food_file( "Food.csv" );
                                                        if( !food_file ) throw std::runtime_error( "Failed to open Food.csv" );
                                                        return std::vector<FoodItem>{  std::istream_iterator<FoodItem>( food_file ),
                                                                                       std::istream_iterator<FoodItem>() };
                                                      }()
                                                    };

      constexpr unsigned max_quantity = ( sizeof( std::size_t ) * 8 ) - 1;           // number of bytes * 8 bits/byte  with a range of zero to 2^n - 1
      if( quantity > max_quantity ) quantity = max_quantity;                         // Algorithm definition requires 1 bit per item

      SelectedFoodItems inventory;
      inventory.reserve( quantity );

      // Grab the first quantity items from all_foods that are valid and whose weight is within the specified weight range.
      for( auto item = all_foods.begin(), end = all_foods.end(); ( inventory.size() < quantity ) && ( item != end ); ++item )
      {
        auto && [min_weight, max_weight] = weight_range;
        if( item->is_valid()  &&  item->weight() >= min_weight  &&  item->weight() <= max_weight )  inventory.push_back( &*item );
      }
      return inventory;
    }


    std::size_t weight()
    {
      std::size_t result = 0;
      for( const auto & item : *this ) result += item->weight();
      return result;
    }


    long double calories()
    {
      long double result = 0.0;
      for( const auto & item : *this ) result += item->calories();
      return result;
    }
};






// Specialize the formatter to format a standard vector of pointers-to-FoodItem typed objects
template<>
struct std::formatter<FoodPantry>
{
  // No container-level nor FoodItem format specifiers supported.
  constexpr auto parse( auto & ctx )  { return ctx.begin(); }


  auto format( const FoodPantry & collection, auto & ctx ) const
  {
    std::size_t max_description_length = 0;
    std::size_t max_weight_length      = 0;
    std::size_t max_calories_length    = 0;
    std::size_t max_ratio_length       = 0;

    for( const auto & item : collection )
    {
      const auto description_length = std::format( "{:?},",  item->description()               ).length();
      const auto weight_length      = std::format( "{}",     item->weight()                    ).length();
      const auto calories_length    = std::format( "{:.2f}", item->calories()                  ).length();
      const auto ratio_length       = std::format( "{:.4f}", item->calories() / item->weight() ).length();


      if( description_length > max_description_length )  max_description_length = description_length;
      if( weight_length      > max_weight_length      )  max_weight_length      = weight_length;
      if( calories_length    > max_calories_length    )  max_calories_length    = calories_length;
      if( ratio_length       > max_ratio_length       )  max_ratio_length       = ratio_length;
    }

    auto out = ctx.out();
    std::format_to( out,
                    "       {0:{1}.{1}} {2:{3}.{3}}  {4:{5}.{5}}  {6:{7}.{7}}\n",
                    "Description", max_description_length,
                    "Ozs",         max_weight_length,
                    "Cals",        max_calories_length,
                    "Cal/Oz",      max_ratio_length );

    for( std::size_t i = 0, size = collection.size();  i != size;  ++i )
    {
      const auto & item = *collection[i];
      out               = std::format_to( out, "{:4}: {{{:{}} {:{}}, {:{}.2f}, {:{}.4f}}}\n",
                                                i,
                                                std::format( "{:?},", item.description() ), max_description_length,
                                                item.weight(),                              max_weight_length,
                                                item.calories(),                            max_calories_length,
                                                item.calories() / item.weight(),            max_ratio_length );
    }
    return std::move(out);
  }
};





// Specialize the formatter to format a standard vector of FoodItem typed objects
template<>
struct std::formatter<std::vector<FoodItem>>
{
  // No container-level nor FoodItem format specifiers supported.
  constexpr auto parse( auto & ctx )  { return ctx.begin(); }


  auto format( const std::vector<FoodItem> & collection, auto & ctx ) const
  {
    FoodPantry indirectVector;
    indirectVector.reserve( collection.size() );
    for( auto & item : collection )  indirectVector.push_back( &item );

    return std::formatter<FoodPantry>().format( indirectVector, ctx );
  }
};
