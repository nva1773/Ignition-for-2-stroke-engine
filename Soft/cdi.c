/*
 * File:   cdi.c
 * Author: nva1773@gmail.com
 *
 * Created on 11/19/2019 4:54:13 PM UTC
 * "Created in MPLAB Xpress"
 * V2 - for 65 degree
 */

// Configuration Bit Settings
#pragma config FOSC = XT // Oscillator Selection bits (XT oscillator: Crystal/resonator on GP4/OSC2/CLKOUT and GP5/OSC1/CLKIN)
#pragma config WDTE = ON // Watchdog Timer Enable bit (WDT enable, 18ms without prescaler)
#pragma config PWRTE = ON // Power-Up Timer Enable bit (PWRT enable = 72ms)
#pragma config MCLRE = OFF // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF // Data Code Protection bit (Data memory code protection is disabled)

// Files include
#define _XTAL_FREQ 4000000 // for debuger
#include <xc.h> // access to SFRs
#include <pic12f675.h>
#include "header.h"
#include "typedef.h"

// dealy for start remoute pulse,
// without compensation of the execution time of the function RpmToAngle() = 120 mks ***********************************************
const unsigned int angleDeg[64] = { 64840,64831,64827,64823,64818,64825,64831,64827,64823,64830,64837,64844,64852,64848,64856,64864,
                                    64867,64860,64847,64834,64820,64805,64790,64775,64764,64753,64742,64729,64717,64707,64662,64616,
                                    64517,64048,63553,63028,62619,61884,61370,60483,60100,59656,59300,58513,58155,57761,57133,56396,
                                    56040,55655,55092,54212,53415,52752,51813,50774,49914,48988,47988,47287,46151,45365,44543,43174 };


// Prototypes
void InitCPU(void);
void InitVAR(void);
void RpmToAngle(void);

// Variable
__bit isFirst, isCalcAngle, isRun, isPulse;
unsigned char TMR0H, addr;
WORD delay;
WORD duration;

// Main function
void main(void)
{
    ClrWdt();
    InitCPU();
    InitVAR();
    
    mInterruptEnable();
    
    while(1)
    {
        ClrWdt();
        
        if(isCalcAngle)
            RpmToAngle();
        
        while(isRun)
            ClrWdt();
        
        if(isPulse)
        {
            __delay_us(200); // 200 mks pulse for tiristor
            mStopOUT();
            isPulse = false;
            mInitINT();
        }
    }
    
    return;
}

// Initialization CPU
void InitCPU(void)
{
    mInterruptDisable();
    mResetPorts();
    mInitPINS();
    mInitTimer0();
    mInitTimer1();
    mInitINT();
}

// Initialization Variable
void InitVAR(void)
{
    isFirst = false; isCalcAngle = false; isRun = false; isPulse = false;
    TMR0H = 0xFF;
    addr = 0;
    delay.word = 0xFFFF;
    duration.word = 0;
}

// Run time = 120mks
void RpmToAngle(void)
{
    isCalcAngle = false;
    
    duration.word -= RPM_10750; // zero compensation
    
    // Calc RPM and choise Angle
    if(duration.byte[1] >= 0x80) // (duration < 0) -> (RPM > 10750) -> angle = 5 deg;
    {
        delay.word = DELAY_MIN; // from file "Ignition Yamaha DT125.xslx"
    }
    else
    {
        if(duration.byte[1] > 0)
        {
            if(duration.byte[1] > 32) // RPM < 430
            {
                // not soft control !!!
                mInitINT();
                return;
            }
            else // RPM >= 6000 
            {
                addr = duration.byte[1] + 31;
                delay.word = angleDeg[addr];
            }
        }
        else // 430 < RPM < 6000
        {
            addr = duration.byte[0] >> 3;
            delay.word = angleDeg[addr];
        }
    }
    
    isRun = true;
    TMR1 = delay.word;
    //TMR1L = delay.byte[0];
    //TMR1H = delay.byte[1];
    mStartTimer1();
}

// Interrupt
void __interrupt() myISR(void)
{
	// For INT (GP2 - PickUp - rising pulse)
	if(mInterruptINT)
	{
	    mFlagINT = 0;
	    if(isFirst)
	    {
	        // Read Timer0
	        duration.byte[0] = TMR0L;
	        duration.byte[1] = TMR0H;
	        isCalcAngle = true;
            mDisableINT();
	    }
	    else
	    {
	        isFirst = true;
	    }
	    TMR0L = 0; TMR0H = 0;
	    mStartTimer0();
	}
	
	// For TMR0 -> increment TMR0H, 
	// if TRM0 over -> RPM < 430 -> not soft control !!!
	if(mInterruptTMR0)
	{
	    mFlagTMR0 = false;
	    TMR0H++;
	    if(TMR0H == 0x00)
	    {
	        mStopTimer0();
	        mStopTimer1();
	        mStopOUT();
	        isFirst = false;
	        isCalcAngle = false;
	        isRun = false;
	        isPulse = false;
            mInitINT();
	    }
	}
	
	// For TMR1 -> start pulse for open tiristor
	if(mInterruptTMR1)
	{
	    mStopTimer1();
	    mStartOUT();
	    isRun = false;
	    isPulse = true;
	}
}
