#ifndef COM_H
#define COM_H


//velocidades de transmision
#define _9600   0x0c
#define _19200  0x06
#define _38400  0x03
#define _57400  0x02
#define _115000 0x01	//1-abr-12


//direccion     base del puerto
#define  PTO     0x03F8
#define  PTO3    0x03E8


#define TRX1     PTO              /*lectura de datos*/
#define INTER1   PTO + 1          /*registro de habilitacion de interruciones*/
#define INTIR1   PTO + 2	  /*registro indicador de interrupciones*/
#define LCR1     PTO + 3          /*registro control de linea*/
#define MCR1     PTO + 4          /*registro control de modem*/
#define LSR1     PTO + 5          /*registro status de linea*/
#define MSR1     PTO + 6          /*registro status de modem*/

#define TRX3     PTO3              /*lectura de datos*/
#define INTER3   PTO3 + 1          /*registro de habilitacion de interruciones*/
#define INTIR3   PTO3 + 2	  /*registro indicador de interrupciones*/
#define LCR3     PTO3 + 3          /*registro control de linea*/
#define MCR3     PTO3 + 4          /*registro control de modem*/
#define LSR3     PTO3 + 5          /*registro status de linea*/
#define MSR3     PTO3 + 6          /*registro status de modem*/


#endif