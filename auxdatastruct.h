#ifndef _AUX_DATA_STRUCT_H_
#define _AUX_DATA_STRUCT_H_

#include <algorithm>
#include <vector>
#include <initializer_list>
#include <assert.h>

// this class is not been tested. @meili
template<typename _T>
class Heap{
public:
    explicit Heap(const std::initializer_list<_T>&val)
    :vec_(val)
    {
        std::make_heap(vec_.begin(), vec_.end());
    }

    //
    int push_heap(const _T&val);
    bool empty() const;
    const _T&top() const;
    int pop_top();
private:
    std::vector<_T> vec_;
};


template<typename _T>
 int Heap<_T>::push_heap(const _T&val)
 {
     vec_.push_back(val);
     std::push_heap(vec_.begin(), vec_.end());
     return 0;
 }

template<typename _T>
const _T&Heap<_T>::top() const
{
    assert(vec_.size() > 0);
    return *vec_.begin();
}

template<typename _T>
int Heap<_T>::pop_top()
{
    assert(vec_.size() > 0);
    std::pop_heap(vec_.begin(), vec_.end());
    int popVal = vec_.back();
    vec_.pop_back();
    return popVal;
}

template<typename _T>
 bool Heap<_T>::empty() const
 {
     return vec_.empty();
 }

#endif