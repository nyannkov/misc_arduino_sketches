#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#include <memory>
#include "IAnimation.hpp"

extern std::unique_ptr<IAnimation> get_animation(int32_t param);


#endif/*ANIMATIONS_HPP*/
