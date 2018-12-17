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

int puente (){
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

  *(uint32_t *)A_addr=STSFIFO; //-/ Flag = inp(STSFIFO);
  Flag = *(uint32_t *)D_addr;

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
void ResetFifo()
 {
   *(uint32_t *)A_addr=0x2350;//-/ outpw(0x350,RSTFIFO); //enable datos
   *(uint32_t *)D_addr=RSTFIFO;
   *(uint32_t *)A_addr=0x2350;//-/outpw(0x350,ENDATOS); //reset fifo
   *(uint32_t *)D_addr=ENDATOS;
   *(uint32_t *)A_addr=0x2350;//-/ outpw(0x350,RSTFIFO); //enable datos
   *(uint32_t *)D_addr=RSTFIFO;
 }


///////////////INICIO DE GETDATO///////////////////////
int GetDato(unsigned int* Dato )
{
  unsigned int Aux;
  *(uint32_t *)A_addr=FIFO;//-/Aux = inpw(FIFO);
  Aux = *(uint32_t *)D_addr;

  if(CtrlDato(Aux))
    return -1;
  *Dato = Aux;
 return 0;
}
int CtrlDato(unsigned int Dato)
{
  //CONTROLA QUE EL DATO RECIBIDO PERTENEZCA A ALGUNO DE LOS ESPERADOS
  if(((Dato&0xf000) != 0x8000)&&
     ((Dato&0xf000) != 0x3000)&&
     ((Dato&0xf000) != 0xC000))
     {
      cprintf("ERROR0   %x   ",Dato);
      ResetFifo();
      FlagIni = 0;
      return -1;
     }
  //ESPERA EL PRIMER DATO PARA LUEGO EN EL PROXIMO , HACER EL CONTROL
  if(!FlagIni)
    {
     if((Dato&0xf000) == 0x8000) AzAnt = Dato;
     DatoAnt = Dato;
     FlagIni = 1;
     return 0;
    }
  //CONTROLA UN DATO DE AZIMUT
  if((Dato&0xf000)  == 0x8000)
    {
	    AzAnt = AzAnt & 0x8FFF;
      //SI EL DATO ES MENOR O IGUAL AL DATO DE AZIMUT ANT
      //Y NO ES UN CAMBIO DE SECTOR
      if(Dato <= AzAnt)
        {
		if(((AzAnt&0xff00) != 0x8F00) ||
	   	((Dato&0xff00)  != 0x8000))
	    	{
		 	if(AzAnt!=Dato){
		    	cprintf("%x   %x    ",AzAnt,Dato);
		    	cprintf("ERROR1                            ");
	        	}
		    AzAnt=Dato;
	     	ResetFifo();//<------------
	     	FlagIni = 0;
	     	return -1;
	    	}
		}
	//SI ES CAMBIO DE SECTOR, MANDA UN CARACTER
	//else  Buff[iwr++]= 'T';
      DatoAnt = AzAnt = Dato;
    }
   //SI EL DATO ANTERIOR ES UNA ALTURA, ESTE DEBE SER UN RANGO
   if(((DatoAnt&0xF000) == 0xC000)&&((Dato&0xF000) != 0x3000))
       {
	       cprintf("ERROR2   ");
          ResetFifo();
          //Buff[iwr++]= '5'; //error 5
          //iwr = ird = 0;
          FlagIni = 0;
          banderainic = 1;    //modif R01!!!
          return -1;
       }
    else DatoAnt = Dato;		//<--Agregado 10/03/2010

  return 0;

}
int Primario_eco(unsigned int lectura)
{
   //unsigned int retorno;

   int aux;
   int j;
   int dif;
   int modulo;
   int i;
   int aztfinal;
   int RangoAltura=0;
//cprintf(". ");
   if(((lectura & 0xf000)==0x8000)||((lectura & 0xf000)==0x3000)||((lectura & 0xf000)==0xC000)) /*Es lectura un dato valido?*/
   		{ /* lectura si es dato valido*/
    	if((lectura & 0xf000)==0x8000) /* es lectura un dato de azimuth?*/
    		{  /* DATO ES AZIMUTH */
    		//cprintf("%x  ",lectura);
    		aztprom=lectura;
    		sectoractual= ( lectura & 0x0f00 ) >> 8;
            if( sectoractual != sectoranterior )
      			{
	      			//cprintf("%x   ",sectoractual);
       			sectoranterior=sectoractual;
       			cabecera(sectoractual);
      			}
       		}/*fin de "lectura es un azt"*/
    	else /* es un rango o altura*/
    		{
     		if ((lectura & 0xf000)==0x3000) /*Si es rango*/
     			{
	     			lectura = ((lectura & 0x3FFF) + 19);   //1-abr-12   //Eliminar al corregir error de FPGA
	     			if(lectura <= 0x3FFF){    //Procesa si esta dentro del rango   //1-abr-12

	     		blancos(aztprom,lectura,0);
          			}
	          	}
	        }
    	}

 return 0;
}

void cabecera(char sect)
{
/*
  Esta rutina recibe el numero de sector y coloca la informacion de
  cabecera en el buffer de transmision.
  Modifica solamente el nibble correspondiente a sector.
*/
	int indloc;
   unsigned char aux;

   header[2]=(header[2] & 0xf0)|sect;

	for(indloc=0;indloc<=3;indloc++)
      {
   		Buff[iwr++]=header[indloc];
      	CabBuffUDP[iwr_u++]=header[indloc];
      }
   SectorActual = (unsigned char)sect;
       //reordenamiento de bytes en el buffer
       aux=CabBuffUDP[3];
       CabBuffUDP[3]=CabBuffUDP[2];
       CabBuffUDP[2]=aux;
       //envio por UDP
       if (WriteSocket(iSendSock, CabBuffUDP, 4, NET_FLG_BROADCAST) < 0)//error
	    printf("Error on NetWrite %d bytes: %s\n", 4, Err(iNetErrNo));
       iwr_u=0;
}
void blancos(unsigned int az, unsigned int rng, unsigned int Altr)
{
 unsigned char Sector = (unsigned char) ((az&0x0f00)>>8);
 unsigned char SectAnt = (unsigned char) (abs(SectorActual - 1));
 unsigned char aux;
//cprintf("1=%x   2=%x   3=%x   ",Sector,SectAnt,SectorActual);
 //cprintf("A=%x  ",Altr);
 //Version WP101
 if((Sector == SectorActual)||(Sector == SectAnt))
  {
   Buff[iwr++] = az & 0x00ff;
   Buff[iwr++] = (az & 0xff00)>>8;
   Buff[iwr++] = rng & 0x00ff;
   Buff[iwr++] = (rng & 0xff00)>>8;
   Buff[iwr++] = Altr & 0x00ff;
   Buff[iwr++] = ((Altr & 0x0F00)>>8)+ 0xC0;

   PlotBuffUDP[iwr_u++] = az & 0x00ff;
   PlotBuffUDP[iwr_u++] = (az & 0xff00)>>8;
   PlotBuffUDP[iwr_u++] = rng & 0x00ff;
   PlotBuffUDP[iwr_u++] = (rng & 0xff00)>>8;
   PlotBuffUDP[iwr_u++] = Altr & 0x00ff;
   PlotBuffUDP[iwr_u++] = ((Altr & 0x0F00)>>8)+ 0xC0;
  }
//reordenamiento de bytes en el buffer
for(iwr_u=0;iwr_u<=5;iwr_u=iwr_u+2)
{
aux=PlotBuffUDP[iwr_u];
PlotBuffUDP[iwr_u]=PlotBuffUDP[iwr_u+1];
PlotBuffUDP[iwr_u+1]=aux;
}
//envio por UDP
if (WriteSocket(iSendSock, PlotBuffUDP, 6, NET_FLG_BROADCAST) < 0)//error
	    printf("Error on NetWrite %d bytes: %s\n", 4, Err(iNetErrNo));
       iwr_u=0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}
