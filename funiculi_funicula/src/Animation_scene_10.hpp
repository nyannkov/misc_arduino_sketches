#ifndef ANIMATION_SCENE_10_HPP
#define ANIMATION_SCENE_10_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_10 : IAnimation {
    Animation_scene_10() {
    }
    ~Animation_scene_10() = default;

    void initialize() override {
        counter_ = 0;

        spr_train_.set_bitmap_image(&image_train);
        spr_train_.x = -14;
        spr_train_.y = 9;

        spr_mountain_.set_bitmap_image(&image_mountain);
        spr_mountain_.x = 6;
        spr_mountain_.y = 5;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        (void)param;
        if ( ++counter_ >= 30000 ) {
            counter_ = 0;

            spr_train_.x +=2;
            spr_train_.y -=1;

            buffer = (LedMatrixBuffer){};
            spr_train_.render_led_matrix_buffer(buffer);
            spr_mountain_.render_led_matrix_buffer(buffer);
            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_train_;
    Sprite spr_mountain_;
    uint32_t counter_;
};

#endif/*ANIMATION_SCENE_10_HPP*/
