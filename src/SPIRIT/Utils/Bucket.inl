////////////////////////////////////////////////////////////
//
// Spirit Engine
// Copyright (C) 2022 Matthieu Beauchamp-Boulay
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////



#ifndef SPIRIT_ENGINE_BUCKET_INL
#define SPIRIT_ENGINE_BUCKET_INL


#include "include/SPIRIT/Utils/Bucket.hpp"
#include "SPIRIT/Core.hpp"


namespace sp
{

////////////////////////////////////////////////////////////
// Bucket
////////////////////////////////////////////////////////////


template< sp::Uint32 bucketSize, class ValueType >
Bucket< bucketSize, ValueType >::Bucket(const Bucket & other)
        : Bucket()
{
    this->operator=(other);
}


template< sp::Uint32 bucketSize, class ValueType >
Bucket< bucketSize, ValueType > &
Bucket< bucketSize, ValueType >::operator=(const Bucket & other)
{
    if ( this != & other )
    {
        for ( sp::Uint32 i = 0 ; i < bucketSize ; ++i )
            this->data()[i] = other.data()[i];

        this->last = other.last;
    }
    return * this;
}


////////////////////////////////////////////////////////////
// Capacity
////////////////////////////////////////////////////////////

template< sp::Uint32 bucketSize, class ValueType >
bool
Bucket< bucketSize, ValueType >::empty() const
{ return count() == 0; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::size_type
Bucket< bucketSize, ValueType >::size() const
{ return count(); }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::size_type
Bucket< bucketSize, ValueType >::max_size() const
{ return capacity(); }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::size_type
Bucket< bucketSize, ValueType >::capacity() const
{ return bucketSize; }


template< sp::Uint32 bucketSize, class ValueType >
bool
Bucket< bucketSize, ValueType >::isFull() const
{ return count() == bucketSize; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::size_type
Bucket< bucketSize, ValueType >::count() const
{ return last; }


////////////////////////////////////////////////////////////
// Element Access
////////////////////////////////////////////////////////////

template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::value_type &
Bucket< bucketSize, ValueType >::operator[](sp::Uint32 pos)
{ return this->data()[pos]; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::value_type const &
Bucket< bucketSize, ValueType >::operator[](sp::Uint32 pos) const
{ return this->data()[pos]; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::value_type &
Bucket< bucketSize, ValueType >::at(sp::Uint32 pos)
{
    SPIRIT_ASSERT(pos < count(), "element at {} is out of range, "
                                 "bucket size is {}", pos, count()
                 );
    return this->data()[pos];
}


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::value_type const &
Bucket< bucketSize, ValueType >::at(sp::Uint32 pos) const
{
    SPIRIT_ASSERT(pos < bucketSize, "element at {} is out of range, "
                                    "bucket size is {}", pos, bucketSize
                 );
    return this->data()[pos];
}


////////////////////////////////////////////////////////////
// Iterator helpers
////////////////////////////////////////////////////////////

template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::pointer
Bucket< bucketSize, ValueType >::firstEmpty()
{ return this->data() + last; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::const_pointer
Bucket< bucketSize, ValueType >::firstEmpty() const
{ return this->data() + last; }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::pointer
Bucket< bucketSize, ValueType >::firstValid()
{ return this->data(); }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::const_pointer
Bucket< bucketSize, ValueType >::firstValid() const
{ return this->data(); }


////////////////////////////////////////////////////////////
// Modifiers
////////////////////////////////////////////////////////////

template< sp::Uint32 bucketSize, class ValueType >
template< class... Args >
typename Bucket< bucketSize, ValueType >::pointer
Bucket< bucketSize, ValueType >::construct(Bucket::pointer where,
                                           Args && ... args
                                          )
{ return new(where) value_type(std::forward< Args >(args)...); }


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::destroy(Bucket::pointer where)
{ where->~value_type(); }


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::moveRight(Bucket::pointer start,
                                           Bucket::pointer finish,
                                           sp::Int32 offset
                                          )
{
    SPIRIT_ASSERT(offset >= 0 && finish >= start, "Invalid parameters");
    if ( offset == 0)
        return;

    while ( finish != start )
    {
        --finish;
        // When we are storing const elements, we cannot reassign...
        // std::swap(*(finish + offset), *finish);
        construct(finish + offset, std::move(* finish));
    }
}


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::moveLeft(Bucket::pointer start,
                                          Bucket::pointer finish,
                                          sp::Int32 offset
                                         )
{
    SPIRIT_ASSERT(offset >= 0 && finish >= start,
                  "Invalid parameters");
    if ( offset == 0)
        return;

    while ( start != finish )
    {
        // When we are storing const elements, we cannot reassign...
        construct(start - offset, std::move(* start));
        ++start;
    }
}


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::clear()
{ while ( last != 0 ) pop_back(); }


template< sp::Uint32 bucketSize, class ValueType >
template< class... Args >
void
Bucket< bucketSize, ValueType >::emplace_back(Args && ... args)
{
    if ( this->isFull() )
        return;
    construct(& this->data()[last++], std::forward< Args >(args)...);
}


template< sp::Uint32 bucketSize, class ValueType >
template< class... Args >
typename Bucket< bucketSize, ValueType >::iterator
Bucket< bucketSize, ValueType >::emplace(Bucket::const_iterator pos,
                                         Args && ... args
                                        )
{
    if ( this->isFull() )
        return this->end();

    iterator location{(pointer) pos.ptr};
    if ( location.ptr < & this->data()[last] )
        moveRight(location.ptr, & this->data()[last], 1);

    construct(location.ptr, std::forward< Args >(args)...);
    ++last;
    return location;
}


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::iterator
Bucket< bucketSize, ValueType >::insert(Bucket::const_iterator pos,
                                        const value_type & value
                                       )
{ return emplace(pos, value); }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::iterator
Bucket< bucketSize, ValueType >::insert(Bucket::const_iterator pos,
                                        value_type && value
                                       )
{ return emplace(pos, std::move(value)); }


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::size_type
Bucket< bucketSize, ValueType >::insert(Bucket::const_iterator pos,
                                        Bucket::size_type count,
                                        const value_type & value
                                       )
{
    size_type nCopies = std::min(count, this->max_size() - this->size());

    iterator location{(pointer) pos.ptr};
    if ( location.ptr < & this->data()[last] )
        moveRight(location.ptr, & this->data()[last], nCopies);

    for ( size_type _ = 0 ; _ < nCopies ; ++_ )
        construct((location++).ptr, value);

    last += nCopies;
    return nCopies;
}


template< sp::Uint32 bucketSize, class ValueType >
template< class Iter, class Requirement >
Iter
Bucket< bucketSize, ValueType >::insert(Bucket::const_iterator pos,
                                        Iter start,
                                        Iter finish
                                       )
{
    size_type count = std::distance(start, finish);
    size_type nCopies = std::min(count, this->max_size() - this->size());

    iterator location{(pointer) pos.ptr};
    if ( location.ptr < & this->data()[last] )
        moveRight(location.ptr, & this->data()[last], nCopies);

    for ( size_type _ = 0 ; _ < nCopies ; ++_ )
        construct((location++).ptr, * (start++));

    last += nCopies;
    return start;
}


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::iterator
Bucket< bucketSize, ValueType >::erase(Bucket::const_iterator pos)
{
    iterator location{(pointer) pos.ptr};

    destroy(location.ptr);
    moveLeft(location.ptr + 1, & this->data()[last--], 1);
    return location;
}


template< sp::Uint32 bucketSize, class ValueType >
typename Bucket< bucketSize, ValueType >::iterator
Bucket< bucketSize, ValueType >::erase(Bucket::const_iterator start,
                                       Bucket::const_iterator finish
                                      )
{
    size_type count = std::distance(start, finish);
    count = std::min(count, bucketSize);

    iterator location{(pointer) start.ptr};

    for ( size_type _ = 0 ; _ < count ; ++_ )
        destroy((location++).ptr);

    moveLeft(location.ptr, & this->data()[last], count);
    last -= count;
    return location;
}


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::push_back(const value_type & value)
{ emplace_back(value); }


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::push_back(value_type && value)
{ emplace_back(std::move(value)); }


template< sp::Uint32 bucketSize, class ValueType >
void
Bucket< bucketSize, ValueType >::pop_back()
{
    if ( this->empty() )
        return;
    destroy(& this->data()[--last]);
}


} // namespace sp


#endif //SPIRIT_ENGINE_BUCKET_INL
