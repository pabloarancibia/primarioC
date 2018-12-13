#ifndef FIFO_H
#define FIFO_H



//direcciones de operaciones de control
//tomando como base la direccion 0x0D00
#define STSFIFO 0x0D10
#define CLRWDT1 0x0D20
#define FIFO    0x0D30
#define AZT     0x0D40
#define CNTRL   0x0D50

//banderas
#define FULL     0x0002
#define HALF     0x0003
#define EMPTY    0x0005

//datos de control
#define RSTFIFO 0x00fe
#define ENDATOS 0x00fd






#endif