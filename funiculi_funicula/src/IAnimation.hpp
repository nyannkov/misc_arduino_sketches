#ifndef IANIMATION_HPP
#define IANIMATION_HPP

#include <cstdint>
#include "render_matrix/LedMatrixBuffer.hpp"

struct IAnimation {
    virtual void initialize() = 0;
    virtual bool update(int32_t param, LedMatrixBuffer &buffer) = 0;
};


#endif/*IANIMATION_HPP*/
