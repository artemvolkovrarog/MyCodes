#pragma once
#include <iostream>
#include "my_vec.hpp"
#include "my_vec_alloc.hpp" 

template <class T>
class Iterator{
    T* ptr;

    Iterator(const T* & p){
        ptr = p;
    }

    T operator *(){
        return *ptr;
    }

    Iterator<T> & operator ++(){
        ++ptr;
        return (*this);
    }

    Iterator<T> & operator --(){
        --ptr;
        return (*this);
    }

    bool operator ==(const  Iterator<T>& it){
        return (ptr == it.ptr);
    }

    bool operator !=(const Iterator<T>& it){
        return (ptr != it.ptr);
    }
    
};

template <class T>
class ReversedIterator{
    T* ptr;

    ReversedIterator(const T* & p){
        ptr = p;
    }

    T operator *(){
        return *ptr;
    }

    ReversedIterator<T> & operator ++(){
        --ptr;
        return (*this);
    }

    ReversedIterator<T> & operator --(){
        ++ptr;
        return (*this);
    }

    bool operator ==(const  ReversedIterator<T>& it){
        return (ptr == it.ptr);
    }

    bool operator !=(const ReversedIterator<T>& it){
        return (ptr != it.ptr);
    }
    
};