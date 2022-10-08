#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "any_range.hpp"
#include "combine_range.hpp"
#include "enumerate_range.hpp"
#include "index_range.hpp"
#include "map_range.hpp"
#include "range.hpp"
#include "step_range.hpp"
#include "transform_range.hpp"

namespace shake {
namespace unit_tests {

//----------------------------------------------------------------
template<typename T>
inline void print_outcome( const T& result, const T& expected_result, const std::string& function_name )
{
    const auto result_string = std::string{ ": " } + ( result == expected_result ? "SUCCESS" : "FAILED !!!" );
    std::cout << function_name << result_string << "\n";
}

//----------------------------------------------------------------
// INDEX RANGE

inline void test_index_range()
{
    // simply create a string from all indices in an index range
    auto result = std::string{ "" };
    for ( const auto& i : range( 10 ) )
    {
        result += std::to_string( i );
    }

    const auto expected_result = std::string{ "0123456789" };
    print_outcome(result, expected_result, "test_index_range");
}

//----------------------------------------------------------------
// STEP RANGE

inline void test_step_range_exact_fit()
{
    auto vector = std::vector<int> { 0, 1, 2, 3, 4 };
    auto result = std::vector<int> { };
    for ( const auto& value : step( range( vector ), 2 ) )
    {
        result.emplace_back( value );
    }
    const auto expected_result = std::vector<int> { 0, 2, 4 };
    print_outcome(result, expected_result, "test_step_range_exact_fit");
}

inline void test_step_range_with_remainder()
{
    auto vector = std::vector<int> { 0, 1, 2, 3, 4, 5 };
    auto result = std::vector<int> { };
    for ( const auto& value : step( range( vector ), 2 ) )
    {
        result.emplace_back( value );
    }
    const auto expected_result = std::vector<int> { 0, 2, 4 };
    print_outcome( result, expected_result, "test_step_range_with_remainder" );
}

inline void test_step_range_large_step()
{
    auto vector = std::vector<int> { 0, 1, 2, 3, 4, 5 };
    auto result = std::vector<int> { };
    for ( const auto& value : step( range( vector ), 4 ) )
    {
        result.emplace_back( value );
    }
    const auto expected_result = std::vector<int> { 0, 4 };
    print_outcome(result, expected_result, "test_step_range_large_step");
}

//----------------------------------------------------------------
// COMBINE RANGE

inline void test_combine_range()
{
    auto ints = std::vector<int> { 1, 2, 3 };
    auto strings = std::vector<std::string> { "a", "b", "c" };
    auto result = std::vector<std::string> { };
    for ( const auto& [ i, s ] : combine( range(ints), range(strings) ) )
    {
        result.emplace_back( std::to_string( i ) + std::string( " : " ) + s );
    }
    const auto expected_result = std::vector<std::string> {
        "1 : a",
        "2 : b",
        "3 : c"
    };
    print_outcome(result, expected_result, "test_combine_range");
}

//----------------------------------------------------------------
// ENUMERATE RANGE

inline void test_simple_enumerate()
{
	const auto input = std::vector<std::string> { "zero", "one", "two" };
    auto result = std::vector<std::string> { };
	for ( const auto& [ i, s ] : enumerate( const_range( input ) ) )
	{
		result.emplace_back( std::to_string( i ) + std::string( " : " ) + s );
	}
	const auto expected_result = std::vector<std::string>
	{
		"0 : zero",
        "1 : one",
        "2 : two"
	};
	print_outcome( result, expected_result, "test_simple_enumerate" );
}

inline void test_modifying_enumerate()
{
	auto modified_input = std::vector<std::string> { "zero", "one", "two" };
	for ( const auto& [ i, s ] : enumerate( range( modified_input ) ) )
	{
		s = std::to_string( i ) + std::string( " : " ) + s;
	}
	const auto expected_result = std::vector<std::string>
	{
		"0 : zero",
        "1 : one",
        "2 : two"
	};
	print_outcome( modified_input, expected_result, "test_modifying_enumerate" );
}


//----------------------------------------------------------------
// MAP RANGE

inline void test_map_range_keys()
{
    const auto map = std::map<int,std::string> { { 1, "one" }, { 2, "two" }, { 3, "three" } };
    auto result = std::vector<int> {};
    for ( const auto& key : keys( map ) )
    {
        result.emplace_back( key );
    }
    const auto expected_result = std::vector<int>{ 1, 2, 3 };
    print_outcome(result, expected_result, "test_map_range_keys");
}

inline void test_map_range_values()
{
    const auto map = std::map<int,std::string> { { 1, "one" }, { 2, "two" }, { 3, "three" } };
    auto result = std::vector<std::string> {};
    for ( const auto& value : values( map ) )
    {
        result.emplace_back( value );
    }
    const auto expected_result = std::vector<std::string>{ "one", "two", "three" };
    print_outcome(result, expected_result, "test_map_range_values");
}

//----------------------------------------------------------------
// TRANSFORM RANGE

inline void test_transform_range_int_as_string()
{
    // create a transform range, that transforms an int to a string for you
    auto result = std::string{ "" };
    const auto transformed_range = transform<const int&, std::string>
    (
        range( 10 ),
        []( const int& i ) -> std::string
        {
            return std::to_string( i );
        }
    );

    // we can now iterate over the range of ints, as if it is a range of strings
    for ( const auto s : transformed_range )
    {
        // note that the converison to string has already been performed for us
        result += s;
    }

    const auto expected_result = std::string{ "0123456789" };
    print_outcome( result, expected_result, "test_transform_range_int_as_string" );
}

inline void test_transform_range_modifying_int_through_tuple()
{
    // create a transform range, that transforms an int to a tuple that contains a reference to the original int, and a dummy variable
    auto result = std::vector { 1, 2, 3 };
    const auto transformed_range = transform<int&, std::tuple<int&,int> >
    (
        range( result ),
        []( int& i ) -> std::tuple<int&,int>
        {
            return std::forward_as_tuple( i, 3 );
        }
    );

    // we can now iterate over the range of ints, as if it is a range of tuples
    // we unpack the tuple using structured bindings
    for ( auto [ f1, f2 ] : transformed_range )
    {
        // we can still modify the original int through the reference
        f1 += 1;
        // the dummy isn't really interesting now
        assert( f2 == 3 );
    }

    const auto expected_result = std::vector<int>{ 2, 3, 4 };
    print_outcome(result, expected_result, "test_transform_range_modifying_int_through_tuple");
}

//----------------------------------------------------------------
inline void test_any_range()
{
    // create a transform range, that transforms an int to a string for you
    auto int_to_string = []( int i ) -> std::string
    {
        return std::to_string( i );
    };
    auto transformed_range = transform<const int&, std::string> ( range( 10 ), int_to_string );

    // this function will accept any range that produces strings.
    // The types of what is producing the strings are erased so that you can pass in any kind of string producer
    // The fact that this was a transformed range is hidden to the function,
    // and not required to be specified in the argument type
    const auto consume_any_string_range = []( AnyRange<std::string> string_range ) -> std::string
    {
        auto result = std::string { "" };
        for ( const auto& s : string_range )
        {
            result += s;
        }
        return result;
    };
    auto any_range = make_any_range(transformed_range);
    const auto result = consume_any_string_range( any_range );
    const auto expected_result = std::string { "0123456789" };
    print_outcome(result, expected_result, "test_any_range");
}

//----------------------------------------------------------------
inline void run()
{
    test_index_range();

    test_step_range_exact_fit();
    test_step_range_with_remainder();
    test_step_range_large_step();

    test_combine_range();

    test_simple_enumerate();
    test_modifying_enumerate();

    test_map_range_keys();
    test_map_range_values();

    test_transform_range_int_as_string();
    test_transform_range_modifying_int_through_tuple();

    test_any_range();
}


} // unit_tests
} // namespace shake

#endif // UNIT_TESTS_HPP