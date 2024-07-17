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
#include "src/Animations.hpp"

namespace { 

/*
 * Funiculì funiculà
 *
 * Music by Luigi Denza
 */
const char mml[] = 
    // CH A
    "T136"
    "V15L4O5"
    "$M1$J12$L90$T4" "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    "Q7"
    "@C-1" "BA8R8BA8R8 >C8.<B16A8.>C16<B2"
    "@C-1" "BA8R8BA8R8 >C8.<B16A8.>C16<G8."
    "@C-2" "[3" "E16E8R16E16E8.]"
    "@C1"  "E16E8R16E16Q8>C2"
    "@C-3" "Q7D8.C16<B8.A16G8."
    "@C-4" "E16E8."
    "@C1"  "F16G8."
    "@C0"  "F16E8."
    "@C2"  "D16Q8C2"

    "V15L4O5"
    "$M1$J12$L90$T4" "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    "Q7"
    "@C-10" "BA8R8BA8R8 >C8.<B16A8.>C16<B2"
    "BA8R8BA8R8 >C8.<B16A8.>C16<G8."
    "@C-22" "[3" "E16E8R16E16E8.]"
    "@C1"  "E16E8R16E16Q8>C2"
    "@C-25" "Q7D8."
    "@C2" "C16"
    "@C3" "<B8."
    "@C4" "A16"
    "@C5" "G8."

    "@C-26" "E16E8."
    "@C1"  "F16G8."
    "@C0"  "F16E8."
    "@C2"  "D16C4>"
    "@C4"  "$F500C4"
    ","
    // CH B
    "T136"
    "L4O3 S0M5000I0"
    "[7" "G8.G16M2000H8.M5000D16]"
    "C8.C16M2000H8.M5000C16<"
    "B8.B16M2000H8.M5000B16"
    "A8.A16M2000H8.M5000A16"
    "G#8.G#16M2000H8.M5000G#16"
    "A8.A16M2000H8.M5000A16"
    "F8.F16M2000H8.M5000F16"
    "G8.G16M2000H8.M5000G16"
    "G8.G16M2000H8.M5000G16"
    "C8.C16M2000H8.M4000C16"

    "L4O3 S0M5000I0"
    "[7" "G8.G16M2000H8.M5000D16]"
    "C8.C16M2000H8.M5000C16<"
    "B8.B16M2000H8.M5000B16"
    "A8.A16M2000H8.M5000A16"
    "G#8.G#16M2000H8.M5000G#16"
    "A8.A16M2000H8.M5000A16"
    "F8.F16M2000H8.M5000F16"
    "G8.G16M2000H8.M5000G16"
    "G8.G16M2000H8.M5000G16"
    "C8.C16>C4"
    ","
    // CH C
    "T136"
    "V15L4O5"
    "$M1$J12$L90$T4" "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    "Q7"
    "GF8R8GF8R8 A8.G16F8.A16G2"
    "GF8R8GF8R8 A8.G16F8.A16E8."
    "<G16G8R16G16G#8."
    "G#16G#8R16G#16A8."
    "A16A8R16A16B8."
    "B16B8R16B16>E2"
    "Q7F8.E16D8.C16<B8."
    "G16G8.A16B8.A16G8.F16Q8E2"

    "V15L4O5"
    "$M1$J12$L100$T4" "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    "Q7"
    "GF8R8GF8R8 A8.G16F8.A16G2"
    "GF8R8GF8R8 A8.G16F8.A16E8."
    "<G16G8R16G16G#8."
    "G#16G#8R16G#16A8."
    "A16A8R16A16B8."
    "B16B8R16B16>E2"
    "Q7F8.E16D8.C16<B8."
    "G16G8.A16B8.A16G8.F16E4>$F500E4"
;

constexpr uint32_t PSG_EMU_CLOCK = 2000000; // 2 MHz
constexpr uint32_t PSG_RATE = 20000; // 20 kHz
constexpr uint16_t MML_PROC_RATE = 400;  // 400 Hz
constexpr unsigned long EXEC_CYCLE_MML = (1000*1000uL)/MML_PROC_RATE; // unit: usec.

PwmOut pwm_out(D8);         // Sound output
PSG *psg;                   // PSG emulator
Psgino psgino;              // MML decoder for PSGs

unsigned long time0;
IRQn_Type cycle_end_irq;

ArduinoLEDMatrix matrix;
LedMatrixBuffer matrix_buf;
std::unique_ptr<IAnimation> animation = nullptr;
int32_t animation_param;
bool is_update_param;

void cb_timer_overflow_irq(void) {

    if ( psg ) {
        int16_t output;
        float duty;
        output = PSG_calc(psg)>>1; // Adjust volume.
        duty = 100.0F*((float)output)/32768;
        pwm_out.pulse_perc(duty);
    }

    R_BSP_IrqStatusClear(cycle_end_irq);
}

void psg_write(uint8_t addr, uint8_t data) {
    __disable_irq();
    PSG_writeReg(psg, addr, data);
    __enable_irq();
}

void mml_callback(uint8_t ch, int32_t param) {
    (void)ch;
    animation_param = param;
    is_update_param = true;
}

}

void setup() {

    //  Setup PSG
    FspTimer *timer;

    psg = PSG_new(PSG_EMU_CLOCK, PSG_RATE);
    PSG_reset(psg);

    psgino.Initialize(psg_write, PSG_EMU_CLOCK, MML_PROC_RATE);
    psgino.SetUserCallback(mml_callback);
    psgino.SetMML(mml);
    psgino.Play();

    pwm_out.begin((float)PSG_RATE, 0.F);
    timer = pwm_out.get_timer();
    timer->setup_overflow_irq(12, cb_timer_overflow_irq);
    cycle_end_irq = timer->get_cfg()->cycle_end_irq;
    timer->enable_overflow_irq();

    // Setup LED MATRIX
    matrix.begin();

    // Record the start time for measurement
    time0 = micros();
}

void loop() {
    unsigned long time_now = micros();

    if ( (time_now - time0) >= EXEC_CYCLE_MML ) {
        time0 = time_now;
        psgino.Proc();
    }

    if ( is_update_param ) {
        is_update_param = false;
        if ( animation_param < 0 ) {
            animation = get_animation(animation_param);
            if ( animation ) {
                animation->initialize();
            }
        }
    }

    if ( animation && animation->update(animation_param, matrix_buf) ) {
        matrix.renderBitmap(matrix_buf.data, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH);
    }
}

