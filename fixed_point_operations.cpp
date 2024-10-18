#include "fixed_point_operations.hpp"
#include <iostream>
#include <cassert>

namespace fp {

tGLfixed fabs(const tGLfixed& val) {
#ifdef SHOW_WARNINGS_FOR_PROPER_IMPLEMENTATION
    std::cout<<"implement properly fabs for fixed point";
#endif
    tGLfixed result(val);
    if (val<0) {
        result = -1*val;
    }
    return result;
}

tGLfixed multiply(const tGLfixed& a, const tGLfixed& b)
{
#ifdef SHOW_WARNINGS_FOR_PROPER_IMPLEMENTATION
    std::cout<<"implement properly operator*= for fixed point";
#endif
    tGLfixed_acc acc(a);
    acc *= b;
    return acc;
}

tGLfixed operator*(const tGLfixed& a, const tGLfixed& b)
{
#ifdef SHOW_WARNINGS_FOR_PROPER_IMPLEMENTATION
    std::cout<<"implement properly operator* for fixed point";
#endif
//    std::cout<<"overloaded *"<<std::endl;
    tGLfixed_acc acc(a);
    acc *= b;
//    std::cout<<"acc="<<acc<<std::endl;
    return acc;
}

} // namespace fp

