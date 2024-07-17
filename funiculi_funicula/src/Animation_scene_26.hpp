#ifndef ANIMATION_SCENE_26_HPP
#define ANIMATION_SCENE_26_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_26 : IAnimation {
    Animation_scene_26() {
    }
    ~Animation_scene_26() = default;

    void initialize() override {
        counter_ = 0;
        current_param_ = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        if ( current_param_ != param ) {
            current_param_ = param;
        }

        if ( ++counter_ >= 8000 ) {
            counter_ = 0;

            if ( current_param_ <= 0 ) {
                spr_.set_bitmap_image(&image_funi);
                spr_.x = 0;
                spr_.y = 0;
            } else if ( current_param_ == 1 ) {
                spr_.set_bitmap_image(&image_culi);
                spr_.x = 0;
                spr_.y = 0;
            } else if ( current_param_ == 2 ) {
                spr_.set_bitmap_image(&image_cula);
                spr_.x = 0;
                spr_.y = 0;
            } else if ( current_param_ == 3 ) {
                spr_.set_bitmap_image(&image_smile);
                spr_.x = 4;
                spr_.y = 2;
            } else if ( current_param_ == 4 ) {
                spr_.set_bitmap_image(nullptr);
                spr_.x = 4;
                spr_.y = 2;
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

#endif/*ANIMATION_SCENE_26_HPP*/

