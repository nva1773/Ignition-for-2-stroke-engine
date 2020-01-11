/* 
 * File:   header.h
 * Author: nva1773@gmail.com
 *
 * Created on 11/19/2019 4:54:48 PM UTC
 * "Created in MPLAB Xpress"
 */

#ifndef HEADER_H
#define	HEADER_H

/** A S M ***********************************************************/
#define Nop() {__asm("nop");}
#define ClrWdt() {__asm("clrwdt");}
#define Sleep() {__asm("sleep");}
#define Reset() {__asm("reset");}
#define Retfie() {__asm("retfie");}
#define Swapf(reg) {reg = (reg >> 4) | (reg << 4);}

/** O S C ***********************************************************/
// OSC = 4 MHz
#define OSCILLATOR	4000000
#define INSTRUCTION_CYCLE (OSCILLATOR)/4

/** P O R T S ******************************************************/
// Comp. - disable, ADC - disable, IO - inputs, IOCB - Off, WPU -Off
#define mResetPorts() {CMCON = 0x07;		\
					   ANSEL = 0x00;		\
					   GPIO = 0xFF;			\
					   TRISIO = 0xFF;		\
					   IOCB = 0x00;			\
					   WPU = 0x00;}

/** P I N S *********************************************************/
// Output Pulse on GP0 and GP1 (Pin6 and Pin7);
// Input - PickUp on GP2 (Pin5) __|-|_________
// other Pin - inputs - not used
#define mInitPINS() {TRISIO &= 0xFC; GPIO &= 0xFC; OPTION_REG = 0xC0;}
#define mStartOUT() GPIO |= 0xFF;
#define mStopOUT() GPIO &= 0xFC;
#define mInitINT() INTF = 0; INTE = 1
#define mDisableINT() INTE = 0

/** I S R  *********************************************************/
// Interrupt for: 
// - INT (GP2 rising pulse);
// - TMR0 - delay for angle ignition
// - TMR1 - over -> time out for measure RPM -> morot is stop
#define mInterruptEnable()  PEIE = 1; GIE = 1
#define mInterruptDisable() PEIE = 0; GIE = 0
// INT
#define mFlagINT INTF
#define mInterruptINT INTE&&INTF

// Timer0
#define mFlagTMR0 T0IF
#define mInterruptTMR0 T0IE&&T0IF
// Timer1
#define mFlagTMR1 TMR1IF
#define mInterruptTMR1 TMR1IE&&TMR1IF

/** T I M E R 0 *****************************************************/
// Fosc/4, prescale 1:16
#define TMR0L TMR0
#define mInitTimer0() OPTION_REG &= 0xC0; OPTION_REG |= 0x03
#define mStartTimer0() T0IF = 0; T0IE = 1
#define mStopTimer0() T0IF = 0; T0IE = 0

/** T I M E R 1 *****************************************************/
// Fosc/4, prescale 1:1
#define mInitTimer1() T1CON = 0x00; TMR1L = 0; TMR1H = 0
#define mStartTimer1() TMR1ON = 1; TMR1IF = 0; TMR1IE = 1
#define mStopTimer1() TMR1ON = 0; TMR1IF = 0; TMR1IE = 0

// from file "Ignition Yamaha DT125.xslx"
#define RPM_10750 349
// without program runtime compensation
//#define DELAY_MIN 64652
// without compensation of the execution time of the function RpmToAngle() = 120 mks
#define DELAY_MIN 64888

#endif	/* HEADER_H */
