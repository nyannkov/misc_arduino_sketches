#ifndef ANIMATION_SCENE_02_HPP
#define ANIMATION_SCENE_02_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"


struct Animation_scene_02 : IAnimation {
    Animation_scene_02() {
    }
    ~Animation_scene_02() = default;

    void initialize() override {
        counter_ = 0;
        current_param_ = 0;
        spr_funi_.set_bitmap_image(&image_funi_katakana);
        spr_funi_.x = 2;
        spr_funi_.y = 8;

        spr_culi_.set_bitmap_image(&image_culi_katakana);
        spr_culi_.x = 4;
        spr_culi_.y = 16;

        spr_cula_.set_bitmap_image(&image_cula_katakana);
        spr_cula_.x = 4;
        spr_cula_.y = 32;

        spr_funiculaaaa_.set_bitmap_image(&image_funiculaaaa_katakana);
        spr_funiculaaaa_.x = 12;
        spr_funiculaaaa_.y = 0;
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
                spr_funiculaaaa_.x-=2;
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
            spr_funiculaaaa_.render_led_matrix_buffer(buffer);
            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_funi_;
    Sprite spr_culi_;
    Sprite spr_cula_;
    Sprite spr_funiculaaaa_;
    uint32_t counter_;
    int32_t current_param_;
};

#endif/*ANIMATION_SCENE_02_HPP*/
