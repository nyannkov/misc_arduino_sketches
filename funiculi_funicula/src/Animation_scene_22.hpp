#ifndef ANIMATION_SCENE_22_HPP
#define ANIMATION_SCENE_22_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_22 : IAnimation {
    Animation_scene_22() {
    }
    ~Animation_scene_22() = default;

    void initialize() override {
        counter_ = 0;
        current_param_ = 0;
        spr_funi_.set_bitmap_image(&image_funi);
        spr_funi_.x = 0;
        spr_funi_.y = 8;

        spr_culi_.set_bitmap_image(&image_culi);
        spr_culi_.x = 0;
        spr_culi_.y = 16;

        spr_cula_.set_bitmap_image(&image_cula);
        spr_cula_.x = 0;
        spr_cula_.y = 32;

        spr_culaaaa_.set_bitmap_image(&image_culaaaa);
        spr_culaaaa_.x = 13;
        spr_culaaaa_.y = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        if ( current_param_ != param ) {
            current_param_ = param;
        }

        if ( ++counter_ >= 8000 ) {
            counter_ = 0;
            spr_funi_.y--;
            spr_culi_.y--;
            spr_cula_.y--;
            if ( current_param_ == 1 ) {
                spr_funi_.x -=2;
                spr_culi_.x -=2;
                spr_cula_.x -=2;
                spr_culaaaa_.x -=2;
            }

            if ( spr_funi_.y <= -8 ) {
                spr_funi_.y = 8;
            }
            if ( spr_culi_.y <= -16 ) {
                spr_culi_.y = 16;
            }
            if ( spr_cula_.y <= -16 ) {
                spr_cula_.y = 16;
            }

            buffer = (LedMatrixBuffer){};
            spr_funi_.render_led_matrix_buffer(buffer);
            spr_culi_.render_led_matrix_buffer(buffer);
            spr_cula_.render_led_matrix_buffer(buffer);
            spr_culaaaa_.render_led_matrix_buffer(buffer);
            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_funi_;
    Sprite spr_culi_;
    Sprite spr_cula_;
    Sprite spr_culaaaa_;
    uint32_t counter_;
    int32_t current_param_;
};

#endif/*ANIMATION_SCENE_22_HPP*/
