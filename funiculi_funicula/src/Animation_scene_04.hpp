#ifndef ANIMATION_SCENE_04_HPP
#define ANIMATION_SCENE_04_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_04 : IAnimation {
    Animation_scene_04() {
    }
    ~Animation_scene_04() = default;

    void initialize() override {
        counter_ = 0;
        current_param_ = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        if ( current_param_ != param ) {
            current_param_ = param;
        }

        if ( ++counter_ >= 6000 ) {
            counter_ = 0;

            if ( current_param_ <= 0 ) {
                spr_.set_bitmap_image(&image_funi_katakana);
                spr_.x = 2;
                spr_.y = 0;
            } else if ( current_param_ == 1 ) {
                spr_.set_bitmap_image(&image_culi_katakana);
                spr_.x = 4;
                spr_.y = 0;
            } else if ( current_param_ == 2 ) {
                spr_.set_bitmap_image(&image_cula_katakana);
                spr_.x = 4;
                spr_.y = 0;
            } else {
                // Fail-safe
                spr_.set_bitmap_image(&image_funi_katakana);
                spr_.x = 2;
                spr_.y = 0;
            }
            buffer = (LedMatrixBuffer){};
            spr_.render_led_matrix_buffer(buffer);
            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_;
    uint32_t counter_;
    int32_t current_param_;
};

#endif/*ANIMATION_SCENE_04_HPP*/
