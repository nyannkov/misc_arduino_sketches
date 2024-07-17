#include "Animations.hpp"
#include "Animation_scene_01.hpp"
#include "Animation_scene_02.hpp"
#include "Animation_scene_03.hpp"
#include "Animation_scene_04.hpp"
#include "Animation_scene_10.hpp"
#include "Animation_scene_22.hpp"
#include "Animation_scene_25.hpp"
#include "Animation_scene_26.hpp"


std::unique_ptr<IAnimation> get_animation(int32_t param) {

    std::unique_ptr<IAnimation> ret = nullptr;

    switch ( param ) {
    case -1:
        ret = std::make_unique<Animation_scene_01>();
        break;

    case -2:
        ret = std::make_unique<Animation_scene_02>();
        break;

    case -3:
        ret = std::make_unique<Animation_scene_03>();
        break;

    case -4:
        ret = std::make_unique<Animation_scene_04>();
        break;

    case -10:
        ret = std::make_unique<Animation_scene_10>();
        break;

    case -22:
        ret = std::make_unique<Animation_scene_22>();
        break;

    case -25:
        ret = std::make_unique<Animation_scene_25>();
        break;

    case -26:
        ret = std::make_unique<Animation_scene_26>();
        break;

    default:
        break;
    }

    return ret;
}

