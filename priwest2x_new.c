//Progarama igual a PRIWEST2X.C en el que se ha habilitado el JMP5 para área de inhibición.
// 0 --> 3 MN
// 1 --> 7 MN


//PRIMARIO 113
//El pretrigger o PRF se produce 40 useg antes del ZMT (P3)
//Esta demora DEBE SER compensada a nivel FPGA por lo que el inicio de la lectura de Rango es casi coincidente con el ZMT 

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include "fifo2.h"
#include "comalt.h"
//#include "interrup.h"
#include "interalt.h"
#include "primari1.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
//++++++++++++++++++++++++++++
#include "compiler.h"
#include "capi.h"
//++++++++++++++++++++++++++++
//#define UDP_PORT 33  //Direccion puerto remoto

unsigned char  Buff[1024];
unsigned char  iwr=0,ird=0;
unsigned FlagIni = 0;
unsigned int DatoAnt, AzAnt;
unsigned char SectorActual;//version WP101

//unsigned int Mapa_Clutter[4][64][128];
//unsigned int Mapa_Clutter_Total[64][128];
//unsigned int Mapa_Umbral[64][128];
//unsigned int vuelta_mapa;
//unsigned int limite_clutter=40;
//int flag_sector=0;
//int dato_umbral=0x10;

unsigned int AREA_INHIBICION=0;

unsigned char InBuff[256];
unsigned int IndWr, IndRd;
unsigned char bu1, bu2, bu3, bu4, bu5, bu6, bu7, bu8, bu9, bu10;
int ok1, ok2, ok3;
int Flag_alt;
float X_alt, Y_alt;
float X_bco, Y_bco;
unsigned int sector_alt;
unsigned int Dist, Dist_max=3;//En MN. Valor estimado aproximado
unsigned int alt_alt;
//+++++++++++++++++++++++++variables globales martin+++++++++++++++++++++++++++
int UDP_PORT;  //Direccion puerto remoto
int iSendSock;
unsigned char  CabBuffUDP[4],iwr_u=0,PlotBuffUDP[6];
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float PI=3.1415;
//funciones
void ComSet(void);
void ResetFifo(void);
void IntEna(void);
void Inicializa(void);
int ChkFlag(void);
int CtrlDato(unsigned int );
int GetDato(unsigned int* );
void blancos(unsigned int , unsigned int , unsigned int );
void cabecera(char );
int Primario(unsigned int );
int Primario_eco(unsigned int );
void InicializaPr(void);

//+++++++++++++++++++++++funciones martin+++++++++++++++++++++++++++++++++++++++
static int GetClientSock(void);
static NET_ADDR sNetAddr; //general use
int StartListen(int iPort);
char *Err(unsigned uErrCode);
void InicializarUDP(void);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//Rutina de interrupcion de lectura por COM3
void interrupt intRxCom3()
 {
  InBuff[IndWr] = inp(TRX3);
  IndWr = (IndWr+1) & 0xFF;
  //cprintf(".");
   outp(0xA0,0x20);
   outp(INT1,0x20);
 }


void interrupt TxBuf()
 {
  //disable();
  outp(TRX1,Buff[ird++]);
  //if(iwr==ird) outportb(INTER1,0x00);
  outp(INTER1,0x00);
  outp(INT1,0x20);
  //enable();
 }



void ResetFifo()
 {
   outpw(0x350,RSTFIFO); //enable datos
   outpw(0x350,ENDATOS); //reset fifo
   outpw(0x350,RSTFIFO); //enable datos
 }


void ComSet()
{
  outp(LCR1,0X83);         /*dlab=1,8 data,1 stop,no parity 	       */
  outp(INTER1,0x0);        /*velocidad  9600 dlab HIGH       	       */
  outp(TRX1,_57400);       //_57400);       /*velocdidad 115000 dlab LOW    */	//1-abr-12
  outp(LCR1,0x03);         /*dlab=0				       */
  outp(INTER1,0x00);       /*deshabilito interrupcion de dato a recibir */
  outp(MCR1,0x0B);
  outp(LSR1,0x60);

  outp(LCR3,0X83);         /*dlab=1,8 data,1 stop,no parity 	       */
  outp(INTER3,0x0);        /*velocidad  9600 dlab HIGH       	       */
  outp(TRX3,_9600);       /*velocdidad 9600 dlab LOW       	       */
  outp(LCR3,0x03);         /*dlab=0				       */
  outp(INTER3,0x01);       /*deshabilito interrupcion de dato a recibir */
  outp(MCR3,0x0B);
  outp(LSR3,0x60);
}


void IntEna()
{
 unsigned char estado;

 estado = inp(INT2);    /* hab. interrupciones a traves del 8259 */
 estado = estado & 0x00;
 outp(INT2,estado);

}

void Inicializa()
{
 ComSet();
 IntEna();
 _dos_setvect(INTR1,TxBuf); //puerto 1
 _dos_setvect(INTR3,intRxCom3); //puerto 3 para altura
 ResetFifo();

}




//++++++++++++++++++++++++++++++funciones martin++++++++++++++++++++++++++++++++

void InicializarUDP(void)
{
iSendSock = GetClientSock();  //Habilita el socket de envio
if (iSendSock == 0)
return;
}

static int GetClientSock(void)
{
   int iSock;

   memset(&sNetAddr, 0, sizeof(NET_ADDR));
   sNetAddr.wRemotePort = UDP_PORT;

   printf("\nCONFIGURACION SOCKET\n");
   printf("%d: sNetAddr.wLocalPort = %d\n", __LINE__, sNetAddr.wLocalPort);
   printf("%d: sNetAddr.wRemotePort = %d\n", __LINE__, sNetAddr.wRemotePort);

   if ((iSock = GetSocket()) < 0)
      printf("Error on GetSocket(): %s\n",Err(iNetErrNo));
   else if (SetSocketOption(iSock, 0, NET_OPT_NON_BLOCKING, 1, 1) < 0)
      printf("Error on setOpt(): %s\n",Err(iNetErrNo));
   else if (ConnectSocket(iSock, DATA_GRAM, &sNetAddr) < 0)
      printf("Error on net_connect: %s\n",Err(iNetErrNo));
   else
   {
   	printf("%d: sNetAddr.wLocalPort = %d\n",  __LINE__, sNetAddr.wLocalPort);
      printf("%d: sNetAddr.wRemotePort = %d\n", __LINE__, sNetAddr.wRemotePort);
      printf("Client sock successfully created\n");
      return iSock;
   }

   return 0;
}

char *Err(unsigned uErrCode)
{
   static char rgcUnk[30];
   static char *rgszErrs[] =
   {
      "NoErr",
      "InUse",
      "DOSErr",
      "NoMem",
      "NotNetconn",
      "IllegalOp",
      "BadPkt",
      "NoHost",
      "CantOpen",
      "NetUnreachable",
      "HostUnreachable",
      "ProtUnreachable",
      "PortUnreachable",
      "TimeOut",
      "HostUnknown",
      "NoServers",
      "ServerErr",
      "BadFormat",
      "BadArg",
      "EOF",
      "Reset",
      "WouldBlock",
      "UnBound",
      "NoDesc",
      "BadSysCall",
      "CantBroadcast",
      "NotEstab",
      "ReEntry",
   };

   if (uErrCode == ERR_API_NOT_LOADED)
      return "Sockets API not loaded";
   if ((uErrCode & 0xff) > ERR_RE_ENTRY)
   {
      sprintf(rgcUnk,"Unknown error 0x%04X",uErrCode);
      return rgcUnk;
   }
   return rgszErrs[uErrCode & 0xff];
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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


int CtrlDato(unsigned int Dato)
{
  //CONTROLA QUE EL DATO RECIBIDO PERTENEZCA A ALGUNO DE LOS ESPERADOS
  if(((Dato&0xf000) != 0x8000)&&
     ((Dato&0xf000) != 0x3000)&&
     ((Dato&0xf000) != 0xC000))
     {
      //Buff[iwr++]= '1'; //error 1
      cprintf("ERROR0   %x   ",Dato);
      ResetFifo();
      FlagIni = 0;
      //iwr = ird = 0;
      //banderainic = 1; //modif R01!!!
      return -1;
     }
  //ESPERA EL PRIMER DATO PARA LUEGO EN EL PROXIMO , HACER EL CONTROL
  if(!FlagIni)
    {
     if((Dato&0xf000) == 0x8000) AzAnt = Dato;
     DatoAnt = Dato;
     FlagIni = 1;
     //banderainic = 1;  //modif R01!!!
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
	     	//iwr = ird = 0;
	     	//banderainic = 1; //modif R01!!!
	     	return -1;
	    	}
		}
	//SI ES CAMBIO DE SECTOR, MANDA UN CARACTER
	//else  Buff[iwr++]= 'T';


      DatoAnt = AzAnt = Dato;
    }

   //if((DatoAnt&0xF000) == 0xC000) cprintf("%x  %x   ",DatoAnt,Dato);
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

   //CONTROLA EL DATO DE RANGO

   /*
   if((Dato&0xf000)  == 0x3000)
    {
     //SI EL DATO ANTERIOR ES DE RANGO. ESTE DEBE SER CONSECUTIVO AL ANTERIOR,,!!!!REVEER
     if(((DatoAnt&0x3000) == 0x3000)&&
	 (Dato != (DatoAnt+1)))
	  {
	   ResetFifo();
	   Buff[iwr++]= '3'; //error 3
	   FlagIni = 0;
	   return -1;
	  }

     DatoAnt = Dato;
    }
   */
  //CONTROLA EL DATO DE ALTRURA
  /*
  if((Dato&0xf000)  == 0xC000)
    {
     //EL DATO ANTERIOR DEBE SR UN AZIMUT REVEER!!!!!!!
     if((DatoAnt&0xF000) != 0x8000)
	{
	 ResetFifo();
	 Buff[iwr++]= '4'; //error 4
	 FlagIni = 0;
	 return -1;
	}
     DatoAnt = Dato;
    }
  */

  return 0;

}
unsigned int AuxAnt;

int GetDato(unsigned int* Dato )
{
  unsigned int Aux;

  Aux = inpw(FIFO);
  //if((Aux & 0xf000)== 0xc000)
//  if((Aux & 0xf000)== 0x8000){
//  			if((Aux & 0x0f00) != (AuxAnt & 0xf00))
  	//				cprintf("alt   ");
//  			AuxAnt=Aux;
//			}
////  if((Aux & 0xf000)== 0x3000)
  //return -1;
  //if((Aux & 0xf000)== 0x8000)
  	//cprintf("%x  ",Aux);
  //else cprintf(" ");
  if(CtrlDato(Aux))
    return -1;
  *Dato = Aux;

 return 0;
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
   //cprintf("%x  ",Buff[iwr]);
   Buff[iwr++] = (az & 0xff00)>>8;
   //cprintf("%x  ",Buff[iwr]);
   Buff[iwr++] = rng & 0x00ff;
   //cprintf("%x  ",Buff[iwr]);
   Buff[iwr++] = (rng & 0xff00)>>8;
   //cprintf("%x  ",Buff[iwr]);
   Buff[iwr++] = Altr & 0x00ff;
   //cprintf("%x  ",Buff[iwr]);
   Buff[iwr++] = ((Altr & 0x0F00)>>8)+ 0xC0;
   //cprintf("%x  ",Buff[iwr]);
//++++++++++++++++++++++++++++++++++++++++++++++++++++

   PlotBuffUDP[iwr_u++] = az & 0x00ff;
   PlotBuffUDP[iwr_u++] = (az & 0xff00)>>8;
   PlotBuffUDP[iwr_u++] = rng & 0x00ff;
   PlotBuffUDP[iwr_u++] = (rng & 0xff00)>>8;
   PlotBuffUDP[iwr_u++] = Altr & 0x00ff;
   PlotBuffUDP[iwr_u++] = ((Altr & 0x0F00)>>8)+ 0xC0;
 
//++++++++++++++++++++++++++++++++++++++++++++++++++++
  }
//++++++++++++++++++++++++++++++++++++++++++++++++++++  
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

void cabecera(char sect)
{
/*
  Esta rutina recibe el numero de sector y coloca la informacion de
  cabecera en el buffer de transmision.
  Modifica solamente el nibble correspondiente a sector.
*/

	int indloc;
   unsigned char aux;
	/*
	Buff[iwr++] = 0xff;
	Buff[iwr++] = 0xff;
	Buff[iwr++] = (header[2] & 0xf0)|sect;
	Buff[iwr++] = 0x0;
	*/

   header[2]=(header[2] & 0xf0)|sect;

	for(indloc=0;indloc<=3;indloc++)
      {
   		Buff[iwr++]=header[indloc];
   		//++++++++++++++++++++++++++++++++++++++
      	CabBuffUDP[iwr_u++]=header[indloc];
      	//++++++++++++++++++++++++++++++++++++++
      }
   SectorActual = (unsigned char)sect;

       /* outbyte(ier,txen);      */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

       //reordenamiento de bytes en el buffer
       aux=CabBuffUDP[3];
       CabBuffUDP[3]=CabBuffUDP[2];
       CabBuffUDP[2]=aux;
       //envio por UDP
       if (WriteSocket(iSendSock, CabBuffUDP, 4, NET_FLG_BROADCAST) < 0)//error
	    printf("Error on NetWrite %d bytes: %s\n", 4, Err(iNetErrNo));
       iwr_u=0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
       
}

int Primario(unsigned int lectura)
{
   //unsigned int retorno;

   int ss,tt,uu;	
   int aux;
   int j;
   int dif;
   int modulo;
   int i;
   int aztfinal;
   int RangoAltura=0;

   dif=(tolerancia>>1)+1;   /*dif = maxima distancia entre dos ecos para considerarlo un mismo blanco*/
   //retorno=0;
   //lectura=inword(fifo);   /*lectura carga el primer dato disponible en fifo*/

   if(((lectura & 0xf000)==0x8000)||((lectura & 0xf000)==0x3000)||((lectura & 0xf000)==0xC000)) /*Es lectura un dato valido?*/
   { /* lectura si es dato valido*/
    if((lectura & 0xf000)==0x8000) /* es lectura un dato de azimuth?*/
    {  /* DATO ES AZIMUTH */
     PrsAltura=0;
     if(((lectura < (azimuth+4))&&((lectura > azimuth)||(lectura < 0x8003)))||(banderainic == 1))
     { /*Esta el azimut actual en un entorno del azimut anterior?*/
      banderainic=0;
      azimuth=lectura;
      bandera_azt=1;
      sectoractual= ( lectura & 0x0f00 ) >> 8;
      if( sectoractual != sectoranterior )
      {
       sectoranterior=sectoractual;
       cabecera(sectoractual);
/*
       //Solo para Mapa de Clutter
       if((sectoractual==0)&& (flag_sector==0)){
	       flag_sector=1;
	       //Borra acumulado Mapa anterior
	       		for(ss=0;ss<64;ss++){
		       		for(tt=0;tt<128;tt++){
			       		Mapa_Clutter_Total[ss][tt]==0;
			       		}
	       			}
       		//Carga nuevo Mapa
	       		for(ss=0;ss<64;ss++){
		       		//Mapa actua solo dentro de las 55 MN
		       		for(tt=0;tt<32;tt++){
			       		Mapa_Clutter_Total[ss][tt]=Mapa_Clutter[0][ss][tt]+Mapa_Clutter[1][ss][tt]+Mapa_Clutter[2][ss][tt]+ Mapa_Clutter[3][ss][tt];
			       		//if(Mapa_Clutter_Total[ss][tt]!=0)printf("[%d][%d]= %d  ",ss,tt,Mapa_Clutter_Total[ss][tt]);
			       		if(Mapa_Clutter_Total[ss][tt]>=limite_clutter){
		       				Mapa_Umbral[ss][tt]=0;
		       				}
		       			else{
		       				Mapa_Umbral[ss][tt]=1;
		       				}
		       			//if(Mapa_Clutter_Total[ss][tt]!=0)printf("MU= %d \n",Mapa_Umbral[ss][tt]);
		       			}
		       		//Fuera de las 55 MN no actua filtro por Mapa de Clutter
		       		for(tt=32;tt<128;tt++){
			       			Mapa_Umbral[ss][tt]=1;
		       				}
	       			}

       	vuelta_mapa = (vuelta_mapa + 1)& 0x03;
       	//Borra Arreglo vuelta equivalente anterior
	       	for(ss=0;ss<64;ss++){
		    	for(tt=0;tt<128;tt++){
			   		Mapa_Clutter[vuelta_mapa][ss][tt]=0;
			   		}
	       		}
	       	dato_umbral=(dato_umbral + 0x10) & 0xF0;
	       	if(dato_umbral==0) dato_umbral=0x10;
	       	//printf("--->%x\n",dato_umbral);
//-------->       		outp(0x352,dato_umbral);
        	}
        else flag_sector=0;
*/
      }
      if (bandera==0)
      {
       for(j=0;j<indice;j++)
       {
	modulo=(rangoinic-prs[j].rangoinic);
	if (modulo<0)
	{
	 modulo=~modulo;
	 modulo++;
	}
	if(dif>modulo)
	{
	 dif=modulo;       /*encuentra la menor distancia*/
	 aux=j;
	}
       }
       if(dif<=(tolerancia>>1))  /*hay correlacion*/
       {
	prs[aux].auscons=0;
       }
       else /*crear nuevo blanco*/
       {
			//cprintf("dif=%x   ",dif);
	       //cprintf("crea nvo bco.2 Azt_inic=%x   ",aztant);
	prs[indice].rangoinic=rangoinic;
	prs[indice].aztinic=aztant;
	prs[indice].auscons=0;
	prs[indice].Altura=0;
	indice++;
       }
      } /*fin bandera=0*/

      bandera=1;
      aztant=lectura&0x0fff;
      for(j=0;j<indice;j++)
      {
       prs[j].auscons++;
       if(prs[j].auscons>=max_aus_cons)
       {
	aztfinal=aztant-max_aus_cons;
	aztprom=(prs[j].aztinic+aztfinal)>>1;
	if(aztfinal<prs[j].aztinic)
	{
	 aztprom=(aztprom+(max_vuelta>>1) & 0x0fff);
	 aztfinal=aztfinal+4096;
	}
	//cprintf("-----%x  %x      -----",aztfinal,prs[j].aztinic);
	if((aztfinal-prs[j].aztinic)>=blancocorto)
	{
	 aztprom=aztprom|0x8000;
//	 blancos(aztprom,prs[j].rangoinic,prs[j].Altura);	//HABILITAR PARA WESTINGHOUSE????????????????????
//	}													//HABILITAR PARA WESTINGHOUSE????????????????????

	//SOLO PARA 113 -----------------------------------------------------------------------------------------------
	//Antes de poner el blanco en el buffer de transmisión
	 //se verifica si hay dato de altura y si éste es asociable
	 //al dato a transmitir
	 if(Flag_alt){//Hay dato de altura
	 //cprintf("AP=%x  ",(aztprom & 0x0F00)>>8);
	 			//Se compara con datos en el sector anterior, actual y posterior al de altura
	 			if((sector_alt==((aztprom & 0x0F00)>>8))|                //sector actual
	 			   (sector_alt==((((aztprom & 0x0F00)>>8)-1) & 0x0F))|   //sector anterior
	 			   (sector_alt==((((aztprom & 0x0F00)>>8)+1) & 0x0F))){  //sector posterior

	 					//Se calcula la posición X-Y del blanco
	 					X_bco =  (float)(prs[j].rangoinic & 0x0fff)*0.238356 * sin((float)(aztprom)*2*PI/4096.0);//En MN
  						Y_bco =  (float)(prs[j].rangoinic & 0x0fff)*0.238356 * cos((float)(aztprom)*2*PI/4096.0);//En MN
  						//cprintf("Xb=%x  ",prs[j].rangoinic  & 0x0fff);
  						//cprintf("X=%f  Y=%f  ",X_bco,Y_bco);
  						//cprintf("X=%f  Y=%f  ",X_alt,Y_alt);
  						//Se calcula la distancia entre blanco y dato-altura
  						Dist =  sqrt((X_bco - X_alt)*(X_bco - X_alt) + (Y_bco - Y_alt)*(Y_bco - Y_alt));//En MN
  						cprintf("Dist=%d  ",Dist);
  						//Se verifica si está dentro de la distancia aceptable para su asociación
 						if(Dist<Dist_max){//Se asocia dato de altura a plot y se manda a Buffer de tx
	 							blancos(aztprom,prs[j].rangoinic,alt_alt);
	 							cprintf("EXITO  ");
	 							Flag_alt=0;
	 							}
 					else{	//No se asocia dato de altura a plot (fuera de distancia); se manda a Buffer de tx
	 					blancos(aztprom,prs[j].rangoinic,prs[j].Altura);
	 					Flag_alt=0;
 						}
	 				}
	 			else //No se asocia dato de altura a plot (fuera de sector); se manda a Buffer de tx
	 				blancos(aztprom,prs[j].rangoinic,prs[j].Altura);
	 				//Flag_alt=0;
 				}
	 else ////No hay dato de altura; se manda a Buffer de tx
	 	blancos(aztprom,prs[j].rangoinic,prs[j].Altura);
	 
	}
	//FIN SOLO PARA 113------------------------------------------------------------------------------------------------------------------
	
	for (i=j;i<indice;i++)
	{
	 prs[i]=prs[i+1];
	}
	indice--;
	j--;
       }
      }
     }
     else 
     {
      banderainic = 1;
      if(lectura==azimuth)
      {
       bandera_azt=0;
      }
      else
      {
       indice=0;
      }
     }
    }/*fin de "lectura es un azt"*/
    else /* es un rango o altura*/
    {
     if ((lectura & 0xf000)==0x3000) /*Si es rango y esta fuera de las 15 MN aproxim*/
     {
	     //Agregado el 4-oct-12   NO PROCESA DENTRO DE LAS 15 MN
	     //if((lectura & 0x0fff)>=0x41)
	     //Modificado el 3-jun-14
	     if((lectura & 0x0fff)>=AREA_INHIBICION)
	     {
	     /*
	     //Incrementa celda correspondiente del mapa de clutter siempre que el rango leiodo de la FIFO no sea el que corresponde al dato de altura
	     if(!PrsAltura){
	     	Mapa_Clutter[vuelta_mapa][(azimuth & 0x0FFF)/64][(lectura & 0x0FFF)/8]+=1;
	     	//printf("[%d][%d][%d]= %d --> %d   MCT= %d \n",vuelta_mapa,(azimuth & 0x0FFF)/32,(lectura & 0x0FFF)/8,Mapa_Clutter[vuelta_mapa][(azimuth & 0x0FFF)/32][(lectura & 0x0FFF)/8],Mapa_Umbral[(azimuth & 0x0FFF)/32][(lectura & 0x0FFF)/8],Mapa_Clutter_Total[(azimuth & 0x0FFF)/32][(lectura & 0x0FFF)/8]);
	     	}
	     */
	     
	     //lectura = (lectura & 0x0FFF);  //4-abr-12
	     //Si el rango es menor a 5 MN (cuenta RC=21) no procesa!!!!   //4-abr-12
	//SOLO PARA 113-->     if(((lectura & 0x0FFF) < 21)||((lectura & 0x0FFF) > 0x0FFF)){}  //Verifica, además, que este dentro del rango  //1-abr-12
	//SOLO PARA 113 --> else{
	     //cprintf("%x  ",lectura);
	     //Si es dato de altura y no excedió el limite de clutter
      //if((PrsAltura==1)&&(Mapa_Umbral[(azimuth & 0x0FFF)/64][(lectura & 0x0FFF)/8]))
      if(PrsAltura==1)
        {
        PrsAltura=0;
	    bandera=1;
        RangoAltura=lectura;
        //cprintf("%x  ",RangoAltura);
        dif=(tolerancia>>1)+1;
        for(j=0;j<indice;j++)
           {
	       modulo=(RangoAltura-prs[j].rangoinic);
	       if (modulo<0)
	         {
	         modulo=~modulo;
	         modulo++;
	         }
	       if(dif>modulo)
	         {
	         dif=modulo;       /*encuentra la menor distancia*/
	         aux=j;
	         }
           }
        if(dif<=(tolerancia>>1))  /*hay correlacion de rango altura*/
           {
	       prs[aux].Altura=AlturaTemp;
	       //cprintf("xx   ");
           }
        return(0);
        }
        //Si es rango y no excedió el limite de clutter
       //if ((bandera_azt==1)&&(Mapa_Umbral[(azimuth & 0x0FFF)/64][(lectura & 0x0FFF)/8]))
       if (bandera_azt==1)
        {
        rangoactual= lectura;
        if (bandera==1)
          {
	      rangoinic=rangoactual;
	      bandera=0;
          }
        else /* si bandera=0*/
          {
	      if(rangoactual-rangoinic<=tolerancia)
	        {
	        }
	      else /*si esta fuera de la tolerancia ==> otro blanco*/
	        {
	        dif=(tolerancia>>1)+1;
	        for(j=0;j<indice;j++)
	          {
	           modulo=(rangoinic-prs[j].rangoinic);
	           if (modulo<0)
	             {
	             modulo=~modulo;
	             modulo++;
	             }
	           if(dif>modulo)
	             {
	             dif=modulo;       /*encuentra la menor distancia*/
	             aux=j;
	             }
	          }
	        if(dif<=( tolerancia>>1))  /*hay correlacion*/
	          {
	          rangoinic=rangoactual;
	          prs[aux].auscons=0;
	          }
	        else /*crear nuevo blanco*/
	          {
		          //cprintf("crea nvo bco.1 Azt_inic=%x   ",aztant);
	          prs[indice].rangoinic=rangoinic;
	          prs[indice].aztinic=aztant;
	          prs[indice].auscons=0;
	          prs[indice].Altura=0;
	          rangoinic=rangoactual;
	          indice++;
	          }
	        }
          }
        }
    	}
       //SOLO PARA 113 --> }
       } /* fin if 'es un rango'*/		//<-------------------------------------------------------------------------------
     else  /*es dato de altura*/
       {
	       //No excedio limite clutter
	 
//	 if (Mapa_Umbral[(azimuth & 0x0FFF)/64][(lectura & 0x0FFF)/8]){       
        	PrsAltura=1;
        	AlturaTemp=lectura&0x00ff;
        	//cprintf("%x  ",AlturaTemp);
//    		}

        }
       } /*fin else rango-altura*/
   }
 return 0;/////!!!! ver
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

     		//Cada Azimut es considerado el aztprom
     		//for (i=j;i<indice;i++)
			//	{
	 			//Pone el eco en el buffer de transmision
	 			//blancos(aztprom,prs[j].rangoinic,prs[j].Altura);
 			//	}
			//indice--;
			//j--;
       		}/*fin de "lectura es un azt"*/
    	else /* es un rango o altura*/
    		{
     		if ((lectura & 0xf000)==0x3000) /*Si es rango*/
     			{
	     			lectura = ((lectura & 0x3FFF) + 19);   //1-abr-12   //Eliminar al corregir error de FPGA
	     			if(lectura <= 0x3FFF){    //Procesa si esta dentro del rango   //1-abr-12
	     		/*if((lectura != 0x31fc)&&
	     		   (lectura != 0x31fd)&&
	     		   (lectura != 0x31fe)&&
	     		   (lectura != 0x31ff)&&
				   (lectura != 0x3200)&&
				   (lectura != 0x3201)&&
				   (lectura != 0x31fb)) cprintf("%x  ",lectura);*/
	     		//cprintf("%x   %x   ",aztprom,lectura);
	     		blancos(aztprom,lectura,0);
	     		//A cada eco (rango) se lo toma como plot nuevo
	          	//prs[indice].rangoinic=rangoinic;
	          	//prs[indice].aztinic=aztant;
	          	//prs[indice].auscons=0;
	          	//prs[indice].Altura=0;
	          	//rangoinic=rangoactual;
	          	//indice++;
          			}
	          	}
	        }
    	}

 return 0;
}


void InicializaPr()
{

 indice=0;
 sec_ant=0;

 header[0]=0xff;
 header[1]=0xff;
 header[2]=0x00;
 header[3]=0x00;

 bandera=0;
 banderainic=1;
 azimuth=0;
 lectura=0;
 rango=0;
 sectoranterior=0;

 bandera_azt=0;
 azimut_anterior=0xf000;
 PrsAltura=0;
 AlturaTemp=0;

}

void lee_altura()
{
 unsigned int az_alt, rg_alt;
 float intazt, intrgo, intalt;
 unsigned int mascara=0x200;
 int aa=0;
 float alt_max=100000.00;

 if(IndWr==IndRd){}	//No hay dato leído
 else{//Hay dato
 		bu8=InBuff[IndRd];
 		//cprintf("%x  ",bu8);
 		if((bu1==0xFF)&(bu2==0xFF)){
	 		if((bu4 & 0xF0)==0x80){//Dato de Az valido
	 					az_alt= ((bu4 & 0x0F)<<8) + bu3;
	 					ok1=1;
	 					intazt=(float)((float)(az_alt)*360.00/4096.0);
	 					//cprintf("AZ_ALT= %d\n",az_alt);
	 					cprintf("AZT=%3.2f   ",intazt);
 						}
 			if((bu6 & 0xF0)==0x30){//Dato de Rg valido
	 					rg_alt= ((bu6 & 0x03)<<8) + bu5;
	 					ok2=1;
	 					intrgo=(float)((float)(rg_alt)*0.25);//El bit menos significativo de la altura es de 1/4 MN
	 					//cprintf("RG_ALT= %d\n",rg_alt);
	 					cprintf("RGO=%3.2f   ",intrgo);
 						}
 			if((bu8 & 0xF0)==0xC0){//Dato de Alt valido
	 					alt_alt= ((bu8 & 0x03)<<8) + bu7;
	 					ok3=1;
	 					intalt=0;
	 					for(aa=0;aa<10;aa++){
		 							if((alt_alt & mascara)!= 0)
			 								intalt=intalt+alt_max/2;
			 						alt_max=alt_max/2;
		 							mascara=mascara>>1;		 							
	 								}
	 					//cprintf("ALT_ALT= %x   ",alt_alt);
	 					cprintf("ALT=%3.2f    ",intalt);
 						}
 			if((ok1==1)&(ok2==1)&(ok3==1)){//Paquete valido
 						//Convierte a coordenadas XY
 						X_alt =  (float)(intrgo) * sin((float)((az_alt & 0x0FFF))*2*PI/4096.0);
  						Y_alt =  (float)(intrgo) * cos((float)((az_alt & 0x0FFF))*2*PI/4096.0);
 						sector_alt = (az_alt & 0x0F00)>>8;
 						//cprintf("SA=%x  ",sector_alt);
 						//Activa bandera de dato de altura presente
 						Flag_alt=1;
 						//Resetea banderas de validez
 						ok1=0;ok2=0;ok3=0;
			            }
		    }
	    bu1=bu2;
	    bu2=bu3;
	    bu3=bu4;
	    bu4=bu5;
	    bu5=bu6;
	    bu6=bu7;
	    bu7=bu8;
	    //bu8=bu9;
	    //bu9=bu10;
	    IndRd = (IndRd + 1) & 0x0FF;
      }

}



unsigned int eco=0;
 

void main()
{
 unsigned int Dato,xxx,xxx_ant;
 unsigned long kk;
 unsigned char jumper,swich,inhibicion;
//++++++++++++++++++++++++++++++lectura del archivo socket.cfg de configuracion++++++++++++++++++++++++++

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
InicializarUDP();

//++++++++++++++++++++++++++++++lectura del archivo socket.cfg de configuracion++++++++++++++++++++++++++
 Inicializa();
 InicializaPr();


 outp(0x352,0xf1);

 //En JUMPER (0x300) se tiene:
 // JMP0 (bit0) ==> ECO
 // JMP1 (bit1) ==> UMBRAL Detector Ventana manejado por los Dip Switch o desde el programa

 	 //Lee estado de jumpers
	 jumper=inp(0x300);
	 cprintf("%x  ",jumper);
	 //JMP0
	 eco=jumper & 0x01;
	 if(eco) outp(0x354,0x01);//Anula Detector Ventana
	 else    outp(0x354,0x00);//Habilita Detector Ventana

	 //JMP1
	 //Manejo de umbral del detector ventana
	 swich=(jumper & 0x02)>>1;
	 if(swich) outp(0x352,0x01);//Manejo del Umbral a traves del DipSwitch
	 else      outp(0x352,0x30);//Manejo del Umbral a partir del valor cargado en el nibble superior; en este caso 0x3.

	 //JMP5
	 //Manejo de área de inhibición
	 inhibicion=(jumper & 0x10)>>4;
	 if(inhibicion) AREA_INHIBICION = 0x1E; //7 MN
	 else      		AREA_INHIBICION = 0x0D; //3 MN
	 
 	 cprintf("ECO=%x  SWITCH=%x  \n",eco,swich);

 while(1)
 {
	//Solo lee datos de altura si no esta en modo eco
	if((!eco)&&(!Flag_alt))lee_altura();//Solo para FPS-113

  	if(!ChkFlag())
   		if(!GetDato(&Dato)){

	   		if(eco){//Procesa ecos. Detector ventana anulado
	   				Primario_eco(Dato);
//	   			  	cprintf("E  ");
   					}
	   		else{//Procesa plots. Detector ventana habilitado
	   			Primario(Dato);
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