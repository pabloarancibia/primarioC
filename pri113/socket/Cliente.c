/*
* Javier Abellan, 4 Septiembre 2003
*
* Programa Cliente de un socket UDP, como ejemplo de utilizacion
* de las funciones de sockets
*/

/* Includes del sistema */
#include <netinet/in.h>
#include <stdio.h>

/* Includes de la mini-libreria de sockets */
#include <Socket_Cliente.h>
#include <Socket.h>

/* Programa principal.
 * Abre un socket, envia un numero al servidor y lee la respuesta de este, que
 * es el mismo n�mero incrementado en 1.
 */
main ()
{
   /* Descriptor del socket y buffer para datos */
   int Socket_Con_Servidor;
   int Datos;

   /* Para las llamadas a las funciones de sockets */
   int leidos;                   /* bytes leidos */
   struct sockaddr_in Servidor;  /* Direcci�n del servidor */
   socklen_t Longitud_Servidor;  /* Tama�o de la estructura Servidor */

   /*
    * Se abre un socket UDP para que el cliente pueda enviar y recibir mensajes
    * a trav�s de �l. La funci�n no lleva par�metros porque dejamos que unix se
    * encargue de los detalles.
   */
   Socket_Con_Servidor = Abre_Conexion_Udp ();
   if (Socket_Con_Servidor == -1)
   {
      printf ("No puedo establecer conexion con el servidor\n");
      exit (-1);
   }

   /* Rellenamos la estructura Servidor para poder enviarle un mensaje.
    * El servidor se le supone corriendo en la misma maquina (localhost) y
    * atendiendo al servicio cpp_java */
   Longitud_Servidor = sizeof(Servidor);
   if (Dame_Direccion_Udp ("localhost", "socketudp113", &Servidor,
      &Longitud_Servidor) == -1)
   {
      printf ("No se pueden obtener los datos del servidor\n");
      exit (-1);
   }

   /* Se pone un n�mero aleatorio en Datos, se muestra dicho numero por
    * pantalla y se envia al servidor */
   srand (time(NULL)); /* Semilla para n�meros aleatorios */
   Datos = rand()%20;  /* Aleatorio entre 0 y 19 */
   printf ("Envio %d\n", Datos);

   /* Se env�a el mensaje al servidor */
   if (Escribe_Socket_Udp (Socket_Con_Servidor, (struct sockaddr *)&Servidor,
      Longitud_Servidor, (char *)&Datos, sizeof(Datos)) == -1)
   {
      printf ("Error al enviar datos al servidor\n");
      exit (-1);
   };

   /* Se leen los datos que nos envie el servidor como respuesta.
    * El contenido de Servidor es indiferente, nos vendr� relleno con los
    * datos del que transmite el mensaje. */
   leidos = Lee_Socket_Udp (Socket_Con_Servidor,
         (struct sockaddr *)&Servidor, &Longitud_Servidor,
         (char *)&Datos, sizeof(Datos));

   /* Se escribe en pantalla los datos recibidos o el error si lo ha habido */
   if (leidos > 0)
      printf ("Leido %d\n", Datos);
   else
      printf ("Error al leer del servidor\n");
}
