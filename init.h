#ifndef INIT_H
#define INIT_H

/*
* VDD
* 8,19,26,34,39,54,60
* TQFP 8v,28v,35v,61v,66v,90v,96v
*
* VSS
* 7,20,25,35,40,55,59
* TQFP 7v,29v,34v,62v,67v,91v,95v
*
* PGED 18 -> 27
* PGEC 17 -> 26
*
* MCLR 9
*/

void initOscillator();
void initMem();

#endif