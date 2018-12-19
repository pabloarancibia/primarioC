

void socketnuevoserver()
{
int Descriptor;
Descriptor = socket (AF_INET, SOCK_DGRAM, 0);
//El primer parámetro indica que es socket es de red, (podría ser AF_UNIX para un socket entre procesos dentro del mismo ordenador).
//El segundo indica que es UDP (SOCK_STREAM indicaría un socket TCP orientado a conexión).
//El tercero es el protocolo que queremos utilizar. Hay varios disponibles, pero poniendo un 0 dejamos al sistema que elija este detalle.

//escribir en el fichero /etc/services (con permisos de root) el número de puerto que queremos atender y darle un nombre
//tx113   25558/udp   # servicio de tx primario.

}
struct sockaddr_in Direccion;

// Aquí hay que rellenar la estructura Direccion
Direccion.sin_family = AF_INET;
Direccion.sin_port = Puerto->s_port;//leemos puerto de /etc/services
Direccion.sin_addr.s_addr = INADDR_ANY;//El campo s_addr es la dirección IP del cliente al que queremos atender. Poniendo INADDR_ANY atenderemos a cualquier cliente.

bind ( Descriptor, (struct sockaddr *)&Direccion, sizeof (Direccion));

//Puerto para leer de /etc/services
struct servent *Puerto = NULL;
Puerto = getservbyname (tx113, "udp");
//

////PARA RECIBIR MENSAJE//
// Contendrá los datos del que nos envía el mensaje
struct sockaddr_in Cliente;

// Tamaño de la estructura anterior /
int longitudCliente = sizeof(Cliente);

// Nuestro mensaje es simplemente un entero, 4 bytes. //
int buffer;

recvfrom (Descriptor, (char *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&Cliente, &longtudCliente);
////FIN PARA RECIBIR
////////////////////
sendto (Descriptor, (char *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&Cliente, longitudCliente);
/*La función para envío de mensajes es sendto(). Esta función admite seis parámetros, que son los mismos que la función recvfrom(). Su signifcado cambia un poco, así que vamos a verlos:

int con el descriptor del socket por el que queremos enviar el mensaje. Lo obtuvimos con socket().
char * con el buffer de datos que queremos enviar. En este caso, al llamar a sendto() ya debe estar relleno con los datos a enviar.
int con el tamaño del mensaje anterior, en bytes.
int con opciones. De momento nos vale poner un 0.
struct sockaddr. Esta vez sí tiene que estar relleno, pero seguimos teniendo suerte, nos lo rellenó la función recvfrom(). Poniendo aquí la misma estructura que nos rellenó la función recvfrom(), estaremos enviando el mensaje al cliente que nos lo envío a nosotros previamente.
int con el tamaño de la estructura sockaddr. Vale el mismo entero que nos devolvió la función recvfrom() como sexto parámetro.*/
