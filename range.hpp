#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>

namespace shake {

//----------------------------------------------------------------
// A range wraps a begin and end iterator,
// in order to operate easily on the whole range of values.
// Whether the range object is const says nothing about
// whether the elements that are iterated over are const.
// Instead, the iterator template parameters can be either const or non-const.
// Construct with {  begin,  end } for non-const range.
// Construct with { cbegin, cend } for const range.
// You can construct a const range over a const container.
// You can construct a const range over a non-const container.
// You can construct a non-const range over a non-const container.
// You can NOT construct a non-const range over a const container.
template<typename iterator_t>
class Range
{
public:
    // could be a const_iterator, dependent on how the range is constructed
    using iterator = iterator_t;

    Range
    (
        iterator_t begin,
        iterator_t end
    )
        : m_begin   { begin }
        , m_end     { end }
    { }

    iterator    begin() const  { return m_begin;  }
    iterator    end()   const  { return m_end;    }

private:
    iterator    m_begin;
    iterator    m_end;
};

//----------------------------------------------------------------
// Constructs a non-const range which means the elements that are iterated over can be changed.
template<typename container_t>
Range<typename container_t::iterator> range
(
    container_t& v
)
{
    return Range<typename container_t::iterator>
    {
        std::begin  ( v ),
        std::end    ( v )
    };
}

//----------------------------------------------------------------
// Constructs a const range, which means the elements that are iterated over can not be changed.
template<typename container_t>
Range<typename container_t::const_iterator> const_range
(
    container_t& v
)
{
    return Range
    {
        std::cbegin ( v ),
        std::cend   ( v )
    };
}

} // namespace shake

#endif // RANGE_HPP