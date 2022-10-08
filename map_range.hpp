#ifndef MAP_RANGE_HPP
#define MAP_RANGE_HPP

#include <cstdint>
#include <iterator>
#include <map>

#include "range.hpp"
#include "transform_range.hpp"

namespace shake {

//----------------------------------------------------------------
template<typename map_t>
auto keys( const map_t& map )
{
    return transform< const typename map_t::value_type&, const typename map_t::key_type >
    ( 
        const_range( map ),
        []( const typename map_t::value_type& v )
        {
            return v.first;
        }
    );
}

//----------------------------------------------------------------
template<typename map_t>
auto values( const map_t& map )
{
    return transform< const typename map_t::value_type&, const typename map_t::mapped_type >
    ( 
        const_range( map ),
        []( const typename map_t::value_type& v )
        {
            return v.second;
        }
    );
}


} // namespace shake

#endif // MAP_RANGE_HPP