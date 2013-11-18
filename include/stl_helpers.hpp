#ifndef STL_HELPERS_HPP
#define STL_HELPERS_HPP

namespace zbroker { namespace stl {

template<class Map>
inline bool contains(const Map& mapping,
                     const typename Map::key_type& key) {
    typename Map::const_iterator i = mapping.find(key);
    return i != mapping.end();
}

template<class Map>
inline const typename Map::mapped_type find(const Map& mapping,
                    const typename Map::key_type& key,
                    const typename Map::mapped_type& defaultValue) {
    typename Map::const_iterator i = mapping.find(key);
    if(i == mapping.end()) {
        return defaultValue;
    }
    return i->second;
}

template<class Map>
inline const typename Map::mapped_type find(const Map& mapping,
                                           const typename Map::key_type& key) {
    typename Map::mapped_type defaultValue;
    return find(mapping, key, defaultValue);
}


} //stl
} //zbroker

#endif // STL_HELPERS_HPP
