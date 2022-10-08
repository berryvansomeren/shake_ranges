# shake_ranges

This an older project of mine dating back to 2018/2019.
I saw how powerful the loop structures and weak typing in Python are, 
and wanted to mimic those in c++ with custom ranges and iterators, using templates and type erasure.

The types of ranges supported and related python concepts are:
	
| Shake Range     | Similar Python Concept        |
|-----------------|-------------------------------|
| index range     | range()                       |
| step range      | list slicing with step size   |
| combine range   | zip()                         |
| enumerate range | enumerate()                   |
| map range       | dict.keys() and dict.values() |
| transform range | generators                    |
| any range       | weak typing                   |

For minimal usage examples and comparisons to Python equivalents, see below.
For more complete usage examples you could take a look at _unit_tests.hpp_ 
or run the code online **[on Wandbox](https://wandbox.org/permlink/IAtDx7DerlCof5dc)**

## index ranges

A python loop over integers or indices like the following:
```python3
for i in range(10):
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& i : range( 10 ) )
{
    ...
}
```

## step ranges

A python loop stepping through a list like the following:
```python3
for v in l[::5]:
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& v : step( l, 5 ) )
{
    ...
}
```

## combine ranges

A python loop zipping two list like the following:
```python3
for v1, v2 in zip(l1, l2):
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& [ v1, v2 ] : combine( l1, l2 ) )
{
    ...
}
```

## enumerate ranges

A python loop stepping through a list like the following:
```python3
for i, v in enumerate(l):
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& [ i, v ] : enumerate( l ) )
{
    ...
}
```

## map ranges

A python loop over the keys of a dict like the following:
```python3
for k in d.keys():
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& key : keys( map ) )
{
    ...
}
```

A python loop over the values of a dict like the following:
```python3
for k in d.values():
    ...
```

Can be mimicked in c++ using the following:
```cpp
for ( const auto& key : values( map ) )
{
    ...
}
```


## transform ranges

A python generator like the following:
```python3

def int_to_string_generator(l):
    for i in l:
        yield str(i)

for s in int_to_string_generator(range(10)):
    ...
```

Can be mimicked in c++ using the following:
```cpp
std::string int_to_string( const int& i )
{
    return std::to_string(i);
}

const auto generator_range = transform<const int&, std::string>( range( 10 ), int_to_string )
for ( const std::string& s : generator_range )
{
    ...
}
```

## any ranges

Because python is a weakly typed language it is easy to only specify partial requirements on types.
On the other hand, c++ is strongly typed but offers parametric polymorphism through templates.
Passing around the ranges from the previous examples can be cumbersome if the user needs to use template code a lot.

To alleviate template pain we can apply a technique called **type erasure** to remove or hide type constraints on our types.
_AnyRange_ uses type erasure to hide the internal types of a Range, and reduces its type to only specify the value_type of the Range.
Or, in simpler terms, **with _AnyRange_ we only need to specify the type of what our range produces, and not what went inside**.

_AnyRange_ enables a developer to pass around all different kinds of ranges with minimal template usage. 
I believe that the combination with _AnyRange_ makes all other ranges in this library significantly more useful.

An example is given below, that reuses _generator_range_ from the previous example.
Using _AnyRange_, the function _consume_any_string_range_ is made generic, without using the template keyword.
It can now accept any range that produces strings, as input.
By using _make_any_range_, the type of _generator_range_ is reduced from _Range< TransformIterator< const int&, std::string, IndexIterator > >_
to _AnyRange< std::string >_.


```cpp
void consume_any_string_range( AnyRange<std::string> string_range )
{
    for ( const std::string& s : string_range ) 
    {
        ...
    }
}

auto any_range = make_any_range(generator_range);
consume_any_string_range( any_range );
```



