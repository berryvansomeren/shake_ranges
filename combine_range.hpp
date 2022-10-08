#ifndef COMBINE_RANGE_HPP
#define COMBINE_RANGE_HPP

#include <cstddef>
#include <tuple>

namespace shake {

//----------------------------------------------------------------
// Iterates over multiple ranges simultaneously,
// and exposes their elements by combining them in tuples. 
template<typename... IteratorArgs>
class CombineIterator
{
public:
    using tuple_t = std::tuple<IteratorArgs...>;

private:
    tuple_t m_combined_ranges_iterator;

public:

    // It takes the tuple of the combined iterators,
    // passes it as a parameter pack to our lambda function using std::apply,
    // which dereferences each parameter from the pack using a fold expression,
    // and stores them again in a new tuple.
    // You could unpack the returned tuple again using std::tie or structured bindings.
    // Because it is difficult to specify the type of a tuple that contains the elements
    // corresponding to dereferencing the iterators in the variadic  parameter pack,
    // we just auto-deduce the return type of this function and create an alias for it later.
    auto operator*() const
    {
        return std::apply
        (
            // use std::forward_as_tuple to maintain references
            []( auto&&... args ) { return std::forward_as_tuple( ( *args ) ... ); },
            m_combined_ranges_iterator
        );
    }

public:
    // iterator traits
    using iterator_category = std::random_access_iterator_tag;
    // here we use the auto-deduced return type from our packed dereferencing function
    using value_type        = std::invoke_result_t< decltype( &CombineIterator<IteratorArgs...>::operator* )() >;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

public:

    // The constructor simply stores all iterators in the parameter pack as a tuple
    explicit
    CombineIterator( IteratorArgs... iterator_args )
        : m_combined_ranges_iterator { std::make_tuple( iterator_args... ) }
    { }

    const tuple_t& get_internal_iterator() const { return m_combined_ranges_iterator; }

    // We take the tuple of the combined iterators,
    // passes it as a parameter pack to our lambda function using std::apply,
    // which increments each parameter from the pack using a fold expression,
    // and stores them again in a new tuple which replaces the old iterators.
    CombineIterator& operator++()
    {
        m_combined_ranges_iterator = std::apply
        (
            // use make_tuple because we want to create copies
            []( auto&&... args ) { return std::make_tuple( ( ++args ) ... ); },
            m_combined_ranges_iterator
        );
        return *this;
    }

    CombineIterator operator++(int) { CombineIterator result = *this; ++(*this); return result; }

    bool operator==(CombineIterator other) const { return get_internal_iterator() == other.get_internal_iterator(); }
    bool operator!=(CombineIterator other) const { return !(*this == other); }
};

//----------------------------------------------------------------
template<typename... IteratorArgs>
using CombineRange = Range<CombineIterator<IteratorArgs...>>;

//----------------------------------------------------------------
// Combines multiple ranges, so that you can easily iterate over multiple simultaneously.
// When dereferencing, you obtain a tuple with elements from all ranges combined.
// The return type is auto-deduced, because it is super difficult to express it otherwise.
template<typename... RangeArgs>
auto combine
(
    RangeArgs... range_args
)
{
    // For each range in the parameter pack,
    // compute the length of the range, as the distance between begin() and end().
    // These distances are stored in an initializer list using a fold expression.
    // Compute the minimum value in the initializer list.
    const auto min_range_distance = std::min( { std::distance( std::begin( range_args ), std::end( range_args ) ) ... } );

    // Create a new function to add the minimum range distance to the begin iterator of a range,
    // to compute a new end iterator, so that all ranges end after the same number of increments.
    const auto new_adjusted_end_iterator = [&min_range_distance]( auto range_arg )
    {
        auto it = std::begin( range_arg );
        std::advance( it, min_range_distance );
        return it;
    };

    // Create a new range, with a combined begin iterator, and an adjusted combined end iterator
    return Range
    (
        // Combine the pack of all begin iterators into a new begin iterator
        CombineIterator { std::begin( range_args ) ... },

        // Create new end iterators, and combine the pack into a new end iterator
        CombineIterator { new_adjusted_end_iterator( range_args ) ... }
    );
}

} // namespace shake

#endif // COMBINE_RANGE_HPP