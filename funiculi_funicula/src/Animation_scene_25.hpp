#ifndef ANIMATION_SCENE_25_HPP
#define ANIMATION_SCENE_25_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_25 : IAnimation {
    Animation_scene_25() {
    }
    ~Animation_scene_25() = default;

    void initialize() override {
        counter_ = 0;
        current_param_ = 0;
        spr_.set_bitmap_image(&image_ncoppa_jammo_ja);
        spr_.x = -12;
        spr_.y = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        if ( current_param_ != param ) {
            current_param_ = param;
        }

        if ( ++counter_ >= 4000 ) {
            counter_ = 0;

            if ( current_param_ <= 0 ) {
                spr_.x = 0;
            } else if ( current_param_ == 1 ) {
                spr_.x = -6;
            } else if ( current_param_ == 2 ) {
                spr_.x = -16;
            } else if ( current_param_ == 3 ) {
                spr_.x = -32;
            } else if ( current_param_ == 4 ) {
                spr_.x = -40;
            } else if ( current_param_ == 5 ) {
                spr_.x = -48;
            } else {
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

#endif/*ANIMATION_SCENE_25_HPP*/
