/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#include <misakiUTF16.h>
#include "Encoding.hpp"

bool load_image_of_font8x8(
    uint8_t *bmp_data,
    uint16_t bmp_width,
    uint16_t bmp_height,
    BitmapImage::DotType dot_type,
    const char *string_utf8
) {

    uint16_t x_ofs = 0;
    uint16_t char_utf16 = 0;

    if ( ( bmp_data == nullptr ) ||
         ( bmp_width == 0 )
    ) {
        return false;
    }

    if ( ( dot_type == BitmapImage::DotType::Bit ) &&
         ( (bmp_width % 8) != 0 )
    ) {
        return false;
    }

    bmp_height = ( bmp_height < 8 ) ? bmp_height : 8;

    byte len = charUFT8toUTF16(&char_utf16, (char*)string_utf8);
    if ( char_utf16 == 0 ) {
        return false;
    }

    while ( len > 0 ) {
        uint8_t glyph[8] = {0};
        getFontDataByUTF16(glyph, char_utf16);

        uint16_t glyph_width = isZenkaku(char_utf16) ? 8 : 4;

        if ( (x_ofs + glyph_width) >= bmp_width ) {
            return false;
        }
        for ( uint16_t i = 0; i < bmp_height; i++ ) {
            for ( uint16_t j = 0; j < glyph_width; j++ ) {
                if (glyph[i]&(0x80>>j)) {
                    if ( dot_type == BitmapImage::DotType::Bit ) {

                        size_t index = ((x_ofs+j)>>3) + i*(bmp_width>>3);
                        bmp_data[index] |= 1 << ( 7-((x_ofs+j)%8) );

                    } else if ( dot_type == BitmapImage::DotType::Byte ) {

                        bmp_data[i*bmp_width + j + x_ofs] = 1;

                    } else {

                        return false;
                    }
                }
            }
        }
        x_ofs += glyph_width;
        string_utf8 = &string_utf8[len];

        char_utf16 = 0;
        len = charUFT8toUTF16(&char_utf16, (char*)string_utf8);
        if ( char_utf16 == 0 ) {
            break;
        }
    }
    return true;
}

