/* =============================================================================
 * lab4_starter.c
 * ENCM 511, Fall 2026  --  Assignment 4 (Driver Project 4)
 * Motor Position Control: PWM, Timers, and Interrupts
 *
 * Target: PIC24FJ256GA702, 28-pin DIP
 *
 * WIRING (see lab schematic)
 *
 *   ENCODER  (Part 2 in-lab only)
 *     Orange (5V)  -> +5V rail
 *     Brown  (GND) -> GND rail
 *     Blue   (Ch A)-> Pin 16 (RB7)   IOC source, 5V-tolerant
 *     Yellow (Ch B)-> Pin 17 (RB8)   direction,  5V-tolerant
 *
 *   L298N  (Part 2 in-lab only)
 *     IN3  -> Pin 21 (RB10)    IN4  -> Pin 22 (RB11)
 *     ENB  -> Pin 24 (RB13)    *** Remove ENB jumper before wiring ***
 *
 *   PRELAB LED (remove before Part 2)
 *     Pin 24 (RB13) -> 330 Ohm -> LED -> GND
 *
 * CLOCKING: FRC, FOSC = 8 MHz, FCY = 4 MHz, TCY = 250 ns
 * =============================================================================
 */

#include <xc.h>
#include <stdint.h>
#include "uart.h"
#include <stdio.h>


#pragma config FNOSC=FRC, POSCMD=NONE, OSCIOFCN=ON, FCKSM=CSDCMD
#pragma config IOL1WAY=OFF, FWDTEN=OFF, JTAGEN=OFF

#define FCY            4000000UL
#define COUNTS_PER_REV 2048
#define KP_Q8          2500        // actual gain = KP_Q8 / 256 
#define PWM_MAX        399
#define PWM_MIN        10
#define REFERENCE      256

#define PWM_DUTY      50        // initial duty cycle in %
#define PWM_FREQ      10000UL    // PWM frequency in Hz
#define PWM_PERIOD(freq) (FCY / freq)
#define PWM_DUTY_CYCLE(p) (p * FCY / ((PWM_FREQ) * 100)) // This assumes the PWM will be give in % (50% = 50)

#define LED LATBbits.LATB5
#define IN3 LATBbits.LATB10
#define IN4 LATBbits.LATB11

#define CHA PORTBbits.RB7
#define CHB PORTBbits.RB8

/* encoder_count: updated by _IOCInterrupt on every rising edge of Ch A.
 * volatile -- can change at any time from an interrupt.
 * int32_t  -- signed so CW (+) and CCW (-) are both represented.
 * Reading this 32-bit variable requires special care -- see run_controller(). */
volatile int32_t encoder_count = 0;

// control_flag: set every 10 ms by _T2Interrupt, cleared by main.
volatile uint8_t control_flag  = 0;
volatile uint8_t t2_count = 0;  // counts the number of times _T2Interrupt has fired

/* Function prototypes */
void clock_init(void);
void gpio_init(void);
void encoder_init(void);
void timer_init(void);
void pwm_init(void);
void set_motor_pwm(int32_t control);
void run_controller(void);
void send_controller_data(int32_t reference, int32_t encoder_count, int32_t u);
uint32_t abs(int32_t value);


/* clock_init: configure the device to run at FCY = 4 MHz.
 * Datasheet ref: Section 9.1, Register 9-2 (CLKDIV). */
void clock_init(void) {
    CLKDIVbits.RCDIV = 0;   // Change this to run at 4 MHz (CLK divides by 1)
}


/* gpio_init: configure all digital I/O pins used in this assignment.
 * Input Pins: RB7(encoder CH A), RB8 (encoder CH B)
 * Output Pins: RB5(LED), RB10, RB11, RB13
 * Note: RB13 is configured in pwm_init() -- do not touch it here. */
void gpio_init(void) {
    // your code here 

    ANSELBbits.ANSB13; // RB13 as digital output
    ANSBbits.ANSB13 = 0;

    TRISBbits.TRISB7 = 1; // RB7 as input (Ch A)
    TRISBbits.TRISB8 = 1; // RB8 as input (Ch B)

    TRISBbits.TRISB5 = 0; // RB5 as output (LED)
    TRISBbits.TRISB10 = 0; // RB10 as output (IN3)
    TRISBbits.TRISB11 = 0; // RB11 as output (IN4)

    LATBbits.LATB5 = 0; // Initialize LED to low
    LATBbits.LATB10 = 0; // Initialize IN3 to low
    LATBbits.LATB11 = 0; // Initialize IN4 to low
    
}


/* encoder_init: configure the IOC interrupt on channel A (RB7).
 * The ISR reads the live RB7 state to keep only rising edges.
 * Datasheet ref: Section 11.3. */
void encoder_init(void) {
    PADCONbits.IOCON   = 0;     // disable IOC while configuring -- DO NOT CHANGE 

    IOCPBbits.IOCPB7   = 1;     // enable rising edge on RB7  
    IOCNBbits.IOCNB7   = 0;     // disable falling edge on RB7  

    IOCFB              = 0;     // DO NOT CHANGE 
    IOCSTATbits.IOCPBF = 0;     // DO NOT CHANGE 
    PADCONbits.IOCON   = 1;     // DO NOT CHANGE 
    IFS1bits.IOCIF     = 0;     // DO NOT CHANGE 

    IPC4bits.IOCIP     = 3;     // IOC priority  
    IEC1bits.IOCIE     = 1;     // IOC interrupt enable 
}


/* timer_init: configure Timer2 to interrupt at 100 Hz (every 10 ms).
 * All lines are present -- set each value correctly.
 * Datasheet ref: Section 13, Register 13-1 (T2CON).
 * MUST be called last in main() -- see main() comment. */
void timer_init(void) {
    T2CON = 0;                  // DO NOT CHANGE 
    TMR2  = 0;                  // DO NOT CHANGE 

    PR2               = 625;    // period register - counts = FCY / (prescaler * f_desired) = 4 MHz / (64 * 100 Hz) = 625
    T2CONbits.TCKPS   = 2;      // prescaler select - 1:64
    T2CONbits.TCS     = 0;      // clock source     - FOSC/2
    T2CONbits.T32     = 0;      // timer width - 16 bits

    IFS0bits.T2IF     = 0;      // Clear interrupt flag
    IPC1bits.T2IP     = 2;      // Timer2 priority    
    IEC0bits.T2IE     = 1;      // interrupt enable   
    T2CONbits.TON     = 1;      // start timer        


    T3CON = 0;                  // DO NOT CHANGE 
    TMR3  = 0;                  // DO NOT CHANGE 

    PR3               = 625;    // period register - counts = FCY / (prescaler * f_desired) = 4 MHz / (64 * 100 Hz) = 625
    T3CONbits.TCKPS   = 0;      // prescaler select - 1:64
    T3CONbits.TCS     = 0;      // clock source     - FOSC/2

    T3CONbits.TON     = 1;      // start timer   
}


/* pwm_init: configure OC1 in edge-aligned PWM mode on RB13 (ENB).
 * Implement from scratch.
 *   - RB13 has an analog function that must be disabled before digital use.
 *   - OC1 must be routed to RB13 via the PPS system.
 *   - Set OC1RS for a 10 kHz PWM frequency.
 *   - Set the OCM bits to enable edge-aligned PWM mode.
 * Datasheet ref: Sections 11.5 (PPS) and 15.3 (Output Compare). */
void pwm_init(void) {
    // your code here 
    RPOR6bits.RP13R = 13; // RP13 - RB13 set as PWM output (OC1)
    RPOR3bits.RP6R = 13; // RP6 - RB6 set as PWM output (OC1)
    OC1RS = PWM_PERIOD(PWM_FREQ); // Set the period for 1 kHz PWM frequency
    OC1R = PWM_DUTY_CYCLE(PWM_DUTY);//PWM_PERIOD(PWM_FREQ);
    
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCTSEL = 1; // Time 3 Clock Source

    OC1CON1bits.OCM = 0b110; // Init pin low and compare drive it high
    
    
}


/* set_motor_pwm: apply a signed controller output to the L298N.
 *   control > 0  ->  CW  (IN3=1, IN4=0)
 *   control < 0  ->  CCW (IN3=0, IN4=1)
 *   control = 0  ->  stopped
 * Clear both direction pins before setting the new direction.
 * The duty cycle written to OC1R must be non-negative and clamped to PWM_MAX. */
void set_motor_pwm(int32_t control) {
    int32_t magnitude;
    // Clearing Directions Pins
    // IN3 = 0;
    // IN4 = 0;

    if (control > 0) { // Forward
        IN3 = 1;
        IN4 = 0;
    } 
    else if (control < 0) { // Backward
        IN3 = 0;
        IN4 = 1;
    } 
    else {
        IN3 = 0;
        IN4 = 0;
    }
    magnitude = abs(control);
    if (magnitude > 0) {
        magnitude = PWM_MAX;
    }
    if (magnitude < 0) {
        magnitude = PWM_MIN;
    }
    OC1R = magnitude;
}


/* run_controller: compute the P-control output and drive the motor.
 * NO floating-point. Use Q8 fixed-point: u = (KP_Q8 * error) >> 8
 *
 * WHY __builtin_disi IS NEEDED:
 *   encoder_count is 32-bit on a 16-bit CPU -- reading it takes two load
 *   instructions. If _IOCInterrupt fires between them, the two halves of
 *   'position' come from different ISR updates (corrupted read).
 *   disi(0x3FFF) disables interrupts for those two instructions only.
 *   Clearing IEC1bits.IOCIE instead risks silently dropping an encoder
 *   edge -- a position error the controller can never recover from. */
void run_controller(void) {
    int32_t position;
    int32_t error;
    int32_t u;

    __builtin_disi(0x3FFF);         // DO NOT CHANGE 
    position = encoder_count;       // DO NOT CHANGE
    __builtin_disi(0x0000);         // DO NOT CHANGE

    // write your control algorithm here
    error = REFERENCE - position;
    u = (KP_Q8 * error) >> 8;
    set_motor_pwm(u);
    send_controller_data(REFERENCE, position, u);
}


/* _IOCInterrupt: fires on every edge of channel A (RB7).
 * Implement from scratch:
 *   - Confirm the source is RB7, then keep rising edges only.
 *   - Increment or decrement encoder_count based on channel B.
 *   - Clear flags in this exact order: IOCFB7 -> IOCPBF -> IOCIF.
 * No function calls, UART, or delays inside this ISR. */
void __attribute__((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    if (IOCFBbits.IOCFB7) {
        IOCFBbits.IOCFB7 = 0; // Clear the interrupt flag
        IOCPBbits.IOCPB7 = 0; // Clear the interrupt flag
        IFS1bits.IOCIF = 0; // Clear the interrupt flag

        if (CHB) {
            encoder_count++;
        } 
        else {
            encoder_count--;
        }
    }
}




/* _T2Interrupt: DO NOT MODIFY. */
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    control_flag  = 1;
    IFS0bits.T2IF = 0;
    t2_count++;


    // if (t2_count % 10 == 0) {
    //     // every 100 ms, run the controller
    //     if (OC1R < PWM_MAX) {
    //         OC1R += 5; // increment the duty cycle by 1
    //     }
    //     else {
    //         OC1R = 1; // reset the duty cycle to 1
    //     }
    // }

    if (t2_count == 100) {
        // every 1 second, print the encoder count to the UART
        t2_count = 0;
        LED ^= 1; // toggle LED
    }
}

/* main: initialise peripherals then run the control super-loop.
 *
 * Init order:
 *   clock_init() -> gpio_init() -> pwm_init() -> encoder_init() -> timer_init()
 *
 * timer_init() must be last: starting Timer2 before other peripherals are
 * configured means _T2Interrupt can fire before PWM and the encoder are ready. */
int main(void) {
    IFS0 = 0; IFS1 = 0; IFS2 = 0; IFS3 = 0; IFS4 = 0;
    clock_init();
    gpio_init();
    encoder_init();
    pwm_init();
    timer_init();
    char buffer[50];
    Disp2String("Reference ,Encoder count, input signal\r\n");

    while (1) {
        if (control_flag) {
            control_flag = 0;
            run_controller();
            // OC1R = PWM_DUTY_CYCLE(50);
            // IN3 = 0;
            // IN4 = 1;
        }
        Idle(); // low-power sleep until an interrupt occurs
    }

    return 0;
}

void send_controller_data(int32_t reference, int32_t encoder_count, int32_t u) {
    char buffer[50];
    sprintf(buffer, "%ld,%ld,%ld\r\n", reference,encoder_count, u);
    Disp2String(buffer);
}

uint32_t abs(int32_t value) {
    if (value < 0) {
        return -value;
    } else {
        return value;
    }
}