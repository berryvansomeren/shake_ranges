#ifndef TRANSFORM_RANGE_HPP
#define TRANSFORM_RANGE_HPP

#include <functional>

#include "range.hpp"

namespace shake {

//----------------------------------------------------------------
// Iterates over a range of types while exposing them as different types or values,
// by transforming them using a provided function.
// If non-const and capturing by reference,
// even the original values can be modified through the transformed types!
template<typename in_t, typename out_t, typename iterator_t>
class TransformIterator
{
public:
    using functor_t = std::function<out_t(in_t)>;

public:
    // iterator traits
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = out_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

public:

    explicit
    TransformIterator
    (
        const iterator_t&  iterator,
        const functor_t&   transform_functor
    )
        : m_iterator    { iterator }
        , m_functor     { transform_functor }
    { }

    const iterator_t& get_internal_iterator() const { return m_iterator; }

    TransformIterator&  operator++()       { ++m_iterator; return *this; }
    TransformIterator   operator++(int)    { TransformIterator result = *this; ++(*this); return result; }

    bool operator==(TransformIterator other) const { return get_internal_iterator() == other.get_internal_iterator(); }
    bool operator!=(TransformIterator other) const { return !(*this == other); }

    value_type operator*() const
    {
        // Use the provided function to transforms the types that we iterate over
        return std::invoke( m_functor, *m_iterator );
    }

private:
    iterator_t         m_iterator;
    const functor_t    m_functor;
};

//----------------------------------------------------------------
template<typename in_t, typename out_t, typename iterator_t>
using TransformRange = Range<TransformIterator<in_t, out_t, iterator_t>>;

//----------------------------------------------------------------
template<typename in_t, typename out_t, typename range_t>
TransformRange< in_t, out_t, typename range_t::iterator> transform
(
    range_t v, // could be a range over a const or non-const container
    const typename TransformIterator< in_t, out_t, typename range_t::iterator >::functor_t& f
)
{
    return Range
    {
        TransformIterator { std::begin( v ), f },
        TransformIterator { std::end  ( v ), f }
    };
}

} // namespace shake

#endif // TRANSFORM_RANGE_HPP