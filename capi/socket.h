//  $Id: socket.h 1.5 2005/01/13 18:06:06Z thomd Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 1999-2005 Datalight, Inc.
//  All Rights Reserved
//
//  Datalight, Inc. is a Washington corporation located at:
//
//  21520 30th Dr SE, Suite 110,          Tel: +1.425.951.8086
//  Bothell, WA  98021                    Fax: +1.425.951.8094
//  USA                                   Http://www.datalight.com
//
//  This software, including without limitation all source code and
//  documentation, is the confidential, trade secret property of
//  Datalight, Inc., and is protected under the copyright laws of
//  the United States and other jurisdictions.  Portions of the
//  software may also be subject to one or more the following US
//  patents: US#5860082, US#6260156.
//
//  In addition to civil penalties for infringement of copyright
//  under applicable U.S. law, 17 U.S.C. 1204 provides criminal
//  penalties for violation of (a) the restrictions on circumvention
//  of copyright protection systems found in 17 U.S.C. 1201 and
//  (b) the protections for the integrity of copyright management
//  information found in 17 U.S.C. 1202.
//
//  U.S. Government Restricted Rights.  Use, duplication,
//  reproduction, or transfer of this commercial product and
//  accompanying documentation is restricted in accordance with
//  FAR 12.212 and DFARS 227.7202 and by a License Agreement.
//
//  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS
//  PROTECTED UNDER A SOURCE CODE AGREEMENT, NON-DISCLOSURE
//  AGREEMENT (NDA), OR SIMILAR BINDING CONTRACT BETWEEN DATALIGHT,
//  INC. AND THE LICENSEE ("BINDING AGREEMENT").  YOUR RIGHT, IF ANY,
//  TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,IS SUBJECT
//  TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT, AND BY
//  USING THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE
//  TO BE BOUND BY THE TERMS OF THE BINDING AGREEMENT.  CONTACT
//  DATALIGHT, INC. AT THE ADDRESS SET FORTH ABOVE IF YOU OBTAINED
//  THIS SOFTWARE IN ERROR.
//
/* SOCKET.H--definitions for SOCKET API
 *                                          
 * This header file corresponds to version 1.1 of the Windows Sockets specification.
 *
 * This file includes parts which are Copyright (c) 1982-1986 Regents
 * of the University of California.  All rights reserved.  The
 * Berkeley Software License Agreement specifies the terms and
 * conditions for redistribution.

2004-08-25 gvn IPv6 structures and functions
2003-07-24 gvn add WSA* functions for Winsock compatibility
*/

#ifndef _SOCKETAPI_
#define _SOCKETAPI_

#ifndef D_FAR
#define D_FAR far
#endif

#ifndef __WIN_TYPES
 #define __WIN_TYPES
 typedef unsigned char BYTE;
 typedef unsigned short WORD;
 typedef unsigned long DWORD;
#endif

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | (((WORD)((BYTE)(b))) << 8)))
#endif
#ifndef LOWORD
#define LOWORD(l)           ((WORD)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)(w))
#endif
#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

/*
 * Basic system type definitions, taken from the BSD file sys/types.h.
 */
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;
typedef uint16_t        sa_family_t;
typedef uint16_t        in_port_t;

/*
 * The new type to be used in all
 * instances which refer to sockets.
 */
typedef u_int        SOCKET;

/*
 * Select uses arrays of SOCKETs.  These macros manipulate such
 * arrays.  FD_SETSIZE may be defined by the user before including
 * this file, but the default here should be >= 16.
 *
 * CAVEAT IMPLEMENTOR and USER: THESE MACROS AND TYPES MUST BE
 * INCLUDED IN SOCKET.H EXACTLY AS SHOWN HERE.
 */
#ifndef FD_SETSIZE
#define FD_SETSIZE      16
#endif /* FD_SETSIZE */

typedef struct fd_set {
      u_short fd_count;          /* how many are SET? */
      SOCKET  fd_array[FD_SETSIZE]; /* an array of SOCKETs */
} fd_set;

#ifdef __cplusplus
extern "C" {
#endif
extern int  __WSAFDIsSet(SOCKET, fd_set *);
extern void  __WSAFDSet(SOCKET, fd_set *);
extern void  __WSAFDClr(SOCKET, fd_set *);
#ifdef __cplusplus
}
#endif

#ifdef FD_MACRO_INLINE
#define FD_CLR(fd, set) do { \
   u_int __i; \
   for (__i = 0; __i < ((fd_set *)(set))->fd_count ; __i++) { \
      if (((fd_set *)(set))->fd_array[__i] == fd) { \
         while (__i < ((fd_set *)(set))->fd_count-1) { \
            ((fd_set *)(set))->fd_array[__i] = \
               ((fd_set *)(set))->fd_array[__i+1]; \
            __i++; \
         } \
         ((fd_set *)(set))->fd_count--; \
         break; \
      } \
   } \
} while(0)

#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set *)(set))->fd_count) { \
        if (((fd_set *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set *)(set))->fd_array[__i] = (fd); \
            ((fd_set *)(set))->fd_count++; \
        } \
    } \
} while(0)
#else
#define FD_SET(fd, set) __WSAFDSet((SOCKET)fd, (fd_set *)set)
#define FD_CLR(fd, set) __WSAFDClr((SOCKET)fd, (fd_set *)set)
#endif

#define FD_ZERO(set) (((fd_set *)(set))->fd_count=0)

#define FD_ISSET(fd, set) __WSAFDIsSet((SOCKET)fd, (fd_set *)set)

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct timeval {
      long  tv_sec;     /* seconds */
      long  tv_usec;    /* and microseconds */
};

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)       ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) \
      ((tvp)->tv_sec cmp (uvp)->tv_sec || \
       (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)       (tvp)->tv_sec = (tvp)->tv_usec = 0

/*
 * Commands for ioctlsocket(),   taken from the BSD file fcntl.h.
 *
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#define IOCPARM_MASK 0x7f        /* parameters must be < 128 bytes */
#define IOC_VOID     0x20000000L    /* no parameters */
#define IOC_OUT      0x40000000L    /* copy out parameters */
#define IOC_IN       0x80000000L    /* copy in parameters */
#define IOC_INOUT    (IOC_IN|IOC_OUT)
                              /* 0x20000000 distinguishes new &
                                 old ioctl's */
#define _IO(x,y)     (IOC_VOID|(x<<8)|y)

#define _IOR(x,y,t)  (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)

#define _IOW(x,y,t)  (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)

#define FIONREAD  _IOR('f', 127, u_long) /* get # bytes to read */
#define FIONBIO   _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#define FIOASYNC  _IOW('f', 125, u_long) /* set/clear async i/o */

/* Socket I/O Controls */
#define SIOCSHIWAT   _IOW('s',  0, u_long)  /* set high watermark */
#define SIOCGHIWAT   _IOR('s',  1, u_long)  /* get high watermark */
#define SIOCSLOWAT   _IOW('s',  2, u_long)  /* set low watermark */
#define SIOCGLOWAT   _IOR('s',  3, u_long)  /* get low watermark */
#define SIOCATMARK   _IOR('s',  7, u_long)  /* at oob mark? */

/*
 * Structures returned by network data base library, taken from the
 * BSD file netdb.h.  All addresses are supplied in host order, and
 * returned in network order (suitable for use in system calls).
 */

struct   hostent {
      char  * h_name;         /* official name of host */
      char  * * h_aliases;    /* alias list */
      short h_addrtype;       /* host address type */
      short h_length;         /* length of address */
      char  * * h_addr_list;  /* list of addresses */
#define h_addr h_addr_list[0] /* address, for backward compat */
};

/*
 * It is assumed here that a network number
 * fits in 32 bits.
 */
struct   netent {
      char  * n_name;         /* official name of net */
      char  * * n_aliases;    /* alias list */
      short n_addrtype;       /* net address type */
      u_long   n_net;         /* network # */
};

struct   servent {
      char  * s_name;         /* official service name */
      char  * * s_aliases;    /* alias list */
      short s_port;           /* port # */
      char  * s_proto;        /* protocol to use */
};

struct   protoent {
      char  * p_name;         /* official protocol name */
      char  * * p_aliases;    /* alias list */
      short p_proto;          /* protocol # */
};

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, taken from the BSD file netinet/in.h.
 */

/*
 * Protocols
 */
#define IPPROTO_IP         0           /* dummy for IP */
#define IPPROTO_ICMP       1           /* control message protocol */
#define IPPROTO_GGP        2           /* gateway^2 (deprecated) */
#define IPPROTO_TCP        6           /* tcp */
#define IPPROTO_PUP        12          /* pup */
#define IPPROTO_UDP        17          /* user datagram protocol */
#define IPPROTO_IDP        22          /* xns idp */
#define IPPROTO_ND         77          /* UNOFFICIAL net disk proto */

#define IPPROTO_RAW        255         /* raw IP packet */
#define IPPROTO_MAX        256

/*
 * Port/socket numbers: network standard functions
 */
#define IPPORT_ECHO        7
#define IPPORT_DISCARD     9
#define IPPORT_SYSTAT      11
#define IPPORT_DAYTIME     13
#define IPPORT_NETSTAT     15
#define IPPORT_FTP         21
#define IPPORT_TELNET      23
#define IPPORT_SMTP        25
#define IPPORT_TIMESERVER  37
#define IPPORT_NAMESERVER  42
#define IPPORT_WHOIS       43
#define IPPORT_MTP         57

/*
 * Port/socket numbers: host specific functions
 */
#define IPPORT_TFTP        69
#define IPPORT_RJE         77
#define IPPORT_FINGER      79
#define IPPORT_TTYLINK     87
#define IPPORT_SUPDUP      95

/*
 * UNIX TCP sockets
 */
#define IPPORT_EXECSERVER  512
#define IPPORT_LOGINSERVER 513
#define IPPORT_CMDSERVER   514
#define IPPORT_EFSSERVER   520

/*
 * UNIX UDP sockets
 */
#define IPPORT_BIFFUDP     512
#define IPPORT_WHOSERVER   513
#define IPPORT_ROUTESERVER 520
                              /* 520+1 also used */

/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 */
#define IPPORT_RESERVED    1024

/*
 * Link numbers
 */
#define IMPLINK_IP         155
#define IMPLINK_LOWEXPER   156
#define IMPLINK_HIGHEXPER  158

/*
 * Internet address (old style... should be updated)
 */
struct in_addr {
      union {
            struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
            struct { u_short s_w1,s_w2; } S_un_w;
            u_long S_addr;
      } S_un;
#define s_addr S_un.S_addr
                        /* can be used for most tcp & ip code */
#define s_host S_un.S_un_b.s_b2
                        /* host on imp */
#define s_net  S_un.S_un_b.s_b1
                        /* network */
#define s_imp  S_un.S_un_w.s_w2
                        /* imp */
#define s_impno S_un.S_un_b.s_b4
                        /* imp # */
#define s_lh   S_un.S_un_b.s_b3
                        /* logical host */
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define IN_CLASSA(i)       (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET      0xff000000
#define IN_CLASSA_NSHIFT   24
#define IN_CLASSA_HOST     0x00ffffff
#define IN_CLASSA_MAX      128

#define IN_CLASSB(i)       (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET      0xffff0000
#define IN_CLASSB_NSHIFT   16
#define IN_CLASSB_HOST     0x0000ffff
#define IN_CLASSB_MAX      65536

#define IN_CLASSC(i)       (((long)(i) & 0xc0000000) == 0xc0000000)
#define IN_CLASSC_NET      0xffffff00
#define IN_CLASSC_NSHIFT   8
#define IN_CLASSC_HOST     0x000000ff

#define INADDR_ANY         (u_long)0x00000000
#define INADDR_LOOPBACK    0x7f000001
#define INADDR_BROADCAST   (u_long)0xffffffff
#define INADDR_NONE        0xffffffffL

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
   short sin_family;
   u_short sin_port;
   struct   in_addr sin_addr;
   char  sin_zero[8];
};

typedef struct in6_addr {
   union {
      uint8_t  _S6_u8[16];
      uint16_t _S6_u16[8];
      uint32_t _S6_u32[4];
//    uint64_t _S6_u64[2];
   } _S6_un;
} IN6_ADDR;
#define s6_addr _S6_un._S6_u8

extern const struct in6_addr in6addr_any;
#define IN6ADDR_ANY_INIT {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}

extern const struct in6_addr in6addr_loopback;
#define IN6ADDR_LOOPBACK_INIT {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}}


struct sockaddr_in6 {
   sa_family_t     sin6_family;    /* AF_INET6 */
   in_port_t       sin6_port;      /* transport layer port # */
   uint32_t        sin6_flowinfo;  /* IPv6 traffic class & flow info */
   struct in6_addr sin6_addr;      /* IPv6 address */
   uint32_t        sin6_scope_id;  /* set of interfaces for a scope */
};

typedef struct sockaddr_in6 SOCKADDR_IN6;
typedef struct sockaddr_in6 *PSOCKADDR_IN6;
typedef struct sockaddr_in6 D_FAR *LPSOCKADDR_IN6;

#define SS_PORT(ssp) (((struct sockaddr_in*)(ssp))->sin_port)

struct sockaddr_storage {
   sa_family_t  __ss_family;     /* address family */
   char sa_pad[36];              /* padding to accomodate sockaddr_in6 */
};

typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct sockaddr_storage *PSOCKADDR_STORAGE;
typedef struct sockaddr_storage D_FAR *LPSOCKADDR_STORAGE;

typedef struct addrinfo {
   int     ai_flags;          /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
   int     ai_family;         /* PF_xxx */
   int     ai_socktype;       /* SOCK_xxx */
   int     ai_protocol;       /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
   size_t  ai_addrlen;        /* length of ai_addr */
   char   *ai_canonname;      /* canonical name for nodename */
   struct sockaddr  *ai_addr; /* binary address */
   struct addrinfo  *ai_next; /* next structure in linked list */
} ADDRINFO;

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN  128

typedef struct WSAData {
      WORD              wVersion;
      WORD              wHighVersion;
      char              szDescription[WSADESCRIPTION_LEN+1];
      char              szSystemStatus[WSASYS_STATUS_LEN+1];
      unsigned short    iMaxSockets;
      unsigned short    iMaxUdpDg;
      char *         lpVendorInfo;
} WSADATA;

typedef WSADATA *PWSADATA;


/*
 * Options for use with [gs]etsockopt at the IP level.
 */
#define IP_OPTIONS      1           /* set/get IP per-packet options */

/*
 * Definitions related to sockets: types, address families, options,
 * taken from the BSD file sys/socket.h.
 */

/*
 * This is used instead of -1, since the
 * SOCKET type is unsigned.
 */
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR    (-1)

/*
 * Types
 */
#define SOCK_STREAM     1           /* stream socket */
#define SOCK_DGRAM      2           /* datagram socket */
#define SOCK_RAW        3           /* raw-protocol interface */
#define SOCK_RDM        4           /* reliably-delivered message */
#define SOCK_SEQPACKET  5           /* sequenced packet stream */

/*
 * Option flags per-socket.
 */
#define SO_DEBUG        0x0001         /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002         /* socket has had listen() */
#define SO_REUSEADDR    0x0004         /* allow local address reuse */
#define SO_KEEPALIVE    0x0008         /* keep connections alive */
#define SO_DONTROUTE    0x0010         /* just use interface addresses */
#define SO_BROADCAST    0x0020         /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040         /* bypass hardware when possible */
#define SO_LINGER       0x0080         /* linger on close if data present */
#define SO_OOBINLINE    0x0100         /* leave received OOB data in line */

#define SO_DONTLINGER   (u_int)(~SO_LINGER)

/*
 * Additional options.
 */
#define SO_SNDBUF    0x1001         /* send buffer size */
#define SO_RCVBUF    0x1002         /* receive buffer size */
#define SO_SNDLOWAT  0x1003         /* send low-water mark */
#define SO_RCVLOWAT  0x1004         /* receive low-water mark */
#define SO_SNDTIMEO  0x1005         /* send timeout */
#define SO_RCVTIMEO  0x1006         /* receive timeout */
#define SO_ERROR     0x1007         /* get error status and clear */
#define SO_TYPE      0x1008         /* get socket type */

/*
 * TCP options.
 */
#define TCP_NODELAY  0x0001

/*
 * Address families.
 */
#define AF_UNSPEC    0           /* unspecified */
#define AF_UNIX      1           /* local to host (pipes, portals) */
#define AF_INET      2           /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK   3           /* arpanet imp addresses */
#define AF_PUP       4           /* pup protocols: e.g. BSP */
#define AF_CHAOS     5           /* mit CHAOS protocols */
#define AF_NS        6           /* XEROX NS protocols */
#define AF_ISO       7           /* ISO protocols */
#define AF_OSI       AF_ISO      /* OSI is ISO */
#define AF_ECMA      8           /* european computer manufacturers */
#define AF_DATAKIT   9           /* datakit protocols */
#define AF_CCITT     10          /* CCITT protocols, X.25 etc */
#define AF_SNA       11          /* IBM SNA */
#define AF_DECnet    12          /* DECnet */
#define AF_DLI       13          /* Direct data link interface */
#define AF_LAT       14          /* LAT */
#define AF_HYLINK    15          /* NSC Hyperchannel */
#define AF_APPLETALK 16          /* AppleTalk */
#define AF_NETBIOS   17          /* NetBios-style addresses */
#define AF_INET6     24          /* IPv6 */

#define AF_MAX       25

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr {
      u_short sa_family;         /* address family */
      char  sa_data[14];         /* up to 14 bytes of direct address */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
      u_short sp_family;         /* address family */
      u_short sp_protocol;       /* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC    AF_UNSPEC
#define PF_UNIX      AF_UNIX
#define PF_INET      AF_INET
#define PF_IMPLINK   AF_IMPLINK
#define PF_PUP       AF_PUP
#define PF_CHAOS     AF_CHAOS
#define PF_NS        AF_NS
#define PF_ISO       AF_ISO
#define PF_OSI       AF_OSI
#define PF_ECMA      AF_ECMA
#define PF_DATAKIT   AF_DATAKIT
#define PF_CCITT     AF_CCITT
#define PF_SNA       AF_SNA
#define PF_DECnet    AF_DECnet
#define PF_DLI       AF_DLI
#define PF_LAT       AF_LAT
#define PF_HYLINK    AF_HYLINK
#define PF_APPLETALK AF_APPLETALK
#define PF_INET6     AF_INET6

#define PF_MAX       AF_MAX

/*
 * Structure used for manipulating linger option.
 */
struct   linger {
      u_short l_onoff;           /* option on/off */
      u_short l_linger;          /* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff         /* options for socket level */

/*
 * Maximum queue length specifiable by listen.
 */
#define SOMAXCONN       5

#define MSG_OOB         0x1         /* process out-of-band data */
#define MSG_PEEK        0x2         /* peek at incoming message */
#define MSG_DONTROUTE   0x4         /* send without using routing tables */

#define MSG_MAXIOVLEN   16

/*
 * Define constant based on rfc883, used by gethostbyxxxx() calls.
 */
#define MAXGETHOSTSTRUCT      1024

/*
 * Define flags to be used with the WSAAsyncSelect() call.
 */
#define FD_READ      0x01
#define FD_WRITE     0x02
#define FD_OOB       0x04
#define FD_ACCEPT    0x08
#define FD_CONNECT   0x10
#define FD_CLOSE     0x20

/*
 * All Windows Sockets error constants are biased by WSABASEERR from
 * the "normal"
 */
#define WSABASEERR         10000
/*
 * Windows Sockets definitions of regular Microsoft C error constants
 */
#define WSAEINTR           (WSABASEERR+4)
#define WSAEBADF           (WSABASEERR+9)
#define WSAEACCES          (WSABASEERR+13)
#define WSAEFAULT          (WSABASEERR+14)
#define WSAEEXIST          (WSABASEERR+17)
#define WSAEINVAL          (WSABASEERR+22)
#define WSAEMFILE          (WSABASEERR+24)

/*
 * Windows Sockets definitions of regular Berkeley error constants
 */
#define WSAEWOULDBLOCK     (WSABASEERR+35)
#define WSAEINPROGRESS     (WSABASEERR+36)
#define WSAEALREADY        (WSABASEERR+37)
#define WSAENOTSOCK        (WSABASEERR+38)
#define WSAEDESTADDRREQ    (WSABASEERR+39)
#define WSAEMSGSIZE        (WSABASEERR+40)
#define WSAEPROTOTYPE      (WSABASEERR+41)
#define WSAENOPROTOOPT     (WSABASEERR+42)
#define WSAEPROTONOSUPPORT (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT (WSABASEERR+44)
#define WSAEOPNOTSUPP      (WSABASEERR+45)
#define WSAEPFNOSUPPORT    (WSABASEERR+46)
#define WSAEAFNOSUPPORT    (WSABASEERR+47)
#define WSAEADDRINUSE      (WSABASEERR+48)
#define WSAEADDRNOTAVAIL   (WSABASEERR+49)
#define WSAENETDOWN        (WSABASEERR+50)
#define WSAENETUNREACH     (WSABASEERR+51)
#define WSAENETRESET       (WSABASEERR+52)
#define WSAECONNABORTED    (WSABASEERR+53)
#define WSAECONNRESET      (WSABASEERR+54)
#define WSAENOBUFS         (WSABASEERR+55)
#define WSAEISCONN         (WSABASEERR+56)
#define WSAENOTCONN        (WSABASEERR+57)
#define WSAESHUTDOWN       (WSABASEERR+58)
#define WSAETOOMANYREFS    (WSABASEERR+59)
#define WSAETIMEDOUT       (WSABASEERR+60)
#define WSAECONNREFUSED    (WSABASEERR+61)
#define WSAELOOP           (WSABASEERR+62)
#define WSAENAMETOOLONG    (WSABASEERR+63)
#define WSAEHOSTDOWN       (WSABASEERR+64)
#define WSAEHOSTUNREACH    (WSABASEERR+65)
#define WSAENOTEMPTY       (WSABASEERR+66)
#define WSAEPROCLIM        (WSABASEERR+67)
#define WSAEUSERS          (WSABASEERR+68)
#define WSAEDQUOT          (WSABASEERR+69)
#define WSAESTALE          (WSABASEERR+70)
#define WSAEREMOTE         (WSABASEERR+71)

/*
 * Extended Windows Sockets error constant definitions
 */
#define WSASYSNOTREADY     (WSABASEERR+91)
#define WSAVERNOTSUPPORTED (WSABASEERR+92)
#define WSANOTINITIALISED  (WSABASEERR+93)

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (when using the resolver). Note that these errors are
 * retrieved via WSAGetLastError() and must therefore follow
 * the rules for avoiding clashes with error numbers from
 * specific implementations or language run-time systems.
 * For this reason the codes are based at WSABASEERR+1001.
 * Note also that [WSA]NO_ADDRESS is defined only for
 * compatibility purposes.
 */

#define h_errno      WSAGetLastError()

/* Authoritative Answer: Host not found */
#define WSAHOST_NOT_FOUND  (WSABASEERR+1001)
#define HOST_NOT_FOUND     WSAHOST_NOT_FOUND

/* Non-Authoritative: Host not found, or SERVERFAIL */
#define WSATRY_AGAIN       (WSABASEERR+1002)
#define TRY_AGAIN          WSATRY_AGAIN

/* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define WSANO_RECOVERY     (WSABASEERR+1003)
#define NO_RECOVERY        WSANO_RECOVERY

/* Valid name, no data record of requested type */
#define WSANO_DATA         (WSABASEERR+1004)
#define NO_DATA            WSANO_DATA

/* no address, look for MX record */
#define WSANO_ADDRESS      WSANO_DATA
#define NO_ADDRESS         WSANO_ADDRESS

/* UNIX definitions - conflicts with DOS!
#define EINTR           4
#define EBADF           9
#define EACCES          13
#define EFAULT          14
#define EEXIST          17
#define EINVAL          22
#define EMFILE          24
*/

/*
 * Windows Sockets errors redefined as regular Berkeley error constants
 */
#define EWOULDBLOCK        WSAEWOULDBLOCK
#define EINPROGRESS        WSAEINPROGRESS
#define EALREADY           WSAEALREADY
#define ENOTSOCK           WSAENOTSOCK
#define EDESTADDRREQ       WSAEDESTADDRREQ
#define EMSGSIZE           WSAEMSGSIZE
#define EPROTOTYPE         WSAEPROTOTYPE
#define ENOPROTOOPT        WSAENOPROTOOPT
#define EPROTONOSUPPORT    WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT    WSAESOCKTNOSUPPORT
#define EOPNOTSUPP         WSAEOPNOTSUPP
#define EPFNOSUPPORT       WSAEPFNOSUPPORT
#define EAFNOSUPPORT       WSAEAFNOSUPPORT
#define EADDRINUSE         WSAEADDRINUSE
#define EADDRNOTAVAIL      WSAEADDRNOTAVAIL
#define ENETDOWN           WSAENETDOWN
#define ENETUNREACH        WSAENETUNREACH
#define ENETRESET          WSAENETRESET
#define ECONNABORTED       WSAECONNABORTED
#define ECONNRESET         WSAECONNRESET
#define ENOBUFS            WSAENOBUFS
#define EISCONN            WSAEISCONN
#define ENOTCONN           WSAENOTCONN
#define ESHUTDOWN          WSAESHUTDOWN
#define ETOOMANYREFS       WSAETOOMANYREFS
#define ETIMEDOUT          WSAETIMEDOUT
#define ECONNREFUSED       WSAECONNREFUSED
//#define ELOOP            WSAELOOP
//#define ENAMETOOLONG     WSAENAMETOOLONG
#define EHOSTDOWN          WSAEHOSTDOWN
#define EHOSTUNREACH       WSAEHOSTUNREACH
//#define ENOTEMPTY           WSAENOTEMPTY
#define EPROCLIM           WSAEPROCLIM
#define EUSERS             WSAEUSERS
#define EDQUOT             WSAEDQUOT
#define ESTALE             WSAESTALE
#define EREMOTE            WSAEREMOTE

/* Socket function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

SOCKET  accept (SOCKET s, struct sockaddr *addr,
                    int *addrlen);
int   bind (SOCKET s, const struct sockaddr *addr, int namelen);
int   closesocket (SOCKET s);
int   connect (SOCKET s, const struct sockaddr *name, int namelen);
int   ioctlsocket (SOCKET s, long cmd, u_long *argp);
int   getpeername (SOCKET s, struct sockaddr *name,
                     int * namelen);
int   getsockname (SOCKET s, struct sockaddr *name,
                     int * namelen);
int   getsockopt (SOCKET s, int level, int optname,
                     char * optval, int *optlen);
u_long   htonl (u_long hostlong);
u_short   htons (u_short hostshort);
unsigned long   inet_addr (const char * cp);
char *   inet_ntoa (struct in_addr in);
int   listen (SOCKET s, int backlog);
u_long   ntohl (u_long netlong);
u_short   ntohs (u_short netshort);
int   recv (SOCKET s, char * buf, int len, int flags);
int   recvfrom (SOCKET s, char * buf, int len, int flags,
                   struct sockaddr *from, int * fromlen);
int   select (int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, const struct timeval *timeout);
int   send (SOCKET s, const char * buf, int len, int flags);
int   sendto (SOCKET s, const char * buf, int len, int flags,
                  const struct sockaddr *to, int tolen);
int   setsockopt (SOCKET s, int level, int optname,
                     const char * optval, int optlen);
int   shutdown (SOCKET s, int how);
SOCKET   socket (int af, int type, int protocol);

/* Database function prototypes */

struct hostent *   gethostbyaddr(const char * addr,
                                   int len, int type);
struct hostent *   gethostbyname(const char * name);
int   gethostname (char * name, int namelen);
struct servent *   getservbyport(int port, const char * proto);
struct servent *   getservbyname(const char * name,
                                   const char * proto);
struct protoent *   getprotobynumber(int proto);
struct protoent *   getprotobyname(const char * name);

// Microsoft Windows Extension function prototypes
int WSAStartup(WORD wVersionRequired, PWSADATA psWSAData);
int WSACleanup(void);
void WSASetLastError(int iError);
int WSAGetLastError(void);

// Debug functions
int StartDebug(char *pszFileName);
int StopDebug(void);

// IPv6 functions
int getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints,
                      struct addrinfo **res);

/* Flags used in "hints" argument to getaddrinfo() */

#define AI_PASSIVE     1  /* Socket address will be used in bind() call */
#define AI_CANONNAME   2  /* Return canonical name in first ai_canonname */
#define AI_NUMERICHOST 4  /* Nodename must be a numeric address string */

void freeaddrinfo(struct addrinfo *ai);
char *gai_strerror(int ecode);

const char *inet_ntop(int af, const void *src,
                            char *dst, size_t size);
#define INET_ADDRSTRLEN    16
#define INET6_ADDRSTRLEN   46

#ifdef __cplusplus
}
#endif

/* Microsoft Windows Extended data types */
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr D_FAR *LPSOCKADDR;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in D_FAR *LPSOCKADDR_IN;

typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger D_FAR *LPLINGER;

typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr D_FAR *LPIN_ADDR;

typedef struct fd_set FD_SET;
typedef struct fd_set *PFD_SET;
typedef struct fd_set D_FAR *LPFD_SET;

typedef struct hostent HOSTENT;
typedef struct hostent *PHOSTENT;
typedef struct hostent D_FAR *LPHOSTENT;

typedef struct servent SERVENT;
typedef struct servent *PSERVENT;
typedef struct servent D_FAR *LPSERVENT;

typedef struct protoent PROTOENT;
typedef struct protoent *PPROTOENT;
typedef struct protoent D_FAR *LPPROTOENT;

typedef struct timeval TIMEVAL;
typedef struct timeval *PTIMEVAL;
typedef struct timeval D_FAR *LPTIMEVAL;

#include "netdb.h"
#endif   /* _SOCKETAPI_ */

