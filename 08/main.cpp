
#include "my_vec_alloc.hpp"
#include "my_vec.hpp"
#include <iostream>
#include <cassert>
#include <utility>


int main()
{
    MyVec<int> v;
    v.push_back(10);
    v.emplace_back(12);
    MyVec<int> v1 = v;
    return 0;
}