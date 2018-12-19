/*
* Javier Abellan, 4 Septiembre 2003
*
* Programa Servidor de socket UDP, como ejemplo de utilizacion de las
* funciones de sockets udp.
*/
#include <Socket_Servidor.h>
#include <Socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

/*
 * Programa principal.
 * Atiende al servicio udp de nombre cpp_java y espera peticiones de clientes.
 * El cliente se supone que le envia un entero, el servidor lo incrementa y se lo
 * devuelve incrementado.
 */
main ()
{
	/*
	* Descriptores de socket servidor y de socket con el cliente
	*/
	int Socket_Servidor;
	int contador = 0;
	struct sockaddr_in Cliente;
	socklen_t Longitud_Cliente;

	/*
	* Se abre el socket servidor, con el servicio "cpp_java" dado de
	* alta en /etc/services.
	*/
	Socket_Servidor = Abre_Socket_Udp ("cpp_java");
	if (Socket_Servidor == -1)
	{
		printf ("No se puede abrir socket servidor\n");
		exit (-1);
	}

   /* Preparación de datos para la lectura de mensajes */
	Longitud_Cliente = sizeof (Cliente);

   /* Bucle infinito esperando mensajes de clientes */
	while (1)
	{
      /* Lectura de un cliente. La llamada se queda bloqueada hasta que llegue
       * algún mensaje. En Cliente obtendremos los datos del cliente cuando llegue
       * el mensaje */
      Lee_Socket_Udp (Socket_Servidor, (struct sockaddr *)&Cliente,
         &Longitud_Cliente, (char *)&contador, sizeof(contador));

      /* Escribimos en pantalla lo que ha llegado y lo incrementamos */
      printf ("Recibido %d, envio %d\n", contador, contador+1);
		contador++;

      /* Devolvemos el mensaje, utilizando la estructura sockaddr Cliente, que obtuvimos
       * con la llamada a Lee_Socket_Udp y que contiene los datos del cliente. */
		Escribe_Socket_Udp (Socket_Servidor, (struct sockaddr *)&Cliente,
			Longitud_Cliente, (char *)&contador, sizeof(contador));
	}
}
