//  $Id: CAPI.C 1.18 2006/04/13 22:22:02Z johnb Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 1996-2005 Datalight, Inc.
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
/************************************************************************
   CAPI.C

   Copyright (c) 1996-2008 Datalight, Inc.
   Confidential and Proprietary
   All Rights Reserved

   Provides the Sockets Compatible API (CAPI)


   History
05/26/2008  gvn documentation changes
02/11/2008  gvn reworked DPMI 32 bit API to 16 bit stack
04/13/2006  gvn fix ConfigCommand
01/17/2006  gvn implement ConfigCommand for non-TSR versions
07/06/2005  gvn make room for future expansion of internal structures
01/17/2005  gvn GetAddressInfo() resolves ScopeId
12/06/2004  gvn Change conditional compile defines for easier maintenance
08/10/2004  gvn GetAddressInfo(), ICMPPingEx(), GetAddressEx()
11/18/2003  gvn fix ResolveName(255.255.x.y) failing, Route API
10/22/2003  gvn add ArpApi(ARP_API_ADD,..) and ProprietaryApi()
04/01/2003  gvn conditionals to create library
30/11/2002  gvn (re)fixed return value for SetAsyncNotification
10/03/2001 jmb added DJGPP support
        -------
   1.00 1999-03-18 Starting version control, code usable for Microsoft
      or Borland compilers
   1.01 1999-04-29 fix set_opt() parameter
   1.02 1999-05-27 New version using Datalight Coding guidelines
   1.03 2000-07-11 Added MSVC compatibality
   1.10 2000-11-29 pkg Extensive changes to make the code more portable
   1.11 2000-12-15 RWK added function headers
 ************************************************************************
To only compile specific modules, define _CAPI_LIBRARY and only the
specific module(s).
*/

#ifndef IPV6
#define IPV6
#endif

#ifndef _CAPI_LIBRARY
#define _CAPI_GETSOCKV
#define _CAPI_DISABLEA
#define _CAPI_ENABLEAS
#define _CAPI_ISSOCKET
#define _CAPI_GETDCSOC
#define _CAPI_GETSOCKE
#define _CAPI_GETVERSI
#define _CAPI_GETIPVFL
#define _CAPI_CONVERTD
#define _CAPI_EOFSOCKT
#define _CAPI_FLUSHSOC
#define _CAPI_RELEASES
#define _CAPI_RELEASED
#define _CAPI_ABORTSOC
#define _CAPI_ABORTDCS
#define _CAPI_SHUTDOWN
#define _CAPI_GETADDRE
#define _CAPI_GETPEERA
#define _CAPI_GETKERNI
#define _CAPI_ICMPPING
#define _CAPI_GETKERNC
#define _CAPI_GETNETIN
#define _CAPI_CONNECTS
#define _CAPI_LISTENSO
#define _CAPI_LISTENAC
#define _CAPI_ACCEPTSO
#define _CAPI_SELECTSO
#define _CAPI_READSOCK
#define _CAPI_READFROM
#define _CAPI_WRITESOC
#define _CAPI_WRITETOS
#define _CAPI_SETALARM
#define _CAPI_SETASYNC
#define _CAPI_RESOLVEN
#define _CAPI_PARSEADD
#define _CAPI_SETSOCKE
#define _CAPI_JOINGROU
#define _CAPI_LEAVEGRO
#define _CAPI_JOINGREX
#define _CAPI_LEAVEGRE
#define _CAPI_IFACEIOC
#define _CAPI_ARPAPI
#define _CAPI_ASYNCNOT
#define _CAPI_DPMICBRE
#define _CAPI_PROAPI
#define _CAPI_ROUTE
#define _CAPI_GTADDRIN
#define _CAPI_GTADDREX
#define _CAPI_ICMPPNGX
#define _CAPI_GETBUSYF
#define _CAPI_GTPEEREX
#define _CAPI_GETADCOM
#if defined(_SOCK32) || defined(_SOCKLIB)
#define _CAPI_CONFIGCOMMAND
#endif
#endif

#include <dos.h>
#include <string.h>
#include <stdlib.h>

#include "compiler.h"
#include "capi.h"
#include "api.h"

/*
int GetSocketsVersion(void)
   Parameters
   none

   Description
   Returns the version number of the Sockets API

   Returns
   On success, the version number of the Sockets API
   On error, -1 if the Sockets API is not found
*/
#ifdef _CAPI_GETSOCKV
int GetSocketsVersion(void)
{
   X86Regs r;

   /* Set the function number to call */
   r.r.h.ah = GET_SOCKETS_VERSION;
   return CallSocketsDosApi(&r);
}
#endif

/*
   Parameters
   None

   Description
   Disables Asynchronous notifications (callbacks).

   Returns
   On success, 0 for disabled, 1 for enabled.
   On error, -1 with both iNetErrNo and iNetSubErr set
*/
#ifdef _CAPI_DISABLEA
int DisableAsyncNotification(void)
{
   X86Regs r;

   /* Set the function number to call */
   r.r.h.ah = DISABLE_ASYNC_NOTIFICATION;
   return CallSocketsDosApi(&r);
}
#endif

/*
   Parameters
   None

   Description
   Enables asynchronous notifications (callbacks).

   Returns
   On success, previous state of notification
   On error, -1 with both iNetErrNo and iNetSubErr set
*/
#ifdef _CAPI_ENABLEAS
int EnableAsyncNotification(void)
{
   X86Regs r;
   r.r.h.ah = ENABLE_ASYNC_NOTIFICATION;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Dos Compatible socket handle for the connection

   Description
   Checks a Dos Compatible socket for validity.

   Returns
   On success, 0
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_ISSOCKET
int IsSocket(int iSocket)
{
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = IS_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   None

   Description
   Gets a DOS-compatible socket handle. This function calls DOS to
   open a DOS file handle.

   Returns
   On success, socket handle.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_GETDCSOC
int GetDCSocket(void)
{
   X86Regs r;

   r.r.h.ah = GET_DC_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   None

   Description
   Gets a socket handle.

   Returns
   On success, socket handle.
   On error, -1 with iNetErrNo containing the error code.

*/
#ifdef _CAPI_GETSOCKE
int GetSocket(void)
{
   X86Regs r;

   r.r.h.ah = GET_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   None

   Description
   Gets the version number of the Compatible API.

   Returns
   On success, 0x214.
   On error,  -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_GETVERSI
int GetVersion(void)
{
   X86Regs r;

   r.r.h.ah = GET_NET_VERSION;
   return CallSocketsDosApi(&r);
}
#endif

#ifdef _CAPI_GETIPVFL
int GetIpVersionFlags(void)
{
   int iRet = -1;
   struct {
      VARBLOCK sVar;    // Variable block
      char cSpare[20];  // for future expansion
   } sV;
   struct {
      struct ip_stats sIpStat;
      char cSpare[20];  // for future expansion
   } sI;
   unsigned uLen = sizeof(sV);

   if (GetKernelInformation(0, K_INF_VARBLOCK, 0, &sV, &uLen) < 0)
      return iRet;
   uLen = sizeof(sI);
   if (GetKernelInformation(sV.sVar.ip_stats, K_INF_MEMBLOCK, 0, &sI, &uLen) < 0)
      return iRet;
   iRet = (sI.sIpStat.bIpAddrLen == 16) ? IPVF_IPV6 : 0;
   if (sI.sIpStat.wRoute)
      iRet |= IPVF_IPV4;
   return iRet;
}
#endif
/*
   Parameters
   none

   Description
   Changes a Dos Compatible socket handle into a normal socket handle.
   This function calls DOS to close a DOS file handle.

   Returns
   On success, returns socket handle.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_CONVERTD
int ConvertDCSocket(int iSocket)
{
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = CONVERT_DC_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   Description
   Closes the STREAM (TCP) connection (sends a FIN).
   After EofSocket() has been called, no WriteSocket() calls may be made.
   The socket, however, remains open for reading until the peer closes the
   connection.

   Returns
   On success, 0.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_EOFSOCKT
int EofSocket(int iSocket)
{
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = EOF_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   Description
   Flushes any output data still queued for a TCP connection.

   Returns
   On success, 0
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_FLUSHSOC
int FlushSocket(int iSocket)
{
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = FLUSH_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   Description
   Closes connection and release all resources. On a STREAM (TCP) connection,
   this function should only be called once the connection has been closed
   from both sides otherwise a reset (ungraceful close) can result.

   Returns
   On success, socket handle.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_RELEASES
int ReleaseSocket(int iSocket)
{
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = RELEASE_SOCKET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   Description
   Closes all connections and releases all resources associated with
   Dos Compatible sockets.

   Returns
   On success, 0.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_RELEASED
int ReleaseDCSockets(void)
{
   X86Regs r;

   r.r.h.ah = RELEASE_DC_SOCKETS;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   Description
   Aborts the network connection and releases all resources.
   This function causes an unpredictable close (reset) on a STREAM connection.

   Returns
   On success, returns socket handle.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_ABORTSOC
int AbortSocket(int iSocket)
{
   X86Regs r;

   r.r.h.ah = ABORT_SOCKET;
   r.r.x.bx = iSocket;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   None

   Description
   Aborts all Dos Compatible socket connections.

   Returns
   On success, 0.
   On error, -1 with iNetErrNo containing the error code.

*/
#ifdef _CAPI_ABORTDCS
int AbortDCSockets(void)
{
   X86Regs r;

   r.r.h.ah = ABORT_DC_SOCKETS;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   None

   Description
   Shuts down the network and unloads the Sockets TCP/IP kernel.

   Returns
   On success, 0.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_SHUTDOWN
int ShutDownNet(void)
{
   X86Regs r;

   r.r.h.ah = SHUT_DOWN_NET;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket, socket number to check

   Description
   Gets the local IP address of a connection.  In the case of
   a single interface host, this is the IP address of the host.
   In the case of more than one interface, the IP address of
   the interface being used to route the traffic for the
   specific connection is given.

   Returns
   On success, IP address.
   On error, zero with iNetErrNo containg the error code.
*/
#ifdef _CAPI_GETADDRE
DWORD GetAddress(int iSocket)
{
   DWORD dwAddress;
   X86Regs r;

   r.r.x.bx = iSocket;
   r.r.h.ah = GET_ADDRESS;

   if (CallSocketsDosApi(&r) == -1)
      return 0L;

   dwAddress = (((DWORD) r.r.x.dx << 16) + (WORD)r.r.x.ax);
   return dwAddress;
}
#endif

/*
The GetAddressEx() function gets the local IP address of a connection using
either IPv4 or IPv6. Note that the information may change during the lifetime
of a connection and is generally only valid once data has been sent on the
connection.
C syntax
   int GetAddressEx(int iSocket, NET_ADDR *pAddr);
Options
iSocket
Socket handle for the connection.
pAddr
Pointer to NET_ADDR structure to receive information.
Return values
Returns 0 and NET_ADDR structure filled in on success. The length of the IP
address is returned in pAddr->dwRemoteHost - 4 for IPv4 and 16 for IPv6. The
IP address is returned in  pAddr->sIpAddr. Note that for IPv4 only 4 address
bytes will be returned; it is therefore a good practice to fill the
NET_ADDR structure with zeroes before calling GetAddressEx().
Returns -1 with iNetErrNo  containing the error on failure
*/
#ifdef _CAPI_GTADDREX
int GetAddressCommon(int iSocket, NET_ADDR *psAddr, BYTE bFunc);
int GetAddressEx(int iSocket, NET_ADDR *psAddr)
{
   return GetAddressCommon(iSocket,psAddr,GET_ADDRESS_EX);
}
#endif

/*
The GetAddressInfo() function resolves a symbolic IP address. All the methods
for which the corresponding bit is set in the flags word, will be tried until
the name is resolved or the methods exhausted. The order in which the methods
will be used is the same as the order in which the methods are described below
except when DNS_IPV4, DNS_IPV6 and DNS_IPV6_FIRST are all set, in which case
DNS_IPV6 will be tried before DNS_IPV4. Not setting AI_HOSTTAB will remove the
constraint that DOS may be called.
C syntax
   int GetAddressInfo(char *pszName,WORD wFlags, NET_ADDR *psAddress);
Options
pszName
Pointer to string containing symbolic name.
wFlags
Flag bits specifying method of resolution:
AI_PARSE Parse numeric name (IPv4 dotted decimal, IPv6 hex notation).
AI_HOSTTAB  Use host table to resolve (HOSTS file).
DNS_IPV4 Use DNS to resolve IPv4 address (Record type A).
DNS_IPV6 Use DNS to resolve IPv6 address (Record type AAAA).
DNS_IPV6_FIRST Try IPv6 first if both DNS_IPV4 and DNS_IPV6 are set.
AI_NOBLOCK Do not block on DNS lookup
AI_ABORT Abort a non-blocking request
psAddress
Pointer to NET_ADDR structure to receive IP address.
Return value
   Returns length of IP address (4 or 16) on success,
   0 on failed lookup with iNetErrNo containing ERR_HOST_UNKNOWN
   -1 on error with iNetErrNo containing ERR_WOULD_BLOCK when AI_NOBLOCK is set
   or ERR_NO_MEM if the name is >= 60 characters long or memory is depleted.
*/
#ifdef _CAPI_GTADDRIN
int GetAddressInfo(char *pszName,unsigned uFlags, NET_ADDR *psAddress)
{
   int iRet;
   X86Regs r;
   char *psz;
   DPMIINFO_STRUCT (sAddress);
   DPMIINFO_STRUCT (sName);
   DPMI_INT (iLen);

   if ((psz = strrchr(pszName,'%')) != 0) {
      *psz++ = 0;
      psAddress->dwScopeId = (DWORD)atoi(psz);
   }
   else
      psAddress->dwScopeId = 0;
   DOSBUF_ENTER((iLen = strlen(pszName) + 1) + sizeof(NET_ADDR),0);
   DOSBUF_SETUP(sAddress,psAddress,sizeof(NET_ADDR),1);
   DOSBUF_SETUP(sName,pszName,iLen,1);

   r.r.x.bx = uFlags;
   r.r.h.ah = GET_ADDR_INFO;

   r.r.x.dx = REALMODE_OFFSETC(sName,pszName);
   SET_SEGMENTC(r._s.ds,sName,pszName);
   r.r.x.di = REALMODE_OFFSETC(sAddress,psAddress);
   SET_SEGMENTC(r._s.es,sAddress,psAddress);
   iRet = CallSocketsDosApi(&r);

   DOSBUF_GET(sAddress, psAddress, sizeof(NET_ADDR));
   DOSBUF_EXIT();

   // return address
   return iRet;
}
#endif

#if defined(_CAPI_GETBUSYF) && !defined(__DJGPP__)
WORD D_FAR *GetBusyFlag(void)
{
   X86Regs r;

   /* Set the function number to call */
   r.r.h.ah = GET_BUSY_FLAG;
   CallSocketsDosApi(&r);
   return (WORD D_FAR *)MK_FP(r._s.es,r.r.x.si);
}
#endif
#ifdef _CAPI_GETPEERA
int GetAddressCommon(int iSocket, NET_ADDR *psAddr, BYTE bFunc);
int GetPeerAddress(int iSocket, NET_ADDR *psAddr)
{
   return GetAddressCommon(iSocket,psAddr,GET_PEER_ADDRESS);
}
#endif

#ifdef _CAPI_GTPEEREX
int GetAddressCommon(int iSocket, NET_ADDR *psAddr, BYTE bFunc);
int GetPeerAddressEx(int iSocket, NET_ADDR *psAddr)
{
   return GetAddressCommon(iSocket,psAddr,GET_PEER_ADDRESS_EX);
}
#endif

/*
   Common GetAddress() function
*/
#ifdef _CAPI_GETADCOM
int GetAddressCommon(int iSocket, NET_ADDR *psAddr, BYTE bFunc)
{
   int iReturn;
   X86Regs r;
   DPMIINFO_STRUCT (sAddr);

   DOSBUF_ENTER(sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sAddr,psAddr,sizeof(NET_ADDR),0);
   SET_SEGMENT(r._s.ds,sAddr,psAddr);
   r.r.x.dx = REALMODE_OFFSET(sAddr,psAddr);
   r.r.x.bx = iSocket;
   r.r.h.ah = bFunc;
   iReturn = CallSocketsDosApi(&r);

   DOSBUF_GET(sAddr, psAddr, sizeof(NET_ADDR));
   DOSBUF_EXIT();

   return iReturn;
}
#endif

/*
   Parameters
   iReserved
   Reserved value, set to zero.

   bCode
   Code specifying kernel info to retrieve:
   K_INF_HOST_TABLE  Gets name of file containing host table.
   K_INF_DNS_SERVERS Gets IP addresses of DNS Servers.
   K_INF_TCP_CONS Gets number of Sockets (DC + normal).
   K_INF_BCAST_ADDR  Gets broadcast IP address.
   K_INF_IP_ADDR  Gets IP address of first interface.
   K_INF_SUBNET_MASK Gets netmask of first interface.

   pData
   Pointer to data area to receive kernel information.

   puSize
   Pointer to WORD containing length of data area.

   Description
   Gets specified information from the kernel.

   Returns
   On success returns 0 with data area and size word filled in.
   On error, -1 with iNetErrNo containing the error on failure.
*/
#ifdef _CAPI_GETKERNI
int GetKernelInformation(int iReserved, BYTE bCode, BYTE bDevID,
                   void *pData, unsigned *puSize)
{
   int iReturn;
   X86Regs r;
   DPMIINFO_STRUCT (sSize);
   DPMIINFO_STRUCT (sData);

   DOSBUF_ENTER(*puSize + sizeof(unsigned),-1);
   DOSBUF_SETUP(sData,pData,*puSize,1);
   DOSBUF_SETUP(sSize,puSize,sizeof(unsigned),1);

   r.r.x.si = REALMODE_OFFSET(sData,pData);
   SET_SEGMENTC(r._s.ds,sData,pData);

   r.r.x.di = REALMODE_OFFSET(sSize,puSize);
   SET_SEGMENTC(r._s.es,sSize,puSize);

   r.r.x.bx = iReserved;
   r.r.x.dx = ((WORD)bCode << 8) + bDevID;
   r.r.x.ax = GET_KERNEL_INFO << 8;
   iReturn = CallSocketsDosApi(&r);

   if (iReturn != -1)
   {
      DOSBUF_GET(sSize, puSize, sizeof(unsigned));
      DOSBUF_GET(sData, pData, *puSize);
   }
   DOSBUF_EXIT();

   return iReturn;
}
#endif
/*
int ICMPPing(DWORD dwHost, int iLength)
   Parameters
   dwHost
   IP address of host to ping.

   iLength
   Number of data bytes in ping request.

   Description
   Sends an ICMP ping (echo request) and waits until a response is received
   or for six seconds if no response is received.  ICMPPing() is always a
   blocking function.

   Returns
   On success, 0.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_ICMPPING
int ICMPPing(DWORD dwHost, int iLength)
{
   X86Regs r;

   SET_32BITREG(r.r.x.dx,r.r.x.bx,dwHost);
   r.r.x.cx = iLength;
   r.r.x.ax = ICMP_PING << 8;
   return CallSocketsDosApi(&r);
}
#endif
/*
int ICMPPingEx(NET_ADDR *psHost, WORD wSequence, WORD wWait, int iLength)
   Parameters
   psHost
      Pointer to NET_ADDR structure containing IP address of host to ping.
   wSequence
      Sequence number to send. Any received non-matching echo replies will be
      ignored and flushed.
   wWait
      Time in milliseconds to block while waiting for a response. Can be set
      to zero for non-blocking operation.
   iLength
      Number of data bytes in ping request. If iLength < 0, do not send echo
      request.

   Description
      The ICMPPingEx() function sends an ICMP ping (echo request) to either an
      IPv4 or IPv6 host passed in the NET_ADDR structure pointed to by psHost
      and waits until a response is received or for a time specified in wWait
      if no response is received.  ICMPPingEx() blocks while waiting for a
      response. If  wWait is set to zero, ICMPPingEx() checks for a response
      once and if not received yet, returns an error with iNetErrNo  containing
      ERR_TIMEOUT . In order to again check for a response without sending an
      echo request, iLength can be set to a negative value.
   Returns
      On success, 0.
      On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_ICMPPNGX
int ICMPPingEx(NET_ADDR *psHost, unsigned uSequence, unsigned uWait, int iLength)
{
   int iRet;
   X86Regs r;
   DPMIINFO_STRUCT (sHost);

   DOSBUF_ENTER(sizeof(NET_ADDR),0);
   DOSBUF_SETUP(sHost,psHost,sizeof(NET_ADDR),1);

   r.r.x.bx = uSequence;
   r.r.x.cx = iLength;
   r.r.x.dx = uWait;
   r.r.x.ax = ICMP_PING_EX << 8;
   r.r.x.si = REALMODE_OFFSET(sHost,psHost);
   SET_SEGMENTC(r._s.ds,sHost,psHost);
   iRet = CallSocketsDosApi(&r);
   DOSBUF_GET(sHost, psHost, sizeof(NET_ADDR));
   DOSBUF_EXIT();
   return iRet;
}
#endif
/*
   Parameters
   psKc
   Pointer to KERNEL_CONFIG structure.
   bKMaxTcp Number of TCP sockets allowed.
   BKMaxUdp Number of UDP sockets allowed.
   bKMaxIp  Number of IP sockets allowed (0).
   bKMaxRaw Number of RAW_NET sockets allowed (0).
   bKActTcp Number of TCP sockets in use.
   bKActUdp Number of UDP sockets in use.
   bKActIp  Number of IP sockets in use (0).
   bKActRaw Number of RAW_NET sockets in use (0).
   wKActDCS Number of active Dos Compatible Sockets.
   wKActSoc Number of active normal Sockets.
   bKMaxLnh Maximum header on an attached network.
   bKMaxLnt Maximum trailer on an attached network.
   bKLBUF_SIZE Size of a large packet buffer.
   bKNnet   Number of network interfaces attached.
   dwKCticks   Milliseconds since kernel started.
   dwKBroadcast   IP broadcast address in use.

   Description
   Gets the kernel configuration.

   Returns
   On success, 0 with KERNEL_CONFIG structure filled in.
   On error,  -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_GETKERNC
int GetKernelConfig(KERNEL_CONFIG *psKC)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sKC);

   DOSBUF_ENTER(sizeof(KERNEL_CONFIG),-1);
   DOSBUF_SETUP(sKC,psKC,sizeof(KERNEL_CONFIG),0);
   SET_SEGMENT(r._s.ds,sKC,psKC);
   r.r.x.si = REALMODE_OFFSET(sKC,psKC);
   r.r.x.ax = GET_KERNEL_CONFIG << 8;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sKC, psKC, sizeof(KERNEL_CONFIG));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   psNI
   Pointer to NET_INFO structure.  The following members of NET_INFO are
   obtained:
   DwIPAddress
   dwIPSubnet
   iUp
   iLanLen
   pLanAddr

   Description
   Gets information about the network.

   Returns
   On success, 0 with NET_INFO structure filled .
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_GETNETIN
int GetNetInfo(int iSocket, NET_INFO *psNI)
{
   X86Regs r;
   int iReturn;

   DPMIINFO_STRUCT (sNI);

   DOSBUF_ENTER(sizeof(NET_INFO),-1);
   DOSBUF_SETUP(sNI,psNI,sizeof(NET_INFO),0);
   SET_SEGMENT(r._s.ds,sNI,psNI);
   r.r.x.si = REALMODE_OFFSET(sNI,psNI);
   r.r.x.bx = iSocket;
   r.r.x.ax = GET_NET_INFO << 8;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sNI, psNI, sizeof(NET_INFO));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   iType
   Type of connection: STREAM or DATAGRAM.

   psAddr
   Pointer to NET_ADDR structure.

   Description
   Makes a network connection. If iSocket is specified as -1, a
   Dos Compatible socket is assigned.  In this case only, DOS is called
   to open a file handle.  If iSocket specifies a non-blocking socket or
   iType specifies a DATAGRAM connection, this call returns immediately.
   In the case of a STREAM connection, the connection may not yet be
   established.  ReadSocket() can be used to test for connection
   establishment. As long as ReadSocket() returns an ERR_NOT_ESTAB code,
   the connection is not established. A good return or an error return
   with ERR_WOULD_BLOCK indicates an establishedconnection. A more complex
   method uses SetAsyncNotify() with NET_AS_OPEN to test for connection
   establishment. NET_AS_ERROR should also be set to be notified of a
   failed open attempt.

   Returns
   On success, socket.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_CONNECTS
int ConnectSocket(int iSocket, int iType, NET_ADDR *psAddr)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sAddr);

   psAddr->bProtocol = 6;
   DOSBUF_ENTER(sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sAddr,psAddr,sizeof(NET_ADDR),1);
   SET_SEGMENT(r._s.ds,sAddr,psAddr);
   r.r.x.si = REALMODE_OFFSET(sAddr,psAddr);
   r.r.x.bx = iSocket;
   r.r.x.dx = iType;
   r.r.x.ax = CONNECT_SOCKET << 8;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sAddr, psAddr, sizeof(NET_ADDR));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   iType
   Type of connection:  STREAM or DATAGRAM.

   psAddr
   Pointer to NET_ADDR structure.

   Description
   Listens for a network connection. If iSocket is specified as -1, a Dos Compatible
   socket is assigned. In this case only, DOS is called to open a file handle.
   If iSocket specifies a non-blocking socket or iType specifies a DATAGRAM connection,
   this call returns immediately.  In the case of a STREAM connection, the connection
   may not be established yet.  ReadSocket() can be used to test for connection establishment.
   As long as ReadSocket() returns an ERR_NOT_ESTAB code, the connection is not established.
   A good return or an error return with ERR_WOULD_BLOCK indicates connection establishment.
   A more complex method is to use SetAsyncNotify() with NET_AS_OPEN to test for connection
   establishment.  NET_AS_ERROR should also be set to be notified of a failed open attempt.

   Returns
   On success, returns socket handle on success.
   On error, -1 on failure with iNetErrNo containing the error code.
*/
#ifdef _CAPI_LISTENSO
int ListenSocket(int iSocket, int iType, NET_ADDR *psAddr)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sAddr);

   psAddr->bProtocol = 6;
   DOSBUF_ENTER(sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sAddr,psAddr,sizeof(NET_ADDR),1);
   SET_SEGMENT(r._s.ds,sAddr,psAddr);
   r.r.x.si = REALMODE_OFFSET(sAddr,psAddr);
   r.r.x.bx = iSocket;
   r.r.x.dx = iType;
   r.r.h.ah = LISTEN_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sAddr, psAddr, sizeof(NET_ADDR));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   iType
   Type of connection:  STREAM or DATAGRAM.

   iConnections
   Number of connections to be queued pending AcceptSocket().

   psAddr
   Pointer to NET_ADDR structure.

   Description
   Listens for network connections. If iSocket is specified as -1, a Dos Compatible
   socket is assigned. In this case only, DOS is called to open a file handle.
   This call returns immediately. If iType specifies a DATAGRAM connection, this call
   acts exactly the same as a ListenSocket(). If iType specifies a STREAM connection,
   AcceptSocket() must be used to accept incoming connections on iSocket, which will
   remain listening for new connections. Up to iConnections incoming connections may be
   received before an AcceptSocket() must be issued to prevent further connections
   to be refused.

   Returns
   Success: returns socket handle on success.
   Error: -1 on failure with iNetErrNo containing the error code.
*/
#ifdef _CAPI_LISTENAC
int ListenAcceptSocket(int iSocket, int iType, int iConnections, NET_ADDR *psAddr)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sAddr);

   psAddr->bProtocol = 6;
   DOSBUF_ENTER(sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sAddr,psAddr,sizeof(NET_ADDR),1);
   SET_SEGMENT(r._s.ds,sAddr,psAddr);
   r.r.x.si = REALMODE_OFFSET(sAddr,psAddr);
   r.r.x.bx = iSocket;
   r.r.x.cx = iConnections;
   r.r.x.dx = iType;
   r.r.h.ah = LISTEN_ACCEPT_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sAddr, psAddr, sizeof(NET_ADDR));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the listen/accept connection.

   iType
   Type of connection:  STREAM.

   psAddr
   Pointer to NET_ADDR structure.

   Description
   Accepts network connections on a listen/accept socket and returns a normal socket
   on the new connection. The returned socket must be used to operate on the
   connection. If no incoming connection has been received on a blocking socket,
   AcceptSocket() will block until a connection has been received or a time-out
   occurs. On a non-blocking socket ERR_WOULD_BLOCK will be returned.

   Returns
   Success: returns socket handle on success.
   Error: -1 on failure with iNetErrNo containing the error code.
*/
#ifdef _CAPI_ACCEPTSO
int AcceptSocket(int iSocket, int iType, NET_ADDR *psAddr)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sAddr);

   psAddr->bProtocol = 6;
   DOSBUF_ENTER(sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sAddr,psAddr,sizeof(NET_ADDR),1);
   SET_SEGMENTC(r._s.ds,sAddr,psAddr);
   r.r.x.si = REALMODE_OFFSETC(sAddr,psAddr);
   r.r.x.bx = iSocket;
   r.r.x.dx = iType;
   r.r.h.ah = ACCEPT_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   if (psAddr)
   {
      DOSBUF_GET(sAddr, psAddr, sizeof(NET_ADDR));
   }
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iMaxid
   Number of sockets to test.

   plIflags
   Pointer to input flags indicating receive data availability.

   plOflags
   Pointer to output flags indicating readiness to write.

   Description
   Tests all Dos Compatible sockets for data availability and readiness
   to write.  A 32-bit DWORD representing 32 DC sockets is filled in for
   each socket with receive data, and another 32-bit DWORD for DC sockets
   ready for writing.  The least-significant bit represents the socket with
   value 0 and the most-significant bit represents the socket with value 31.
   Bits representing unused sockets are left unchanged.

   Returns
   On success, returns 0 with *plIflags and *plOflags filled in with current status.
   On error, -1 on failure with iNetErrNo containing the error code.
*/
#ifdef _CAPI_SELECTSO
int SelectSocket(int iMaxID, long *plIflags, long *plOflags)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sOflags);
   DPMIINFO_STRUCT (sIflags);

   // sIflags will contain data for plIflags
   // sOflags will contain data for plOflags

   DOSBUF_ENTER(2 * sizeof(long),-1);
   DOSBUF_SETUP(sOflags,plOflags,sizeof(long),1);
   DOSBUF_SETUP(sIflags,plIflags,sizeof(long),1);
   SET_SEGMENT(r._s.ds,sIflags,plIflags);
   r.r.x.dx = REALMODE_OFFSET(sIflags,plIflags);
   SET_SEGMENT(r._s.es,sOflags,plOflags);
   r.r.x.di = REALMODE_OFFSET(sOflags,plOflags);
   r.r.x.bx = iMaxID;
   r.r.h.ah = SELECT_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_GET(sOflags, plOflags, sizeof(long));
   DOSBUF_GET(sIflags, plIflags, sizeof(long));
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   pcBuf
   Pointer to buffer to receive data.

   uLen
   Length of buffer, ie maximum number of bytes to read.

   psFrom
   Pointer to NET_ADDR structure to receive address information about
   local and remote ports and remote IP address.

   wFlags
   Flags governing operation. Any combination of:
   NET_FLG_PEEK   Don't dequeue data.
   NET_FLG_NON_BLOCKING Don't block.

   Description
   Reads from the network using a socket. Returns as soon as any non-zero
   amount of data is available, regardless of the blocking state. If the
   operation is non-blocking, either by having used SetSocketOption() with
   the NET_OPT_NON_BLOCKING option or specifying wFlags with
   NET_FLG_NON_BLOCKING, ReadSocket() returns immediately with the count
   of available data or an error of ERR_WOULD_BLOCK.

   In the case of a STREAM (TCP) socket, record boundaries do not exist
   and any amount of data can be read at any time regardless of the way
   it was sent by the peer.  No data is truncated or lost even if more data
   than the buffer size is available.  What is not returned on one call, is
   returned on subsequent calls.  If a NULL buffer is specified, the number
   of bytes on the receive queue is returned.

   In the case of a DATAGRAM (UDP) socket, the entire datagram is returned
   in one call, unless the buffer is too small in which case the data is
   truncated, thereby preserving record boundaries.  Truncated data is lost.
   If data is available and both the NET_FLG_PEEK and NET_FLG_NON_BLOCKING
   flags are specified, the number of datagrams on the receive queue is
   returned.  If data is available and NET_FLG_PEEK is set and a NULL buffer
   is specified, the number of bytes in the next datagram is returned.

   Returns
   On success, returns number of bytes read.
   On error, -1 on failure with iNetErrNo containing the error code.
*/
#ifdef _CAPI_READSOCK
int ReadSocket(int iSocket, char *pcBuf, unsigned uLen,
            NET_ADDR *psFrom, unsigned uFlags)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sBuf);
   DPMIINFO_STRUCT (sFrom);

   DOSBUF_ENTER(uLen + sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sBuf,pcBuf,uLen,
               (uFlags & (NET_FLG_PEEK | NET_FLG_NON_BLOCKING)) ==
               (NET_FLG_PEEK | NET_FLG_NON_BLOCKING));
   DOSBUF_SETUP(sFrom,psFrom,sizeof(NET_ADDR),1);
   SET_SEGMENTC(r._s.ds,sBuf,pcBuf);
   r.r.x.si = REALMODE_OFFSETC(sBuf,pcBuf);
   SET_SEGMENTC(r._s.es,sFrom,psFrom);
   r.r.x.di = REALMODE_OFFSETC(sFrom,psFrom);
   r.r.x.dx = uFlags;
   r.r.x.cx = uLen;
   r.r.x.bx = iSocket;
   r.r.h.ah = READ_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   if (psFrom)
   {
      DOSBUF_GET(sFrom, psFrom, sizeof(NET_ADDR));
   }
   if (pcBuf)
   {
      DOSBUF_GET(sBuf, pcBuf, r.r.x.cx);
   }
   DOSBUF_EXIT();
   if (iReturn != -1)
      iReturn = r.r.x.cx;
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket socket for the connection.

   pcBuf
   Pointer to buffer to receive data.

   uLen
   Length of buffer, ie maximum number of bytes to read.

   psFrom
   Pointer to NET_ADDR structure to receive address information about local
   and remote ports and remote IP address.

   wFlags
   Flags governing operation. Any combination of:
   NET_FLG_PEEK   Don't dequeue data.
   NET_FLG_NON_BLOCKING Don't block.

   Description
   Reads from the network using a socket and is only intended to be used on
   DATAGRAM sockets. All datagrams from the IP address and port matching the
   values in the NET_ADDR structure are returned while others are discarded.
   A zero value for dwRemoteHost is used as a wildcard to receive from any
   host and a zero value for wRemotePort is used as a wildcard to receive
   from any port. The local port, wLocalPort , can not be specified as zero.

   In other respects ReadFromSocket() behaves the same as ReadSocket().

   Returns
   On success, returns number of bytes read.
   On error, -1 with iNetErrNo containing the error code.

   Note the following anomaly:
   If blocking is disabled, a failure with an error code of ErrWouldBlock
   is completely normal and only means that no data is currently available.
*/
#ifdef _CAPI_READFROM
int ReadFromSocket(int iSocket, char *pcBuf, unsigned uLen,
               NET_ADDR *psFrom, unsigned uFlags)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sBuf);
   DPMIINFO_STRUCT (sFrom);

   DOSBUF_ENTER(uLen + sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sBuf,pcBuf,uLen,0);
   DOSBUF_SETUP(sFrom,psFrom,sizeof(NET_ADDR),1);
   SET_SEGMENTC(r._s.ds,sBuf,pcBuf);
   r.r.x.si = REALMODE_OFFSETC(sBuf,pcBuf);
   SET_SEGMENTC(r._s.es,sFrom,psFrom);
   r.r.x.di = REALMODE_OFFSETC(sFrom,psFrom);
   r.r.x.dx = uFlags;
   r.r.x.cx = uLen;
   r.r.x.bx = iSocket;
   r.r.h.ah = READ_FROM_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   if (psFrom)
   {
      DOSBUF_GET(sFrom, psFrom, sizeof(NET_ADDR));
   }
   if (pcBuf)
   {
      DOSBUF_GET(sBuf, pcBuf, r.r.x.cx);
   }
   DOSBUF_EXIT();
   if (iReturn != -1)
      iReturn = r.r.x.cx;
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   pcBuf
   Pointer to buffer to containing send data.

   uLen
   Length of buffer, ie number of bytes to write.

   wFlags
   Flags governing operation; can be any combination of:
   NET_FLG_OOB Send out of band data (TCP only).
   NET_FLG_PUSH  Override NET_OPT_WAITFLUSH option if set by setting PSH flag in
                 TCP segment. This does NOT disregard Nagle heuristic (TCP only).
   NET_FLG_NON_BLOCKING Don't block.
   NET_FLG_BROADCAST Broadcast data (UDP only).
   NET_FLG_MC_NOECHO Suppress the local echo of a multicast datagram.

   Description
   Writes to the network using a socket.

   Returns
   On success, returns number of bytes written.
   On error, -1 on failure with iNetErrNo containing the error code.

   The number of bytes actually written on a non-blocking write, can be less
   than uLen.  In such a case, the writing of the unwritten bytes must be
   retried, preferably after some delay.
*/
#ifdef _CAPI_WRITESOC
int WriteSocket(int iSocket, char *pcBuf, unsigned uLen, unsigned uFlags)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sBuf);

   DOSBUF_ENTER(uLen,-1);
   DOSBUF_SETUP(sBuf,pcBuf,uLen,1);
   SET_SEGMENT(r._s.ds,sBuf,pcBuf);
   r.r.x.si = REALMODE_OFFSET(sBuf,pcBuf);
   r.r.x.bx = iSocket;
   r.r.x.cx = uLen;
   r.r.x.dx = uFlags;
   r.r.h.ah = WRITE_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   pcBuf
   Pointer to buffer to containing send data.

   uLen
   Length of buffer, ie number of bytes to write.

   psTo
   Pointer to NET_ADDR structure containing local port to write from and remote port and
   IP address to write to.

   wFlags
   Flags governing operation. Any combination of:
   NET_FLG_NON_BLOCKING Don't block.
   NET_FLG_BROADCAST Broadcast data (UDP only).

   Description
   Writes to the network using a network address (UDP only).

   Returns
   On success, returns number of bytes written.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_WRITETOS
int WriteToSocket(int iSocket, char *pcBuf, unsigned uLen,
              NET_ADDR *psTo, unsigned uFlags)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sBuf);
   DPMIINFO_STRUCT (sTo);

   DOSBUF_ENTER(uLen + sizeof(NET_ADDR),-1);
   DOSBUF_SETUP(sBuf,pcBuf,uLen,1);
   DOSBUF_SETUP(sTo,psTo,sizeof(NET_ADDR),1);
   SET_SEGMENT(r._s.ds,sBuf,pcBuf);
   r.r.x.si = REALMODE_OFFSET(sBuf,pcBuf);
   SET_SEGMENTC(r._s.es,sTo,psTo);
   r.r.x.di = REALMODE_OFFSETC(sTo,psTo);
   r.r.x.bx = iSocket;
   r.r.x.cx = uLen;
   r.r.x.dx = uFlags;
   r.r.h.ah = WRITE_TO_SOCKET;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif
#ifdef _CAPI_SETALARM
#ifdef DPMI_16BIT_STACK
// Internal function to maintain a chain of Real Mode Callback buffers
// required for Async Notifications
INFO_STRUCT_CHAIN *_AllocRmCbRetf(void (CAPI_CB *lpHandler)())
{
   INFO_STRUCT_CHAIN *psInfoChain,*psInfoChainPrev = psInfoChainHead;

   for (psInfoChain = psInfoChainHead;psInfoChain;psInfoChain = psInfoChain->psNext)
   {
      if (psInfoChain->sInfo.pm_offset == (int)lpHandler)
         break;
      psInfoChainPrev = psInfoChain;
   }
   if (!psInfoChain)
   {
      if ((psInfoChain = calloc(1,sizeof(INFO_STRUCT_CHAIN))) == 0)
      {
         iNetErrNo = ERR_NO_MEM;
         return 0;
      }
      psInfoChain->sInfo.pm_offset = (int)lpHandler;
      if (DPMI_ALLOC_RMCB(&psInfoChain->sInfo, &cb_regs))
      {
         free(psInfoChain);
         iNetErrNo = ERR_ILLEGAL_OP;
         return 0;
      }
      if (psInfoChainHead)
      {
         psInfoChainPrev->psNext = psInfoChain;
      }
      else
         psInfoChainHead = psInfoChain;
      //printf("RMCB %04x:%04x\n",psInfoChain->sInfo.rm_segment,psInfoChain->sInfo.rm_offset);
   }
   return psInfoChain;
}

// Function to free RealMode Callback chain
// Should be called via RMCB_FREE() macro before exit
void FreeRealModeCallbacks(void)
{
   INFO_STRUCT_CHAIN *psInfoChain,*psInfoChainNext;

   for (psInfoChain = psInfoChainHead;psInfoChain;psInfoChain = psInfoChainNext)
   {
      psInfoChainNext = psInfoChain->psNext;
      DPMI_FREE_RMCB(&psInfoChain->sInfo);
      free(psInfoChain);
   }
   psInfoChainHead = 0;
}
#endif

/*
   Parameters
   iSocket
   Socket handle for the connection.

   dwTime
   Timer delay in milliseconds.

   lpHandler
   Far address of alarm callback.  See the description of  SetAsyncNotification() for the
   format of the callback function.

   dwHint
   Argument to be passed to callback function.

   Description
   The SetAlarm() function sets an alarm timer.

   Returns
   On success, returns socket handle.
   On error, -1 with iNetErrNo containing the error code.
*/
int SetAlarm(int iSocket, DWORD dwTime,
          void (CAPI_CB *lpHandler)(), DWORD dwHint)
{
   X86Regs r;

   if (lpHandler)
   {
      #ifdef DPMI_16BIT_STACK
      INFO_STRUCT_CHAIN *psInfoChain;

      if ((psInfoChain = _AllocRmCbRetf(lpHandler)) == 0)
         return -1;
      #endif
      SET_SEGMENT(r._s.ds,psInfoChain->sInfo,lpHandler);
      r.r.x.si = REALMODE_OFFSET(psInfoChain->sInfo,lpHandler);
   }
   else
   {
      SET_SEGMENTZ(r._s.ds);
      r.r.x.si = 0;
   }
   SET_32BITREG(r.r.x.cx,r.r.x.dx,dwTime);
   SET_32BITREG(r._s.es,r.r.x.di,dwHint);
   r.r.x.bx = iSocket;
   r.r.h.ah = SET_ALARM;
   return CallSocketsDosApi(&r);
}
#endif
/*
   Parameters
   iSocket
   Socket handle for the connection.

   iEvent
   Event which is being set:
   NET_AS_OPEN Connection has opened.
   NET_AS_RCV  Data has been received.
   NET_AS_XMT  Ready to transmit.
   NET_AS_FCLOSE  Peer has closed connection.
   NET_AS_CLOSE   Connection has been closed.
   NET_AS_ERROR   Connection has been reset.

   lpHandler
   Far address of callback function.

   dwHint
   Argument to be passed to callback function
   The handler is not compatible with C calling conventions but is called
   by a far call with the following parameters:
   BX = Socket handle.
   CX = Event.
   ES:DI = dwHint argument passed to SetAsyncNotification() or SetAlarm().
   DS:DX = SI:DX = variable argument depending on event:
   NET_AS_OPEN
   NET_AS_CLOSE   Pointer to NET_ADDR address structure.
   NET_AS_FCLOSE
   NET_AS_RCV
   NET_AS_ALARM   Zero.
   NET_AS_XMT  Byte count which can be sent without blocking.
   NET_AS_ERROR   Error code -ERR_TERMINATING, ERR_TIME_OUT or ERR_RESET.

   Description
   Sets an asynchronous notification (callback) for a specific event.

   Other CAPI functions may be called in the callback, with the exception of
   ResolveName() which may call DOS.  The callback is not compatible with C
   argument-passing conventions and some care must be taken.  Some CPU
   register manipulation is required.  This can be done by referencing CPU
   registers, such as _BX, or by means of assembler instructions.

   In the callback, the stack is supplied by Sockets and may be quite small
   depending on the /s= command line option when loading Sockets.  The stack
   segment is obviously not equal to the data segment, which can cause
   problems when the Tiny, Small or Medium memory model is used.
   The simplest way to overcome the problem is to use the Compact, Large
   or Huge memory model.  Other options - use the DS != SS compiler option
   or do a stack switch to a data segment stack .

   If the callback is written in C or C++, the _loadds modifier can be used
   to set the data segment to that of the module, which destroys the DS used
   for the variable argument.  (This is why DS == SI on entry for Sockets
   version 1.04 and later.)  An alternate method is to use the argument passed
   to SetAsyncNotification() in ES:DI as a pointer to a structure that is
   accessible from both the main code and the callback.  If DS is not set
   to the data segment of the module, then the functions in CAPI.C do not
   work: Don't use them in the callback.

   The callback will probably be performed at interrupt time with no
   guarantee of reentry to DOS.  Do not use any function, such as putchar()
   or printf(), in the callback which may cause DOS to be called.

   It is good programming practice to do as little as possible in the
   callback.  The setting of event flags which trigger an operation at a
   more stable time is recommended.

   Callback functions do not nest.  The callback function is not called
   while a callback is still in progress, even if other CAPI functions
   are called.

   To alleviate the problems in items 2, 3 and 4 above, a handler is
   provided in CAPI.C which uses the dwHint parameter to pass the address
   of a C-compatible handler, with a stack which is also C-compatible.
   This handler is named AsyncNotificationHandler.  A user handler named
   MyHandler below, is called in the normal way with a stack of 500 bytes
   long.  The stack size value can be set by changing the HANDLER_STACK_SIZE
   constant in CAPI.C.

   Returns
   On success, returns pointer to the previous callback handler.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_SETASYNC

int D_FAR *SetAsyncNotification(int iSocket, int iEvent,
               void (CAPI_CB *lpHandler)(), DWORD dwHint)
{

   X86Regs r;
   int iReturn;
   long lpPrevHandler = -1;

   if (lpHandler)
   {
      #ifdef DPMI_16BIT_STACK
      INFO_STRUCT_CHAIN *_AllocRmCbRetf(void (CAPI_CB *lpHandler)());
      INFO_STRUCT_CHAIN *psInfoChain;

      if ((psInfoChain = _AllocRmCbRetf(lpHandler)) == 0)
         return (int D_FAR *)lpPrevHandler;
      #endif
      SET_SEGMENT(r._s.ds,psInfoChain->sInfo,lpHandler);
      r.r.x.si = r.r.x.dx = REALMODE_OFFSET(psInfoChain->sInfo,lpHandler);
   }
   else
   {
      SET_SEGMENTZ(r._s.ds);
      r.r.x.si = 0;
   }
   SET_32BITREG(r._s.es,r.r.x.di,dwHint);
   r.r.x.bx = iSocket;
   r.r.x.cx = iEvent;
   r.r.h.ah = SET_ASYNC_NOTIFICATION;
   iReturn = CallSocketsDosApi(&r);
   if(iReturn != -1)
      lpPrevHandler = (long)MAKE_REAL_POINTER(r._s.ds, r.r.x.dx);
   return (int D_FAR *)lpPrevHandler;
}
#endif


/*
   Parameters
   pszName
   Pointer to string containing symbolic name.

   pcCname
   Pointer to buffer to receive canonical name.

   ICnameLen
   Length of buffer pointed to by pcName.

   Description
   Resolves IP address from symbolic name.

   Returns
   On success, returns IP address.
   On failure, 0 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_RESOLVEN
DWORD ResolveName(char *pszName, char *pszCName, int iCNameLen)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sCName);
   DPMIINFO_STRUCT (sName);
   DPMI_INT (iLen);

   DOSBUF_ENTER((iLen = strlen(pszName) + 1) + iCNameLen,-1);
   DOSBUF_SETUP(sName,pszName,iLen,1);
   DOSBUF_SETUP(sCName,pszCName,iCNameLen,1);
   r.r.x.dx = REALMODE_OFFSET(sName,pszName);
   SET_SEGMENTC(r._s.ds,sName,pszName);
   r.r.x.di = REALMODE_OFFSET(sCName,pszCName);
   SET_SEGMENTC(r._s.es,sCName,pszCName);
   r.r.x.cx = iCNameLen;
   r.r.h.ah = RESOLVE_NAME;
   iReturn = CallSocketsDosApi(&r);
   // retrieve the string from the transfer buffer
   DOSBUF_GET(sCName, pszCName, iCNameLen);
   // free the transfer buffers
   DOSBUF_EXIT();
   // return address
   if (iReturn == -1)
      return 0L;
   return ((DWORD)r.r.x.dx << 16) + r.r.x.ax;
}
#endif

/*
   Parameters
   pszName
   Pointer to string containing dotted decimal address.

   Description
   Gets IP address from dotted decimal address.

   Returns
   On success, returns IP address.
   On error, 0 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_PARSEADD
DWORD ParseAddress(char *pszName)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sName);
   DPMI_INT (iLen);

   DOSBUF_ENTER(iLen = strlen(pszName) + 1,-1L);
   DOSBUF_SETUP(sName,pszName,iLen,1);
   r.r.x.dx = REALMODE_OFFSET(sName,pszName);
   SET_SEGMENT(r._s.ds,sName,pszName);
   r.r.h.ah = PARSE_ADDRESS;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   if (iReturn == -1)
      return 0L;
   return (((DWORD) r.r.x.dx << 16) + r.r.x.ax);
}
#endif

/*
   Parameters
   iSocket
   Socket handle for the connection.

   iLevel
   Level of option. This value is ignored.

   iOption
   Option to set.
   NET_OPT_NON_BLOCKING Set blocking off if dwOptionValue is non-zero.
   NET_OPT_TIMEOUT   Set the timeout to dwOptionValue milliseconds.
         Turn off timeout if dwOptionValue is zero.
   NET_OPT_WAIT_FLUSH Wait for WriteSocket() with NET_FLG_PUSH if dwOptionValue is non-zero.

   dwOptionValue
   Option value.

   iLen
   Length of dwOptionValue, 4 in all cases.

   Description
   Sets an option on the socket.

   Returns
   On success, returns global socket.
   On error, -1 with iNetErrNo containing the error code.
*/
#ifdef _CAPI_SETSOCKE
int SetSocketOption(int iSocket, int iLevel, int iOption,
               DWORD dwOptionValue, int iLen)
{
   X86Regs r;

   SET_32BITREG(r._s.ds,r.r.x.dx,dwOptionValue);
   r.r.x.bx = iSocket;
   r.r.x.cx = iLen;
   r.r.x.si = iLevel;
   r.r.x.di = iOption;
   r.r.h.ah = SET_OPTION;
   return CallSocketsDosApi(&r);
}
#endif

/*
   Parameters
   dwGroupAddress
   The group address on which to receive multicast datagrams.

   dwIPAddress
   The IP adress for the interface to use.  The first interface to be
   specified in SOCKET.CFG is the default interface in the case where
   dwIPAddress == 0.

   Description
   Causes Sockets to join a multicast group.

   Returns
   On success, 0.
   On error, any other integer value contains the error code.
*/
#ifdef _CAPI_JOINGROU
int JoinGroup(DWORD dwGroupAddress, DWORD dwInterfaceAddress)
{
   X86Regs r;
   GROUP_ADDR sGroupAddress;
   GROUP_ADDR *psGroupAd = &sGroupAddress ;
   int iReturn;
   DPMIINFO_STRUCT (sGroupAd);

   sGroupAddress.dwGroupAddr = dwGroupAddress;
   sGroupAddress.dwIFAddr = dwInterfaceAddress;
   DOSBUF_ENTER(sizeof(GROUP_ADDR),-1);
   DOSBUF_SETUP(sGroupAd,psGroupAd,sizeof(GROUP_ADDR),1);
   r.r.x.si = REALMODE_OFFSET(sGroupAd,psGroupAd);
   SET_SEGMENTC(r._s.ds,sGroupAd,psGroupAd);
   r.r.h.ah = JOIN_GROUP;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif

/*
   Parameters
   dwGroupAddress
   The group address on which multicast datagrams are being received.

   dwIPAddress
   The IP adress for the interface being used.  The first interface to be
   specified in SOCKET.CFG is the default interface in the case where
   dwIPAddress == 0.

   Description
   The LeaveGroup() function causes Sockets to leave a multicast group.

   Returns
   On success, returns 0.
   On error, any other integer value contains the error code.
*/
#ifdef _CAPI_LEAVEGRO
int LeaveGroup(DWORD dwGroupAddress, DWORD dwInterfaceAddress)
{
   X86Regs r;
   GROUP_ADDR sGroupAddress;
   GROUP_ADDR *psGroupAd = &sGroupAddress ;
   int iReturn;
   DPMIINFO_STRUCT (sGroupAd);

   sGroupAddress.dwGroupAddr = dwGroupAddress;
   sGroupAddress.dwIFAddr = dwInterfaceAddress;
   DOSBUF_ENTER(sizeof(GROUP_ADDR),-1);
   DOSBUF_SETUP(sGroupAd,psGroupAd,sizeof(GROUP_ADDR),1);
   r.r.x.si = REALMODE_OFFSET(sGroupAd,psGroupAd);
   SET_SEGMENTC(r._s.ds,sGroupAd,psGroupAd);
   r.r.h.ah = LEAVE_GROUP;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif

/*
   Parameters
   psGroupAddress
   Pointer to the group address on which to receive multicast datagrams.

   dwInterfaceId
   The IP adress for the interface to use.  The first interface to be
   specified in SOCKET.CFG is the default interface in the case where
   dwInterfaceId == 0.
   wAddrLen
   The IP address length, 4 for IPv4, 16 for IPv6

   Description
   Causes Sockets to join a multicast group for IPv4 or IPv6

   Returns
   On success, 0.
   On error, any other integer value contains the error code.
*/
#ifdef _CAPI_JOINGREX
int JoinGroupEx(IPAD *psGroupAddress, DWORD dwInterfaceId, unsigned uAddrLen)
{
   X86Regs r;
   GROUP_ADDR sGroupAddress;
   GROUP_ADDR *psGroupAd = &sGroupAddress ;
   int iReturn;
   DPMIINFO_STRUCT (sGroupAd);

   sGroupAddress.dwGroupAddr = uAddrLen;
   sGroupAddress.dwIFAddr = dwInterfaceId;
   sGroupAddress.sIpAddr = *psGroupAddress;
   DOSBUF_ENTER(sizeof(GROUP_ADDR),-1);
   DOSBUF_SETUP(sGroupAd,psGroupAd,sizeof(GROUP_ADDR),1);
   r.r.x.si = REALMODE_OFFSET(sGroupAd,psGroupAd);
   SET_SEGMENTC(r._s.ds,sGroupAd,psGroupAd);
   r.r.h.ah = JOIN_GROUP;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif

/*
   Parameters
   psGroupAddress
   Pointer to the group address on which multicast datagrams are being received.

   dwInterfaceId
   The IP adress for the interface to use.  The first interface to be
   specified in SOCKET.CFG is the default interface in the case where
   dwInterfaceId == 0.
   wAddrLen
   The IP address length, 4 for IPv4, 16 for IPv6

   Description
   Causes Sockets to leave a multicast group for IPv4 or IPv6

   Returns
   On success, 0.
   On error, any other integer value contains the error code.
*/
#ifdef _CAPI_LEAVEGRE
int LeaveGroupEx(IPAD *psGroupAddress, DWORD dwInterfaceId, unsigned uAddrLen)
{
   X86Regs r;
   GROUP_ADDR sGroupAddress;
   GROUP_ADDR *psGroupAd = &sGroupAddress ;
   int iReturn;
   DPMIINFO_STRUCT (sGroupAd);

   sGroupAddress.dwGroupAddr = uAddrLen;
   sGroupAddress.dwIFAddr = dwInterfaceId;
   sGroupAddress.sIpAddr = *psGroupAddress;
   DOSBUF_ENTER(sizeof(GROUP_ADDR),-1);
   DOSBUF_SETUP(sGroupAd,psGroupAd,sizeof(GROUP_ADDR),1);
   r.r.x.si = REALMODE_OFFSET(sGroupAd,psGroupAd);
   SET_SEGMENTC(r._s.ds,sGroupAd,psGroupAd);
   r.r.h.ah = LEAVE_GROUP;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif

/*
   Parameters
   pszName
   Pointer to interface name.

   WFunction
   Function to perform:
   IOCTL_CONNECT  Start dial operation
   IOCTL_DISCONNECT  Disconnect modem
   IOCTL_ENABLEPORT  Enable communications port
   IOCTL_DISABLEPORT Disable communications port
   IOCTL_ENABLEDOD   Enable dial-on-demand
   IOCTL_DISABLEDOD  Disable dial-on-demand
   IOCTL_GETSTATUS   Get modem/connection status

   Description
   The IfaceIOCTL function controls asynchronous interfaces.

   Returns
   On success, returns >=0.
   On error, returns -1.

   IOCTL_GETSTATUS returns the following bits:
   ST_DTR   0x01  Data Terminal Ready
   ST_RTS   0x02  Request To Send
   ST_CTS   0x10  Clear To Send
   ST_DSR   0x20  Data Set Ready
   ST_RI 0x40  Ring Indicator
   ST_DCD   0x80  Data Carrier Detect
   ST_CONNECTED   0x100 Modem is connected
   ST_MODEMSTATE  0xe00 Modem state mask
   STM_NONE 0x000 No modem on port
   STM_IDLE 0x200 Modem is idle
   STM_INITIALIZING  0x400 Modem is initializing
   STM_DIALING 0x600 Modem is dialing
   STM_CONNECTING 0x800 Modem is connecting
   STM_ANSWERING  0xa00 Modem is answering
*/
#ifdef _CAPI_IFACEIOC
int IfaceIOCTL(char *pszName, unsigned uFunction)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sName);
   DPMI_INT (iLen);

   DOSBUF_ENTER(iLen = strlen(pszName) + 1,-1);
   DOSBUF_SETUP(sName,pszName,iLen,1);
   r.r.x.si = REALMODE_OFFSET(sName,pszName);
   SET_SEGMENT(r._s.ds,sName,pszName);
   r.r.h.ah = IFACE_IOCTL;
   r.r.h.al = uFunction;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}
#endif

#ifdef _CAPI_ARPAPI
int ArpApi(unsigned uFunction,unsigned uHardware, DWORD dwIpAddress, void D_FAR **ppArpTab)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sMacName);

   r.r.h.ah = ARP_API;
   r.r.h.al = uFunction;
   r.r.x.bx = uHardware;
   SET_32BITREG(r.r.x.cx,r.r.x.dx,dwIpAddress);
   if (uFunction == ARP_API_ADD) {
      DOSBUF_ENTER(6,-1);
      DOSBUF_SETUP(sMacName,(void *)*ppArpTab,6,1);
      r.r.x.si = REALMODE_OFFSET(sMacName, (void *)*ppArpTab);
      SET_SEGMENT(r._s.es,sMacName, (void *)*ppArpTab);
      iReturn = CallSocketsDosApi(&r);
      DOSBUF_EXIT();
   }
   else {
      iReturn = CallSocketsDosApi(&r);
      if (uFunction == ARP_API_GETTABLE && ppArpTab)
         *ppArpTab = (void D_FAR *)MAKE_REAL_POINTER(r._s.es,r.r.x.si);
   }
   return iReturn;
}
#endif

/*
Invoke Proprietary API from CAPI.
This is a limited implementation intended only for use where no buffers
are passed. It is specifically intended to be used to close/reset/delete
a TCP connection based on the TCB.
*/

#ifdef _CAPI_PROAPI
int ProprietaryApi(unsigned uFunction,unsigned uTcb)
{
   X86Regs r;

   r.r.h.ah = PROPRIETARY_API;
   r.r.h.al = uFunction;
   r.r.x.bx = uTcb;
   return CallSocketsDosApi(&r);
}
#endif

#ifdef _CAPI_ROUTE
static int RouteCommon(ROUTE_ENTRY *psRouteEntry,BYTE bFunction)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sRouteEntry);

   DOSBUF_ENTER(sizeof(ROUTE_ENTRY),-1);
   DOSBUF_SETUP(sRouteEntry,psRouteEntry,sizeof(ROUTE_ENTRY),1);
   SET_SEGMENT(r._s.ds,sRouteEntry,psRouteEntry);
   r.r.x.si = REALMODE_OFFSET(sRouteEntry,psRouteEntry);
   r.r.h.ah = bFunction;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}

int RouteAdd(ROUTE_ENTRY *psRouteEntry)
{
   return RouteCommon(psRouteEntry,ROUTE_ADD);
}

int RouteChange(ROUTE_ENTRY *psRouteEntry)
{
   return RouteCommon(psRouteEntry,ROUTE_CHANGE);
}

int RouteDrop(ROUTE_ENTRY *psRouteEntry)
{
   return RouteCommon(psRouteEntry,ROUTE_DROP);
}
#endif

#ifdef _CAPI_CONFIGCOMMAND
int ConfigCommand(char *pszCommand,int iShowError)
{
   X86Regs r;
   int iReturn;
   DPMIINFO_STRUCT (sCommand);
   DPMI_INT (iLen);

   DOSBUF_ENTER(iLen = strlen(pszCommand) + 1,-1L);
   DOSBUF_SETUP(sCommand,pszCommand,iLen,1);
   SET_SEGMENT(r._s.ds,sCommand,pszCommand);
   r.r.x.bx = iShowError;
   r.r.x.si = REALMODE_OFFSET(sCommand,pszCommand);
   r.r.h.ah = CONFIG_COMMAND;
   iReturn = CallSocketsDosApi(&r);
   DOSBUF_EXIT();
   return iReturn;
}

#endif

/*
 * Local Variables:
 * tab-width:3
 * End:
 */

/*  EOF: capi.c */

