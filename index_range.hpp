#ifndef INDEX_RANGE_HPP
#define INDEX_RANGE_HPP

#include <cstddef>
#include <iterator>

#include "range.hpp"

namespace shake {

//----------------------------------------------------------------
// An index iterator simply iterates from a begin index to an end index
// You could also create an array, vector or tuple of indices,
// but with this iterator you don't have to keep all the indices in memory at once.
class IndexIterator
{
private:
    using index_t = std::size_t;

public:
    // iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type        = index_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = index_t*;
    using reference         = index_t&;

public:

    explicit
    IndexIterator( index_t  index ) 
        : m_current_index { index } 
    { }

    const index_t& get_internal_index() const { return m_current_index; }

    IndexIterator&  operator++()       { ++m_current_index; return *this; }
    IndexIterator   operator++(int)    { IndexIterator result = *this; ++(*this); return result; }

    bool operator==(IndexIterator other) const { return get_internal_index() == other.get_internal_index(); }
    bool operator!=(IndexIterator other) const { return !(*this == other); }

    const index_t& operator*() const
    {
        return m_current_index;
    }

private:
    index_t m_current_index;
};

//----------------------------------------------------------------
using IndexRange = Range<IndexIterator>;

//----------------------------------------------------------------
IndexRange range
(
    const std::size_t& begin_index,
    const std::size_t& end_index
)
{
    return Range
    {
        IndexIterator { begin_index },
        IndexIterator { end_index   }
    };
}

//----------------------------------------------------------------
IndexRange range
(
    const std::size_t& end_index
)
{
    return range( 0, end_index );
}

} // namespace shake

#endif // INDEX_RANGE_HPP