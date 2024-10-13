/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#include <Arduino_LED_Matrix.h>
#include <pwm.h>
#include <emu2149.h>
#include <Psgino.h>
#include "src/ThreeDigitDisplay.hpp"

namespace { 

constexpr uint32_t PSG_EMU_CLOCK = 2000000; // 2 MHz
constexpr uint32_t PSG_RATE      = 44100;   // 44.1 kHz
constexpr uint16_t MML_PROC_RATE = 400;     // 400 Hz

constexpr unsigned long EXEC_CYCLE_MML = (1000*1000uL)/MML_PROC_RATE; // unit: usec.
constexpr unsigned long EXEC_CYCLE_DIN = (10*1000uL); // unit: usec.

constexpr uint8_t PIN_PWM_OUT    = D8;
constexpr uint8_t PIN_DI_BOOST   = D7;
constexpr uint8_t PIN_DO_CLAP_0  = D6;
constexpr uint8_t PIN_DO_CLAP_1  = D5;
constexpr uint8_t PIN_DO_CLAP_2  = D4;
constexpr uint8_t PIN_DI_FINISH  = D3;
constexpr size_t NUM_OF_CLAPPER  = 3;

struct ClapInfo {
    bool clap_flag;
    bool palm_together_flag;
    int32_t palm_together_hold_counter;
    int32_t hold_count_num;
    const uint8_t pin;
} clap_info[NUM_OF_CLAPPER] = {
    [0] = { false, false, 0, 0, PIN_DO_CLAP_0 },
    [1] = { false, false, 0, 0, PIN_DO_CLAP_1 },
    [2] = { false, false, 0, 0, PIN_DO_CLAP_2 },
};

ThreeDigitDisplay three_digit_display;
ArduinoLEDMatrix matrix;
LedMatrixBuffer matrix_buffer;
bool update_display_flag;

PwmOut pwm_out(PIN_PWM_OUT);   // Sound output
PSG *psg;                      // PSG emulator
Psgino psgino;                 // MML decoder for PSGs
Psgino psgino_clap;            // MML decoder for PSGs

unsigned long time0_mml;
unsigned long time0_din;

IRQn_Type cycle_end_irq;

bool last_boost_button_state;
int boost_button_state_change_counter;
int16_t speed_factor;
bool finish_flag;
uint32_t cycle_counter_din = 0;

void cb_timer_overflow_irq(void) {

    if ( psg ) {
        int16_t output;
        output = PSG_calc(psg)>>5;// Adjust volume by shifting right.
        pwm_out.pulseWidth_raw(output);
    }

    R_BSP_IrqStatusClear(cycle_end_irq);
}

void psg_write(uint8_t addr, uint8_t data) {
    __disable_irq();
    PSG_writeReg(psg, addr, data);
    __enable_irq();
}

void mml_clap_callback(uint8_t ch, int32_t param) {
    (void)ch;
    int32_t clapper_id = 0;

    clapper_id = param&0x3;
    if ( clapper_id < static_cast<int32_t>(NUM_OF_CLAPPER) ) {

        clap_info[clapper_id].clap_flag = true;
        clap_info[clapper_id].hold_count_num = ((param&0x10) != 0) ? 20000 : 300;
    }
}

void mml_callback(uint8_t ch, int32_t param) {
    (void)ch;
    (void)param;
}

const char mml_clap[] =
    "T130"
    "R2"
    "R1" "R1" "R1" "R1"
    "[0"
        "[7@C0R4 @C0R4 @C0R4 @C0R4]"
    "|"
        "@C0R4 @C0R4 @C0R4 @C0R4"
    "]"
    "@C0R4 @C0R4 @C(0x10)R2"
    ","
    "T130"
    "R2"
    "R1" "R1" "R1" "R1"
    "[0"
        "[7 R8 @C1R16 R16 R8 @C1R8 R8 R16 @C1R16 R8 @C1R8]"
    "|"
        "R8 @C1R16 R16 R8 @C1R8 @C1R4 @C1R4"
    "]"
    "R8 @C1R16 R16 R8 @C1R8 @C(0x11)R2"
    ","
    "T130"
    "R2"
    "R1" "R1" "R1" "R1"
    "[0"
        "[7 R8 R16 @C2R16 R8 @C2R8 R8 @C2R16 R16 R8 @C2R8]"
    "|"
        "R8 R16 @C2R16 R8 @C2R8 @C2R4 @C2R4"
    "]"
    "R8 R16 @C2R16 R8 @C2R8 @C2R8"
    "R2.@C(0x12)R4"
    ;

/*
 * The Other Day I Met a Bear
 *
 * An American traditional camp song.
 *
 */
const char mml[] = 
    "T130" "V15$E1$A0$H80$D100$S85$F0"
    "O4L8 R8 GAB"
    "O4L4 >C<GEC"  "A2 R8 A8 B8 A8" "GFED"         "C2 R8G8 F#8 G8"
    "[0"
        "O4L8 E4 R4R8 ED#E"   "C4 R4R8 EDC"  "D4 R4R8 GAG" "E4 R4R8  GAB"
        "O4L4 >C<GEC"  "A2" 
    "|"
        "R8 A8 B8 A8" "GFED C2" "R8G8 F#8 G8"
    "]"
    "R8 A8 B8 A8" "GFED C2" "R2"
    ","
    "T130" "V13$E1$A0$H80$D100$S85$F0"
    "O4L8 R8 CEG"
    "O4L4 GEC<G>"  "F2 R8 F8 G8 F8" "E D C <B"     "G2>R4R4"
    "[0"
        "O4L8 R8 GF#G E4R4"   "R8 ED#EC4R4"  "R8 EDCD4R4"  "R8 GAGE  CEG"
        "O4L4 GEC<G>"  "F2 R8 F8 G8 F8" "E D C <B"     "G2>"
    "|"
        "R4R4"
    "]"
    "R2"
    ","
    "T130" "S0M5000"
    "O2L4R8 R8 G"
    "O2L4C<G>C<G>" "FCFC" "G<GAB>"  "C<G>C V12O7L64EGEGEGEGEGEGEGEGS0"
    "[0"
        "O2L8C>C<<G>>C O2C>C<<G>>C"
        "O2L8C>C<<G>>C O2C>C<<G>>C"
        "O2L8G>D<<G>>D O2G>D<<G>>D"
        "O2L8C>C<<G>>C O2C>C<<G>>C"
        "O2L8C>C<<G>>C O2C>C<<G>>C"
        "O2L8F>F<<F>>F O2F>F<<F>>F"
        "O2L8G>G<<G>>G O2G>G<<G>>G"
        "O2L8C>C<<G>>C"
    "|"
        "O2L8C>C<<G>>C"
    "]"
    "O2L8 C R8R4"
    ;
}

void setup() {

    // Setup DIO
    for ( auto &info : clap_info ) {
        info.clap_flag = false;
        info.palm_together_flag = false;
        info.palm_together_hold_counter = 0;
        info.hold_count_num = 300;
        pinMode(info.pin, OUTPUT);
        digitalWrite(info.pin, LOW);
    }

    pinMode(PIN_DI_BOOST, INPUT_PULLUP);
    speed_factor = 100;
    pinMode(PIN_DI_FINISH, INPUT_PULLUP);
    finish_flag = false;

    //  Setup PSG
    psg = PSG_new(PSG_EMU_CLOCK, PSG_RATE);
    PSG_reset(psg);

    psgino.Initialize(psg_write, PSG_EMU_CLOCK, MML_PROC_RATE);
    psgino.SetUserCallback(mml_callback);
    psgino.SetMML(mml);
    psgino.Play();

    psgino_clap.Initialize(nullptr, PSG_EMU_CLOCK, MML_PROC_RATE);
    psgino_clap.SetUserCallback(mml_clap_callback);
    psgino_clap.SetMML(mml_clap);
    psgino_clap.Play();

    FspTimer *timer;
    pwm_out.begin((float)PSG_RATE, 0.F);
    timer = pwm_out.get_timer();
    timer->setup_overflow_irq(12, cb_timer_overflow_irq);
    cycle_end_irq = timer->get_cfg()->cycle_end_irq;
    timer->enable_overflow_irq();

    //Setup LED MATRIX
    matrix.begin();
    three_digit_display.initialize();
    matrix_buffer = (LedMatrixBuffer){};
    update_display_flag = true;

    // Record the start time for measurement
    time0_mml = micros();
    time0_din = time0_mml;
}

void loop() {
    unsigned long time_now = micros();

    if ( (time_now - time0_mml) >= EXEC_CYCLE_MML ) {
        time0_mml = time_now;
        psgino.Proc();
        psgino_clap.Proc();
    }

    if ( (time_now - time0_din) >= EXEC_CYCLE_DIN ) {
        time0_din = time_now;
        cycle_counter_din++;

        bool boost_button_state = digitalRead(PIN_DI_BOOST);
        bool state_changed = false;

        if ( last_boost_button_state != boost_button_state ) {
            boost_button_state_change_counter++;
            if ( boost_button_state_change_counter > 0 ) {
                last_boost_button_state = boost_button_state;
                state_changed = true;
            }
        } else {
            boost_button_state_change_counter = 0;
        }

        if ( state_changed && (last_boost_button_state == LOW) ) {
            int16_t pitch_factor;
            speed_factor++;

            pitch_factor = (int32_t)(360 * (speed_factor - 100))/100;

            psgino.SetSpeedFactor(speed_factor);
            psgino.ShiftFrequency(pitch_factor);

            psgino_clap.SetSpeedFactor(speed_factor);
            //psgino_clap.ShiftFrequency(pitch_factor); // Not needed

            update_display_flag = true;
        }
        if ( finish_flag ) {
            int16_t pitch_factor;
            if ( (speed_factor > 100) && ((cycle_counter_din&0x1)==0) ) {
                speed_factor--;

                pitch_factor = (int32_t)(360 * (speed_factor - 100))/100;

                psgino.SetSpeedFactor(speed_factor);
                psgino.ShiftFrequency(pitch_factor);
                psgino_clap.SetSpeedFactor(speed_factor);
                //psgino_clap.ShiftFrequency(pitch_factor);

                update_display_flag = true;
            }
        }

        if ( !finish_flag && (digitalRead(PIN_DI_FINISH) == LOW ) ) {
            finish_flag = true;
            psgino.FinishPrimaryLoop();
            psgino_clap.FinishPrimaryLoop();
        }
    }

    for ( auto &info : clap_info ) {
        if ( info.clap_flag ) {
            info.clap_flag = false;
            digitalWrite(info.pin, HIGH);
            info.palm_together_flag = true;
            info.palm_together_hold_counter = info.hold_count_num;
        }

        if ( info.palm_together_flag ) {
            if ( info.palm_together_hold_counter == 0 ) {
                info.palm_together_flag = false;
                digitalWrite(info.pin, LOW);
            } else {
                info.palm_together_hold_counter--;
            }
        }
    }

    if ( update_display_flag ) {
        matrix_buffer = (LedMatrixBuffer){};
        three_digit_display.load_three_digit_image(
            matrix_buffer,
            speed_factor
        );
        matrix.renderBitmap(matrix_buffer.data, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH);
    }
}

