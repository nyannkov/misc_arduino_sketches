#ifndef ANIMATION_SCENE_01_HPP
#define ANIMATION_SCENE_01_HPP

#include "IAnimation.hpp"
#include "Resources.hpp"
#include "render_matrix/Sprite.hpp"

struct Animation_scene_01 : IAnimation {
    Animation_scene_01() {
    }
    ~Animation_scene_01() = default;

    void initialize() override {
        counter_ = 0;

        spr_jammo_1_.set_bitmap_image(&image_jammo_katakana);
        spr_jammo_1_.x = 12;
        spr_jammo_1_.y = 0;

        spr_jammo_2_.set_bitmap_image(&image_jammo_katakana);
        spr_jammo_2_.x = spr_jammo_1_.x + spr_jammo_1_.get_bitmap_image()->width + 4;
        spr_jammo_2_.y = 0;

        spr_ncoppa_.set_bitmap_image(&image_ncoppa_katakana);
        spr_ncoppa_.x = spr_jammo_2_.x + spr_jammo_2_.get_bitmap_image()->width + 4;
        spr_ncoppa_.y = 0;

        spr_jammo_3_.set_bitmap_image(&image_jammo_katakana);
        spr_jammo_3_.x = spr_ncoppa_.x + spr_ncoppa_.get_bitmap_image()->width + 4;
        spr_jammo_3_.y = 0;

        spr_ja_.set_bitmap_image(&image_ja_katakana);
        spr_ja_.x = spr_jammo_3_.x + spr_jammo_3_.get_bitmap_image()->width + 4;
        spr_ja_.y = 0;
    }

    bool update(int32_t param, LedMatrixBuffer &buffer) override {
        (void)param;
        if ( ++counter_ >= 3000 ) {
            counter_ = 0;

            spr_jammo_1_.x--;
            spr_jammo_2_.x--;
            spr_jammo_3_.x--;
            spr_ncoppa_.x--;
            spr_ja_.x--;

            buffer = (LedMatrixBuffer){};

            spr_jammo_1_.render_led_matrix_buffer(buffer);
            spr_jammo_2_.render_led_matrix_buffer(buffer);
            spr_jammo_3_.render_led_matrix_buffer(buffer);
            spr_ncoppa_.render_led_matrix_buffer(buffer);
            spr_ja_.render_led_matrix_buffer(buffer);

            return true;
        } else {
            return false;
        }
    }

private:
    Sprite spr_jammo_1_;
    Sprite spr_jammo_2_;
    Sprite spr_jammo_3_;
    Sprite spr_ncoppa_;
    Sprite spr_ja_;
    uint32_t counter_;
};

#endif/*ANIMATION_SCENE_01_HPP*/
