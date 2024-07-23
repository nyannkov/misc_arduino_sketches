/*
 * MIT License
 * (https://opensource.org/license/mit/)
 *
 * Copyright (c) 2024 nyannkov
 */
#include <cstdio>
#include <pwm.h>
#include <emu2149.h>
#include <Psgino.h>
#include <RTC.h>
#include <Arduino_LED_Matrix.h>
#include "src/DisplayClock.hpp"
#include "src/DisplayMomotaro.hpp"

namespace {

constexpr uint8_t PIN_PWM_OUT = D8;
constexpr uint8_t PIN_DO_CLAP = D7;
constexpr uint8_t PIN_DI_STOP = D6;

constexpr uint32_t PSG_EMU_CLOCK = 2000000; // 2 MHz
constexpr uint32_t PSG_RATE = 44100; // 44.1 kHz
constexpr uint16_t MML_PROC_RATE = 400;  // 400 Hz

constexpr unsigned long EXEC_CYCLE_MML = (1000*1000uL)/MML_PROC_RATE; // unit: usec.
constexpr unsigned long EXEC_CYCLE_RTC = (500*1000uL); // unit: usec.
constexpr unsigned long EXEC_CYCLE_DIN = (10*1000uL); // unit: usec.

DisplayClock display_clock;
DisplayMomotaro display_momotaro;
ArduinoLEDMatrix matrix;
LedMatrixBuffer matrix_buffer;

PwmOut pwm_out(PIN_PWM_OUT);  // Sound output
PSG *psg;                     // PSG emulator
Psgino psgino;                // MML decoder for PSGs
IRQn_Type cycle_end_irq;

RTCTime alarm_time;
RTCTime current_time;
AlarmMatch match_time;
int prev_minute;
bool start_alarm_flag;
bool stop_alarm_request_flag;
uint32_t snooze_exec_counter;
unsigned long snooze_wait_time;
enum class AlarmState {
    Stopped,
    SnoozeWaitingForAlarm,
    AlarmRinging,
} alarm_state;

bool last_stop_button_state;
int stop_button_state_change_counter;

bool clap_flag;
bool palm_together_flag;
uint32_t palm_together_hold_counter;

unsigned long time0_rtc;
unsigned long time0_mml;
unsigned long time0_din;
unsigned long time0_snz;

char mml_buffer[1024];
int32_t scene_id;
bool is_update_scene_id;
/*
 * Momotarou
 *
 * Music by Teiichi Okano
 */
const char * mml_format = 
    // CH A
    "T%u$B%d"
    "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    ">C16.C32<G8E16.E32A16.A32G16.G32E16.D32C8>C8<"
    "[0"
        "G8..A32G16.G32E8" "G16.G32E16.C32D4"
        "C16.C32D16.D32E16.E32D8" "E16.E32A16.A32G4"
        ">C16.C32<G8E16.E32A16.A32G16.G32E16.D32C4"
    "]"
    ","
    // CH B 
    "T%u$B%d"
    "L4O3 S0M5000"
    "@C1C16.C32G16.G32" "E16.E32F16.F32" "G16.G32E16.D32" "C16.<G32>@C-1C8"
    "[0"
        "@C2" "@C-1C16.G32<@C-1G16.>G32" "@C-1C16.G32<@C-1G16.>G32" "@C-1C16.G32<@C-1G16.>G32" "@C-1<G16.D32@C-1G16.>D32"
        "@C3" "@C-1C16.C32<@C-1G16.G32>" "@C-1C16.C32<@C-1G16.G32>" "@C-1C16.C32<@C-1F16.F32" "@C-1G16.G32@C-1A16.B32"
        "@C4" "@C-1>C16.C32@C-1G16.G32" "@C-1E16.E32@C-1F16.F32" "@C-1G16.G32@C-1E16.D32" "@C-1C16.<G32>@C-1C8"
    "]"
    ","
    // CH C
    "T%u$B%d"
    "V15L4O5"
    "$E1$A0$H80$D100$S85$F0"
    "G16.G32E8C16.C32F16.F32E16.E32C16.<B32G8>G8"
    "[0"
        "E8..F32E16.E32C8" "E16.E32C16.<G32B16.G32B16.>G32"
        "<G16.G32B16.B32>C16.C32<B8" ">C16.C32F16.F32E16.>G32E16.D32<"
        "G16.G32E8C16.C32F16.F32E16.E32C16.<B32G4>"
    "]"
;

void alarm_callback() {
    start_alarm_flag = true;
}

void cb_timer_overflow_irq(void) {

    if ( psg ) {
        int16_t output;
        float duty;
        output = PSG_calc(psg);
        duty = 100.0F*((float)output)/32768/2;// '/2' is for volume adjustment.
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
    if ( param >= 0 ) {
        scene_id = param;
        is_update_scene_id = true;
    } else {
        clap_flag = true;
    }
}

const char *get_momotaro_mml(uint16_t tempo, int16_t bias) {
    int rc = -1;
    rc = snprintf(mml_buffer, sizeof(mml_buffer), mml_format, tempo, bias, tempo, bias, tempo, bias);
    return (rc > 0) ? mml_buffer : nullptr;
}

const char *get_dissonance_momotaro_mml() {
    int rc = -1;
    rc = snprintf(mml_buffer, sizeof(mml_buffer), mml_format, 190, 300, 190, 310, 190, 290);
    return (rc > 0) ? mml_buffer : nullptr;
}

void start_alarm() {
    
    const char *mml = nullptr;

    display_momotaro.initialize();

    switch ( snooze_exec_counter ) {
    case 0:/*FALLTHROUGH*/
    case 1: 
        mml = get_momotaro_mml(80, 0);
        display_momotaro.set_speed(1);
        break;
    case 2:
        mml = get_momotaro_mml(90, 30);
        display_momotaro.set_speed(2);
        break;
    case 3:
        mml = get_momotaro_mml(100, 60);
        display_momotaro.set_speed(3);
        break;
    case 4:
        mml = get_momotaro_mml(110, 90);
        display_momotaro.set_speed(4);
        break;
    case 5:
        mml = get_momotaro_mml(120, 120);
        display_momotaro.set_speed(5);
        break;
    case 6:
        mml = get_momotaro_mml(130, 150);
        display_momotaro.set_speed(6);
        break;
    case 7:
        mml = get_momotaro_mml(150, 180);
        display_momotaro.set_speed(7);
        break;
    case 8:
        mml = get_momotaro_mml(160, 210);
        display_momotaro.set_speed(8);
        break;
    case 9:
        mml = get_momotaro_mml(170, 240);
        display_momotaro.set_speed(9);
        break;
    case 10:
        mml = get_momotaro_mml(180, 270);
        display_momotaro.set_speed(10);
        break;
    default:
        mml = get_dissonance_momotaro_mml();
        display_momotaro.set_speed(11);
        display_momotaro.enter_kibidango_mode();
        break;

    }
    psgino.SetMML(mml);
    psgino.Play();
}

}

void setup() {

    // Setup DIO
    pinMode(PIN_DO_CLAP, OUTPUT);
    digitalWrite(PIN_DO_CLAP, LOW);
    pinMode(PIN_DI_STOP, INPUT_PULLUP);
    last_stop_button_state = digitalRead(PIN_DI_STOP);
    stop_button_state_change_counter = 0;
    clap_flag = false;
    palm_together_flag = false;
    palm_together_hold_counter = 0;

    // Setup PSG.
    psg = PSG_new(PSG_EMU_CLOCK, PSG_RATE);
    PSG_reset(psg);

    psgino.Initialize(psg_write, PSG_EMU_CLOCK, MML_PROC_RATE);
    psgino.SetUserCallback(mml_callback);

    pwm_out.begin((float)PSG_RATE, 0.F);
    FspTimer *timer;
    timer = pwm_out.get_timer();
    timer->setup_overflow_irq(12, cb_timer_overflow_irq);
    cycle_end_irq = timer->get_cfg()->cycle_end_irq;
    timer->enable_overflow_irq();

    // Setup RTC
    RTC.begin();
    RTCTime startTime(7, Month::JULY, 2024, 6, 29, 50, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_ACTIVE);
    RTC.setTime(startTime);

    alarm_time.setSecond(0);
    alarm_time.setMinute(30);
    alarm_time.setHour(6);
    match_time.addMatchSecond();
    match_time.addMatchMinute();
    match_time.addMatchHour();
    RTC.setAlarmCallback(alarm_callback, alarm_time, match_time);
    start_alarm_flag = false;
    stop_alarm_request_flag = false;
    snooze_wait_time = 0;
    snooze_exec_counter = 0;
    scene_id = 0;
    is_update_scene_id = false;
    alarm_state = AlarmState::Stopped;

    // Setup display.
    matrix.begin();
    display_clock.initialize();
    display_momotaro.initialize();

    matrix_buffer = (LedMatrixBuffer){};
    RTC.getTime(current_time);
    display_clock.load_display_clock_image(matrix_buffer, current_time.getHour(), current_time.getMinutes());
    matrix.renderBitmap(matrix_buffer.data, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH);
    prev_minute = current_time.getMinutes();

    // Record the start time for measurement
    time0_mml = micros();
    time0_rtc = time0_mml;
    time0_din = time0_mml;
    time0_snz = time0_mml;
}

void loop() {

    unsigned long time_now = micros();
    bool should_display_latest_time = false;

    if ( (time_now - time0_mml) >= EXEC_CYCLE_MML ) {
        time0_mml = time_now;
        psgino.Proc();
    }

    if ( (time_now - time0_rtc) >= EXEC_CYCLE_RTC ) {
        time0_rtc = time_now;
        RTC.getTime(current_time);
        int minute = current_time.getMinutes();
        if ( prev_minute != minute ) {
            prev_minute = minute;
            should_display_latest_time = true;
        }
    }

    if ( (time_now - time0_din) >= EXEC_CYCLE_DIN ) {
        time0_din = time_now;

        bool stop_button_state = digitalRead(PIN_DI_STOP);
        bool state_changed = false;

        if ( last_stop_button_state != stop_button_state ) {
            stop_button_state_change_counter++;
            if ( stop_button_state_change_counter > 5 ) {
                last_stop_button_state = stop_button_state;
                state_changed = true;
            }
        } else {
            stop_button_state_change_counter = 0;
        }

        if ( state_changed && (last_stop_button_state == LOW) ) {
            stop_alarm_request_flag = true;
        }
    }


    if ( clap_flag ) {
        clap_flag = false;
        digitalWrite(PIN_DO_CLAP, HIGH);
        palm_together_flag = true;
        palm_together_hold_counter = 1000;
    }

    if ( palm_together_flag ) {
        if ( palm_together_hold_counter == 0 ) {
            palm_together_flag = false;
            digitalWrite(PIN_DO_CLAP, LOW);
        } else {
            palm_together_hold_counter--;
        }
    }

    __disable_irq();
    bool is_alarm_started = start_alarm_flag;
    start_alarm_flag = false;
    __enable_irq();

    bool is_stop_alarm_request = stop_alarm_request_flag;
    stop_alarm_request_flag = false;


    if ( alarm_state == AlarmState::SnoozeWaitingForAlarm ) {
        if ( (time_now - time0_snz) >= snooze_wait_time ) {
            is_alarm_started = true;
            snooze_exec_counter++;
            if ( snooze_exec_counter >= 100 ) {
                snooze_exec_counter = 100;
            }
        }
    }

    if ( alarm_state != AlarmState::AlarmRinging ) {
        if ( is_alarm_started ) {
            alarm_state = AlarmState::AlarmRinging;
            start_alarm();
        }
    } else {
        if ( is_stop_alarm_request ) {
            time0_snz = time_now;

            snooze_wait_time = ( snooze_exec_counter == 0 ) ? 2800000
                             : ( snooze_exec_counter == 1 ) ? 2100000
                             : ( snooze_exec_counter == 2 ) ? 1000000
                             : ( snooze_exec_counter == 3 ) ?  500000
                             : ( snooze_exec_counter == 4 ) ?  300000
                             : 0;

            if ( snooze_exec_counter <= 10 ) {
                alarm_state = AlarmState::SnoozeWaitingForAlarm;
                psgino.Stop();
            } else {
                // Rejected.
            }

            should_display_latest_time = true;
        }
    }

    if ( alarm_state != AlarmState::AlarmRinging ) {
        if ( should_display_latest_time ) {
            int hour = current_time.getHour();
            int minute = current_time.getMinutes();
            matrix_buffer = (LedMatrixBuffer){};
            display_clock.load_display_clock_image(matrix_buffer, hour, minute);
            matrix.renderBitmap(matrix_buffer.data, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH);
        }
    } else {
        if ( is_update_scene_id ) {
            is_update_scene_id = false;
            display_momotaro.set_animation(scene_id);
        }
        if ( display_momotaro.update_animation(matrix_buffer) ) {
            matrix.renderBitmap(matrix_buffer.data, LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH);
        }
    }
}
