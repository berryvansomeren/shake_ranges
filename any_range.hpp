#ifndef ANY_RANGE_HPP
#define ANY_RANGE_HPP

#include <functional>
#include <any>

#include "range.hpp"

namespace shake {

//----------------------------------------------------------------
// The any iterator is based on type erasure to remove the ugly template arguments,
// so that iterators of different types, that produce the same type of data can be used interchangeably.
// The type erasure is necessary in two places.
// First, the data should be type erased. This is achieved by using a std::any.
// Secondly, the functions operating on the type erased data, should somehow have some information
// about the underlying type, although that info should not be present in the signature.
// This is achieved by using std::functions that capture the type info internally.
template<typename data_t>
class AnyIterator
{
public:
    // iterator traits
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = data_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = data_t*;
    using reference         = data_t&;

public:
    // type erased functions to operate on type erased data
    struct TypeErasedFunctions
    {
        using F_dereference     = std::function<data_t(std::any&)>;
        using F_preincrement    = std::function<std::any&(std::any&)>;
        using F_equality        = std::function<bool(const std::any&, const std::any&)>;

        F_dereference   dereference;
        F_preincrement  preincrement;
        F_equality      equality;
    };

public:
    AnyIterator
    (
        const std::any&             wrapped_iterator, // the iterator is const, but the elements it refers to might not be!
        const TypeErasedFunctions&  type_erased_functions
    )
        : m_wrapped_iterator        { wrapped_iterator      }
        , m_type_erased_functions   { type_erased_functions }
    { }

    const std::any& get_internal_iterator() const { return m_wrapped_iterator; }

    AnyIterator& operator++()
    {
        // Simply call type-erased replacement
        m_type_erased_functions.preincrement( m_wrapped_iterator );
        return *this;
    }

    AnyIterator operator++(int)
    {
        AnyIterator result = *this;
        ++(*this);
        return result;
    }

    bool operator==( const AnyIterator& other ) const
    {
        // Simply call type-erased replacement
        return m_type_erased_functions.equality( get_internal_iterator(), other.get_internal_iterator() );
    }

    bool operator!=( const AnyIterator& other ) const
    {
        return !( *this == other );
    }

    data_t operator*()
    {
        // Simply call type-erased replacement
        return m_type_erased_functions.dereference( m_wrapped_iterator );
    }

public:
    std::any                m_wrapped_iterator;
    TypeErasedFunctions     m_type_erased_functions;
};

//----------------------------------------------------------------
template<typename T>
using AnyRange = Range<AnyIterator<T>>;

//----------------------------------------------------------------
template<typename iterator_t>
AnyRange<typename iterator_t::value_type> make_any_range
(
    Range<iterator_t> range
)
{
    // The type produced by the range depends on the internally captured iterator
    using data_t = iterator_t::value_type;

    // These type erased functions are to remember how to operate on the any's
    // without having to specify the underlying types in their signature.
    // The type iterator_t is erased from the signature by capturing it inside a function.
    const auto type_erased_functions = typename AnyIterator<data_t>::TypeErasedFunctions
    {
        // Dereference
        typename AnyIterator<data_t>::TypeErasedFunctions::F_dereference
        ( []( std::any& any ) -> data_t
        { return *( std::any_cast<iterator_t&>( any ) ); } ),

        // Preincrement
        typename AnyIterator<data_t>::TypeErasedFunctions::F_preincrement
        ( []( std::any& any ) -> std::any&
        { ++( std::any_cast<iterator_t&>( any ) ); return any; } ),

        // Equality
        typename AnyIterator<data_t>::TypeErasedFunctions::F_equality
        ( []( const std::any& lhs, const std::any& rhs ) -> bool
        { return ( std::any_cast<const iterator_t&>( lhs ) == std::any_cast<const iterator_t&>( rhs ) ); } )
    };

    return Range
    {
        AnyIterator<data_t> { std::begin(range),    type_erased_functions },
        AnyIterator<data_t> { std::end(range),      type_erased_functions }
    };
}

} // namespace shake

#endif // ANY_RANGE_HPP