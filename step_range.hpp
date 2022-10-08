#ifndef STEP_RANGE_HPP
#define STEP_RANGE_HPP

#include <cstddef>
#include <iterator>

#include "range.hpp"

namespace shake {

template<typename iterator_t >
class StepIterator
{
public:
    // iterator traits
    using iterator_category = typename iterator_t::iterator_category;
    using value_type        = typename iterator_t::value_type;
    using difference_type   = typename iterator_t::difference_type;
    using pointer           = typename iterator_t::pointer;
    using reference         = typename iterator_t::reference;

public:
    explicit
    StepIterator
    ( 
        iterator_t iterator, 
        std::size_t step_size 
    ) 
        : m_iterator    { iterator  } 
        , m_step_size   { step_size }
    { }

    const iterator_t& get_internal_iterator() const { return m_iterator; }

    StepIterator&  operator++()       { std::advance( m_iterator, m_step_size ); return *this; }
    StepIterator   operator++(int)    { IndexIterator result = *this; ++(*this); return result; }

    bool operator==(StepIterator other) const { return get_internal_iterator() == other.get_internal_iterator(); }
    bool operator!=(StepIterator other) const { return !(*this == other); }

    value_type operator*() const
    {
        return *m_iterator;
    }

private:
    iterator_t m_iterator;
    std::size_t m_step_size;
};

//----------------------------------------------------------------
template<typename iterator_t>
using StepRange = Range<StepIterator<iterator_t>>;

//----------------------------------------------------------------
template<typename range_t>
StepRange<typename range_t::iterator> step
(
    range_t input_range,
    std::size_t step_size
)
{
    const auto begin_iterator = std::begin( input_range );
    const auto distance = std::distance( begin_iterator, std::end( input_range ) );
    // We need to adjust for the fact that the end_iterator is always one increment beyond the last valid state
    const auto n_advances = ( ( static_cast<std::size_t>( distance - 1 ) / step_size ) + 1 ) * step_size;
    const auto adjusted_end_iterator = std::next( begin_iterator, n_advances );

    return Range
    {
        StepIterator( begin_iterator,           step_size ),
        StepIterator( adjusted_end_iterator,    step_size )
    };
}

} // namespace shake

#endif // STEP_RANGE_HPP
