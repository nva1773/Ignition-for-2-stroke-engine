/* 
 * File:   typedef.h
 * Author: nva1773@gmail.com
 *
 * Created on 11/20/2019 2:07:54 PM UTC
 * "Created in MPLAB Xpress"
 */

#ifndef TYPEDEF_H
#define	TYPEDEF_H

//-----------------------------------------
// Union struct and unsigned char. 
//-----------------------------------------
union _BITS
{
 struct
 {
  unsigned b0:1;
  unsigned b1:1;
  unsigned b2:1;
  unsigned b3:1;
  unsigned b4:1;
  unsigned b5:1;
  unsigned b6:1;
  unsigned b7:1;
 } bits;
 unsigned char byte;
};
#define BITS union _BITS

//-----------------------------------------
// Union for Word
//-----------------------------------------
union _WORD
{
 unsigned int word;
 unsigned char byte[2];
};
#define WORD union _WORD 

//-----------------------------------------
// My Bool defined
//-----------------------------------------
#define true 1
#define false 0


#endif	/* TYPEDEF_H */
