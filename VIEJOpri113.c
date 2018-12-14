//*/PROGRAMA PUENTE CON GPIO DE LEANDRO


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"


#define REG_BASE 0xFF200000
#define REG_SPAN 0x00200000

void* virtual_base;
void* D_addr;		//Dirección del vector D de lectura-escritura
void* A_addr;		//Dirección del vector A de lectura-escritura
int fd;
int D;				//Valor del vector D
int A;				//Valor del vector A
int cambio;

/*INICIO DECLARACION VARIABLES VIEJAS*/
unsigned int AREA_INHIBICION=0;
/*FIN DECLARACION VARIABLES VIEJAS*/
/*DECLARACION INCLUDES*/
#include "fifo.h"

int main (){

//Acá comienza la apertura de la memoria del HPS, el mapeo y el direccionamiento de los vectores D y A

fd=open("/dev/mem",(O_RDWR|O_SYNC));
virtual_base=mmap(NULL,REG_SPAN,(PROT_READ|PROT_WRITE),MAP_SHARED,fd,REG_BASE);
D_addr=virtual_base+LECTURAESC_BASE;
A_addr=virtual_base+ESCRITURA_BASE;


//Se imprime y bienvenida al puente
printf("Bienvenido a su puente espectacular de 32 bits \n");
printf("By Lea Dan, Van Dam \n");

//Comienza el programa en un bucle infinito
while(1){
D=*(uint32_t *)D_addr; //D toma el valor que se encuentra almacenado en su dirección
printf("\nEl vector D tiene el valor: %i", D);
printf("\nEscriba un numero entre 0x y 0xFFFF para asignar a A: ");
scanf("%i", &A);

*(uint32_t *)A_addr=A;
usleep(100000);




}
return 0;




}


//-/ Copyright (c) 2018 Faa Reserved by lio .

//-/ FIN LEA








void main()
{
 unsigned int Dato,xxx,xxx_ant;
 unsigned long kk;
 unsigned char jumper,swich,inhibicion;
//++++++++++++++++++++++++++++++lectura del archivo socket.cfg de configuracion++++++++++++++++++++++++++
/* //-/ hay que remplazar con funcion PUENTE Y funcion SOCKET UDP
FILE *fp;
char num[20],cmp[]="UDP:";
int c=0;
fp=fopen("socket.cfg","r");
if(fp==NULL)
	printf("\nERROR:  no se encuentra archivo socket.cfg\n");
else
	{
		do{
			c=fscanf(fp,"%s",num);
			if(strcmp(cmp,num)==0)
				{
					fscanf(fp,"%s",num);
					UDP_PORT=atoi(num);
				}
		    }while(c!=EOF);
		}
printf("\nLectura de puerto UDP:%d\n",UDP_PORT);
fclose(fp);


system("socketp.exe");  //++++cargar configuraciones del socket UDP

*/

//-/ InicializarUDP(); //-/

//++++++++++++++++++++++++++++++lectura del archivo socket.cfg de configuracion++++++++++++++++++++++++++
 //-/ Inicializa();//-/ Inicializa llama a ComSet y a IntEna, ademas a ResetFifo

 InicializaPr(); //-/INICIALIZA LAS VARIABLES


 /// esta línea no es necesaria porque sirve para configurar        outp(0x352,0xf1);

 //En JUMPER (0x300) se tiene:
 // JMP0 (bit0) ==> ECO
 // JMP1 (bit1) ==> UMBRAL Detector Ventana manejado por los Dip Switch o desde el programa

 	 //Lee estado de jumpers
	 *(uint32_t *)A_addr=0x4300; //-/ jumper=inp(0x300);
   jumper=*(uint32_t *)D_addr;

	 cprintf("%x  ",jumper);
	 //JMP0
	 eco=jumper & 0x01;
	 if(eco){
   *(uint32_t *)A_addr=0x2354; //-/ outp(0x354,0x01);//Anula Detector Ventana
   *(uint32_t *)D_addr=0x01;
 }else{
   *(uint32_t *)A_addr=0x2354;
   *(uint32_t *)D_addr=0x00;  //-/ outp(0x354,0x00);//Habilita Detector Ventana
}
	 //JMP1
	 //Manejo de umbral del detector ventana
	 swich=(jumper & 0x02)>>1;
	 if(swich){
   *(uint32_t *)A_addr=0x2352; //-/ outp(0x352,0x01);//Manejo del Umbral a traves del DipSwitch
   *(uint32_t *)D_addr=0x01;
 }else{
      *(uint32_t *)A_addr=0x2352; //-/ outp(0x352,0x30);//Manejo del Umbral a partir del valor cargado en el nibble superior; en este caso 0x3.
      *(uint32_t *)D_addr=0x30;
    }
	 //JMP5
	 //Manejo de �rea de inhibici�n
	 inhibicion=(jumper & 0x10)>>4;
	 if(inhibicion) AREA_INHIBICION = 0x1E; //7 MN
	 else      		AREA_INHIBICION = 0x0D; //3 MN

 	 cprintf("ECO=%x  SWITCH=%x  \n",eco,swich);

 while(1)
 {
//-/	//Solo lee datos de altura si no esta en modo eco
//-/	if((!eco)&&(!Flag_alt))lee_altura();//Solo para FPS-113

  	if(!ChkFlag())
   		if(!GetDato(&Dato)){

	   		if(eco){//Procesa ecos. Detector ventana anulado
	   				Primario_eco(Dato);
//	   			  	cprintf("E  ");
   					}
	   		else{//Procesa plots. Detector ventana habilitado
	  //-/ 			Primario(Dato);
    	Primario_eco(Dato);//-/

//	   			cprintf("P  ");
   				}
   			}

  	if(iwr!=ird)          //outportb(INTER1,0x02);
    	{
      	while(((inp(LSR1))&0x60) != 0x60) {}
      	outp(INTER1,0x02);
     	}

 }

}

int ChkFlag()
{
  unsigned int Flag;

  Flag = inp(STSFIFO);
  //if(Flag!=0x01)
  //cprintf("%x ",Flag);
  //if((Flag != 0x0F)&& (Flag != 0x0C) && (Flag != 0x03))
  if(Flag == 0x02)
  	{
	  //cprintf("%x ",Flag);
	  ResetFifo();
	  FlagIni = 0;
	  return -1;
  	}
  //if(Flag == 0x0C)//FULL
  //    {
  //     ResetFifo();
  //     FlagIni = 0;
       //iwr = ird = 0;
  //     cprintf("R1 ");
  //     return -1;
  //    }

  //if((Flag&0x0003) != EMPTY) return 0;
  if(Flag == 0x00) return 0; //Hay Dato
  return -1;

}
