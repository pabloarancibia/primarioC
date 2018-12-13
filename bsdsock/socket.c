//  $Id: SOCKET.C 1.11 2003/07/26 03:37:14Z johnb Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 2002-2005 Datalight, Inc.
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
/* SOCKET.C
BSD Socket interface functions

IPv6 extensions according to RFC2553

2010-07-01 gvn on normal close of connection give error 0
2008-09-13 gvn fix TCP_NODELAY
2007-11-09 gvn fix getaddrinfo()
2007-03-26 gvn getsockname() & getpeername() address length fixes
2007-02-24 gvn fix recv() to return WASEWOULDBLOCK when connection is not established yet
2007-02-03 gvn implement keyboard select() for socket (file descriptor) 0, fix select()
2005-10-15 gvn implement UDP broadcast
2005-07-06 gvn make room for future expansion of TCB structure
2005-04-09 gvn fix getaddrinfo() to return proper error code when service is unsupported
2005-04-01 gvn implement TCP_NODELAY
2005-01-17 gvn getaddrinfo() resolves sin6_scope_id
2005-01-01 gvn Add sin6_scope_id support
2004-12-06 gvn Change conditional compile defines for easier maintenance
2004-08-26 gvn Change for IPV6
2004-07-27 gvn change for IPAD
2003-07-24 gvn add WSA* functions for Winsock compatibility
2003-07-09 gvn fix udp ioctlsocket() when connect() has not been called
2003-07-08 gvn fix udp recvfrom() when connect() has not been called
2003-01-07 gvn library compile with conditionals
2002-09-30 gvn Adapted from Winsock code

To do:
   OOB data (if ever!)
*/

#ifndef IPV6
#define IPV6
#endif

#ifdef _MSC_VER
#include <malloc.h>
#else
#include <mem.h>
#endif
//#include <alloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#define _POSIX_SOURCE
#include <time.h>
#undef  _POSIX_SOURCE
#include <dos.h>
#include <errno.h>
#include "compiler.h"
#include "capi.h"
#include "api.h"
#include "socket.h"
#include "clock.h"

#ifndef _SAPI_LIBRARY
#define _SAPI_ACCEPT
#define _SAPI_BIND
#define _SAPI_CLOSESOC
#define _SAPI_CONNECT
#define _SAPI_IOCTLSOC
#define _SAPI_GETPEERN
#define _SAPI_GETSOCKN
#define _SAPI_GETSOCKO
#define _SAPI_HTONL
#define _SAPI_HTONS
#define _SAPI_INETADDR
#define _SAPI_INETNTOA
#define _SAPI_INETNTOP
#define _SAPI_GETADDRI
#define _SAPI_GAISTRER
#define _SAPI_LISTEN
#define _SAPI_NTOHL
#define _SAPI_NTOHS
#define _SAPI_RECV
#define _SAPI_RECVFROM
#define _SAPI_SELECT
#define _SAPI_SEND
#define _SAPI_SENDTO
#define _SAPI_SETSOCKO
#define _SAPI_SHUTDOWN
#define _SAPI_SOCKET
#define _SAPI_GHOSTBYA
#define _SAPI_GHOSTBYN
#define _SAPI_GHOSTNAM
#define _SAPI_GSERVBYP
#define _SAPI_GSERVBYN
#define _SAPI_GPROTONU
#define _SAPI_GPROTONA
#define _SAPI_WSASTART
#define _SAPI_WSASETER
#define _SAPI_WSAGETER
#define _SAPI_ISSET
#define _SAPI_FDCLR
#define _SAPI_FDSET

#define _SAPI_MAPERROR
#define _SAPI_GETREADC
#define _SAPI_GETNEXTP
#define _SAPI_CHECKUDP
#define _SAPI_CNVRT128
#define _SAPI_DATABASE
#define _SAPI_BASE
#endif   // _SAPI_LIBRARY

//===========================================================================
// Global variables
//===========================================================================

typedef struct sScb {   // socket control block
   BYTE bType;          // type of socket
   ISOCKET sIsockLocal; // local address passed by bind()
   ISOCKET sIsockRemote;// remote address passed by connect()
   WORD wFlags;         // various bits
#define SF_USED      1
#define SF_BOUND     2
#define SF_NONBLOCK  4  // non-blocking socket
#define SF_LISTEN    8
#define SF_CONNECTED 16 // Datagram socket is connected
#define SF_INET6     32 // IPv6
   WORD wOption;        // option flags
   WORD wTcpOption;     // tcp option flags
   WORD linger_time;    // linger time
   WORD wRcvBufSize;    // receive buffer size
   WORD wSndBufSize;    // send buffer size
} SCB;

#define S_OFFSET 64     // first socket id

extern SCB *_psScbs[FD_SETSIZE];
extern char _szHostname[80];     // our hostname
extern struct  servent  _sSe;
extern struct  protoent _sPe;
extern BYTE _bIpVersionFlags;

//===========================================================================
// Prototypes
//===========================================================================
int _MapError(int iErrNum);// map CAPI to winsock errors
u_short _GetNextPortNum(void);
int _GetReadCount(SOCKET s,SCB *psScb);
int _GetReadCount(SOCKET s,SCB *psScb);
int _CheckUdpOpen(SOCKET s,SCB *psScb);
FILE *_OpenServiceFile(char *pszName);    // open services/protocol file
int _GetServiceEntry(FILE *psFile,char **ppszName,char ***pppszAlias,
            short *psiNum,char **ppszProto);
void Convert128(void *psIpSource,void *psIpDest);

#ifdef _SAPI_BASE
SCB *_psScbs[FD_SETSIZE] = {0};
char _szHostname[80] = "";    // our hostname

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

#ifdef SOCK_DEBUG
char _cTracing = 0x00;
static FILE *psFile = 0;

int StartDebug(char *pszFileName)
{
   if (psFile)
      fclose(psFile);
   if ((psFile = fopen(pszFileName,"wt")) == 0)
      return -1;
   _cTracing = 1;
   return 1;
}

int StopDebug(void)
{
   FILE *psF = psFile;

   psFile = 0;
   _cTracing = 0;
   return fclose(psF);
}


void SockDebug(char *pszName,...)
{
   va_list ap;
   struct time sTime;

   if (_cTracing) {
      gettime(&sTime);
      fprintf(psFile,"%02d:%02d:%02d.%02d ",
               sTime.ti_hour,sTime.ti_min,sTime.ti_sec,sTime.ti_hund);
      va_start(ap, pszName);
      vfprintf(psFile,pszName,ap);
      va_end(ap);
      fprintf(psFile,"\n");
   }
}
#endif   // SOCK_DEBUG


char *_pszTest;
SCB *_pscpTest;

void use_globlals_in_base(void)
{
   _pszTest = _szHostname;
   _pscpTest = _psScbs[0];
}


#endif   // _SAPI_BASE

//===========================================================================
#ifdef _SAPI_ACCEPT
SOCKET accept(SOCKET s, struct sockaddr *addr,int *addrlen)
{
   SCB *psScb,*psScbNew;
   SOCKET sn = INVALID_SOCKET;
   NET_ADDR sNetAddr;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_accept;
   }
   if (addr && addrlen && *addrlen < sizeof(SOCKADDR)) {
      errno = WSAEFAULT;
      goto Exit_accept;
   }
   if (!(psScb->wFlags & SF_LISTEN)) {
      errno = WSAEINVAL;
      goto Exit_accept;
   }
   if ((psScbNew = calloc(1,sizeof(SCB))) == 0) {
      ReleaseSocket(sn);
      errno = WSAENOBUFS;
      goto Exit_accept;
   }
   if ((int)(sn = AcceptSocket(s,STREAM | TYPE_EXT,&sNetAddr)) < 0) {
      free(psScbNew);
      errno = _MapError(iNetErrNo);
      goto Exit_accept;
   }
   if (_psScbs[sn - S_OFFSET]) {
      free(psScbNew);
      ReleaseSocket(sn);
      sn = INVALID_SOCKET;
      errno = WSAENETDOWN;
      goto Exit_accept;
   }
   _psScbs[sn - S_OFFSET] = psScbNew;
   *psScbNew = *psScb;
   psScbNew->wFlags &= ~SF_LISTEN;
   if (sNetAddr.dwRemoteHost == 4)
      psScbNew->wFlags &= ~SF_INET6;
   else
      psScbNew->wFlags |= SF_INET6;
   if (addr && addrlen) {
      if (sNetAddr.dwRemoteHost == 4) {
         ((SOCKADDR_IN *)addr)->sin_addr.s_addr = htonl(sNetAddr.sIpAddr.dwAddr);
         ((SOCKADDR_IN *)addr)->sin_port = htons(sNetAddr.wRemotePort);
         ((SOCKADDR_IN *)addr)->sin_family = AF_INET;
         *addrlen = sizeof(SOCKADDR_IN);
      }
#ifdef IPV6
      else {
         Convert128(sNetAddr.sIpAddr.abAddr,&((SOCKADDR_IN6 *)addr)->sin6_addr);
         ((SOCKADDR_IN6 *)addr)->sin6_port = htons(sNetAddr.wRemotePort);
         ((SOCKADDR_IN6 *)addr)->sin6_family = AF_INET6;
         *addrlen = sizeof(SOCKADDR_IN6);
      }
#endif
   }
   errno = 0;

Exit_accept:
#ifdef SOCK_DEBUG
   if (_cTracing) {
      SockDebug("accept: %u, return %d", s, sn);
   }
#endif
   return sn;
}
#endif   // _SAPI_ACCEPT

//===========================================================================
#ifdef _SAPI_BIND
int bind(SOCKET s, const struct sockaddr *addr, int namelen)
{
   SCB *psScb,*psScb1;
   int   iRet = SOCKET_ERROR,i,iNameLen;
   DWORD dwIpAddr;
   u_short port;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_bind;
   }
   if (psScb->wFlags & SF_BOUND) {
      errno = WSAEINVAL;
      goto Exit_bind;
   }
   if (((SOCKADDR_IN *)addr)->sin_family == AF_INET) {
      iNameLen = sizeof(SOCKADDR_IN);
      dwIpAddr = ntohl(((SOCKADDR_IN *)addr)->sin_addr.s_addr);
      /*
      if (dwIpAddr && IsThisAddress(dwIpAddr) == 0) {
         errno =  WSAEINVAL;
         goto Exit_bind;
      }
      */
      psScb->sIsockLocal.sIpAddress.dwAddr = dwIpAddr;
   }
#ifdef IPV6
   else if (((SOCKADDR_IN *)addr)->sin_family == AF_INET6) {
      iNameLen = sizeof(SOCKADDR_IN6);
      Convert128(&((SOCKADDR_IN6 *)addr)->sin6_addr,psScb->sIsockLocal.sIpAddress.abAddr);
   }
#endif
   else {
      errno =  WSAEAFNOSUPPORT;
      goto Exit_bind;
   }
   if (namelen < iNameLen) {
      errno = WSAEFAULT;
      goto Exit_bind;
   }

   port = ntohs(((SOCKADDR_IN *)addr)->sin_port);
   if (port == 0)
         port = _GetNextPortNum();
   if (!(psScb->wOption & SO_REUSEADDR) && port) {
      for (i = 0;i < FD_SETSIZE;++i) {
         if ((psScb1 = _psScbs[i]) != 0 &&
               (psScb1->wFlags & SF_BOUND) && psScb->bType == psScb1->bType &&
               psScb1->sIsockLocal.wPort == port) {
            errno = WSAEADDRINUSE;
            goto Exit_bind;
         }
      }
   }
   psScb->sIsockLocal.wPort = port;
   psScb->wFlags |= SF_BOUND;
   iRet = 0;

Exit_bind:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("bind: %u, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_BIND

//===========================================================================
#ifdef _SAPI_CLOSESOC
int closesocket(SOCKET s)
{
   SCB *psScb;
   int   iRet = SOCKET_ERROR;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_closesocket;
   }
   if (psScb->bType == SOCK_STREAM && (psScb->wOption & SO_LINGER)) {
      if (psScb->linger_time) {
         if (psScb->wFlags & SF_NONBLOCK) {
            errno = WSAEWOULDBLOCK;
            goto Exit_closesocket;
         }
         if (SetSocketOption(s,0,NET_OPT_TIMEOUT,psScb->linger_time * 1000,
                                                            sizeof(DWORD)) < 0)
            goto closesocket_error;
         if (EofSocket(s) < 0)
            goto abort_close;
      }
      else {
         abort_close:
         if (AbortSocket(s) < 0)
            goto closesocket_error;
         goto abort_exit;
      }
   }
   if (ReleaseSocket(s) < 0) {
      closesocket_error:
      errno = _MapError(iNetErrNo);
      goto Exit_closesocket;
   }
   abort_exit:
   free(psScb);
   _psScbs[s - S_OFFSET] = 0;
   iRet = 0;

Exit_closesocket:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("closesocket: %u, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_CLOSESOC

//===========================================================================
#ifdef _SAPI_CONNECT
int connect(SOCKET s, const struct sockaddr *name, int namelen)
{
   SCB *psScb;
   SOCKADDR_IN *psTo = (SOCKADDR_IN *)name;
#ifdef IPV6
   SOCKADDR_IN6 *psTo6 = (SOCKADDR_IN6 *)name;
#endif
   int i,iRet = SOCKET_ERROR,iNameLen,iType;
   NET_ADDR sNetAddr;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_connect;
   }
   if (((SOCKADDR_IN *)name)->sin_family == AF_INET) {
      psScb->sIsockRemote.sIpAddress.dwAddr = sNetAddr.sIpAddr.dwAddr =
                                                   ntohl(psTo->sin_addr.s_addr);
      sNetAddr.dwRemoteHost = 4;
      iNameLen = sizeof(SOCKADDR_IN);
   }
#ifdef IPV6
   else if (((SOCKADDR_IN *)name)->sin_family == AF_INET6) {
      Convert128(&psTo6->sin6_addr,sNetAddr.sIpAddr.abAddr);
      memcpy(psScb->sIsockRemote.sIpAddress.abAddr,sNetAddr.sIpAddr.abAddr,IP_ADLEN);
      sNetAddr.dwRemoteHost = 16;
      iNameLen = sizeof(SOCKADDR_IN6);
      sNetAddr.dwScopeId = ((SOCKADDR_IN6 *)name)->sin6_scope_id;
   }
#endif
   else {
      errno =  WSAEAFNOSUPPORT;
      goto Exit_connect;
   }
   if (namelen < iNameLen) {
      errno = WSAEFAULT;
      goto Exit_connect;
   }

   psScb->sIsockRemote.wPort =
   sNetAddr.wRemotePort  = ntohs(psTo->sin_port);
   sNetAddr.wLocalPort   = psScb->sIsockLocal.wPort;

   if (psScb->bType == SOCK_STREAM) {
      iType = STREAM;
      for (i = ((int)sNetAddr.dwRemoteHost) / 4 - 1;i >= 0;--i) {
         if (sNetAddr.sIpAddr.adwAddr[i])
            goto dest_ok;
      }
      errno = WSAEDESTADDRREQ;
      goto Exit_connect;
      dest_ok:;
   }
   else {
      iType = DATA_GRAM;
   }
   if (!(psScb->wFlags & SF_BOUND)) {
      psScb->wFlags |= SF_BOUND;
   }
   psScb->wFlags |= SF_CONNECTED;
#ifdef DEBUG_CONNECT
   {
      BYTE bAddr[16];
      char sz[80];

      if (sNetAddr.dwRemoteHost == 4)
         *(DWORD *)bAddr = ntohl(sNetAddr.sIpAddr.dwAddr);
      else
         Convert128(&sNetAddr.sIpAddr,bAddr);
      printf("ConnectSocket(%d,%d,%lx [%s]:%u %u)\n",s,iType,
         sNetAddr.dwRemoteHost,
         inet_ntop(sNetAddr.dwRemoteHost == 4 ? AF_INET : AF_INET6,bAddr,sz,80),
         sNetAddr.wRemotePort,sNetAddr.wLocalPort);
   }
#endif //DEBUG_CONNECT
   if (ConnectSocket(s,iType,&sNetAddr) < 0) {
      errno = iNetErrNo == ERR_RESET ? WSAECONNREFUSED : _MapError(iNetErrNo);
      goto Exit_connect;
   }
   iRet = 0;

Exit_connect:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("connect: %u, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_CONNECT

#ifdef _SAPI_CNVRT128
void Convert128(void *psIpSource,void *psIpDest)
{
   int i;
   BYTE *pbDest = (BYTE *)psIpDest + IP_ADLEN;
   BYTE *pbSource = (BYTE *)psIpSource;

   for (i = 0;i < IP_ADLEN;++i)
            *--pbDest = *pbSource++;
}
#endif //_SAPI_CNVRT128

//===========================================================================
#ifdef _SAPI_IOCTLSOC

int ioctlsocket(SOCKET s, long cmd, u_long *argp)
{
   SCB *psScb = _psScbs[s - S_OFFSET];
   int iRet = SOCKET_ERROR,iVal;

   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_ioctlsocket;
   }
   if (!argp) {
      errno = WSAEINVAL;
      goto Exit_ioctlsocket;
   }
   switch (cmd) {
   case FIONBIO:
      if (!*argp) {  // disable non-blocking
         psScb->wFlags &= ~SF_NONBLOCK;
      }
      else {
         psScb->wFlags |= SF_NONBLOCK;
      }
      if (SetSocketOption(s,0,NET_OPT_NON_BLOCKING,*argp,sizeof(u_long)) < 0)
         errno = _MapError(iNetErrNo);
      break;
   case FIONREAD:
      iVal = _GetReadCount(s,psScb);
      *argp = iVal > 0 ? iVal : 0;
      break;
   case SIOCATMARK:
      *argp = 1;           // always lie about OOB
      break;
   default:
      break;
   }

   iRet = 0;

Exit_ioctlsocket:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("ioctlsocket: %d,x%lx return %d %ld",s, cmd, iRet, *argp);
   }
#endif
   return iRet;
}
#endif   // _SAPI_IOCTLSOC

//===========================================================================
#ifdef _SAPI_GETPEERN
int getpeername (SOCKET s, struct sockaddr *name,int *namelen)
{
   SCB *psScb = _psScbs[s - S_OFFSET];
   int iRet = SOCKET_ERROR;
   NET_ADDR sNetAddr;

   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_getpeername;
   }
   if (!(psScb->wFlags & SF_BOUND)) {
      errno = WSAEINVAL;
      goto Exit_getpeername;
   }
   if (GetPeerAddressEx(s,&sNetAddr) < 0) {
      errno = _MapError(iNetErrNo);
      goto Exit_getpeername;
   }

   if (sNetAddr.dwRemoteHost == 4) {
      if (!namelen || *namelen < sizeof(SOCKADDR_IN)) {
         errno = WSAEFAULT;
         goto Exit_getpeername;
      }
      ((SOCKADDR_IN *)name)->sin_addr.s_addr = htonl(sNetAddr.sIpAddr.dwAddr);
      ((SOCKADDR_IN *)name)->sin_port = htons(sNetAddr.wRemotePort);
      ((SOCKADDR_IN *)name)->sin_family = AF_INET;
      *namelen = sizeof(SOCKADDR_IN);
   }
#ifdef IPV6
   else {
      if (!namelen || *namelen < sizeof(SOCKADDR_IN6)) {
         errno = WSAEFAULT;
         goto Exit_getpeername;
      }
      Convert128(sNetAddr.sIpAddr.abAddr,&((SOCKADDR_IN6 *)name)->sin6_addr);
      ((SOCKADDR_IN6 *)name)->sin6_port = htons(sNetAddr.wRemotePort);
      ((SOCKADDR_IN6 *)name)->sin6_family = AF_INET6;
      *namelen = sizeof(SOCKADDR_IN6);
   }
#endif

   iRet = 0;

Exit_getpeername:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("getpeername: %u, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_GETPEERN

//===========================================================================
#ifdef _SAPI_GETSOCKN
int getsockname(SOCKET s, struct sockaddr *name,int * namelen)
{
   SCB *psScb = _psScbs[s - S_OFFSET];
   int iRet = SOCKET_ERROR;
   NET_ADDR sNetAddr;

   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_getsockname;
   }
   if (!(psScb->wFlags & SF_BOUND)) {
      errno = WSAEINVAL;
      goto Exit_getsockname;
   }
   if (psScb->wFlags & SF_INET6) {
      if (!namelen || *namelen < sizeof(SOCKADDR_IN6)) {
         errno = WSAEFAULT;
         goto Exit_getsockname;
      }
      if (!psScb->sIsockLocal.sIpAddress.adwAddr[0] &&
          !psScb->sIsockLocal.sIpAddress.adwAddr[1] &&
          !psScb->sIsockLocal.sIpAddress.adwAddr[2] &&
          !psScb->sIsockLocal.sIpAddress.adwAddr[3]) {
         GetAddressEx(s,&sNetAddr);
         memcpy(&psScb->sIsockLocal.sIpAddress,&sNetAddr.sIpAddr,IP_ADLEN);
      }
      memset(name,0,sizeof(SOCKADDR_IN6));
      Convert128(&psScb->sIsockLocal.sIpAddress,&((SOCKADDR_IN6 *)name)->sin6_addr);
      ((SOCKADDR_IN6 *)name)->sin6_port = htons(psScb->sIsockLocal.wPort);
      ((SOCKADDR_IN6 *)name)->sin6_family = AF_INET6;
      *namelen = sizeof(SOCKADDR_IN6);
   }
   else {
      if (!namelen || *namelen < sizeof(SOCKADDR_IN)) {
         errno = WSAEFAULT;
         goto Exit_getsockname;
      }
      if (!psScb->sIsockLocal.sIpAddress.dwAddr) {
         psScb->sIsockLocal.sIpAddress.dwAddr = ntohl(GetAddress(s));
      }
      ((SOCKADDR_IN *)name)->sin_addr.s_addr = htonl(psScb->sIsockLocal.sIpAddress.dwAddr);
      ((SOCKADDR_IN *)name)->sin_port = htons(psScb->sIsockLocal.wPort);
      ((SOCKADDR_IN *)name)->sin_family = AF_INET;
      *namelen = sizeof(SOCKADDR_IN);
   }
   iRet = 0;

Exit_getsockname:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("getsockname: %d, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_GETSOCKN

//===========================================================================
#ifdef _SAPI_GETSOCKO
int getsockopt (SOCKET s, int level, int optname,
                     char * optval, int *optlen)
{
   SCB *psScb = _psScbs[s - S_OFFSET];
   int iRet = SOCKET_ERROR;
   int *pval = (int *)optval;

   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_getsockopt;
   }
   if (!optname || !optlen) {
      errno = WSAEINVAL;
      goto Exit_getsockopt;
   }
   switch (level) {
   case IPPROTO_TCP:
      if (psScb->bType != SOCK_STREAM) {
         errno = WSAENOPROTOOPT;
         break;
      }
      switch (optname) {
      case TCP_NODELAY:
         if (*optlen < sizeof(int)) {
            errno = WSAEFAULT;
            goto Exit_getsockopt;
         }
         *optlen = sizeof(int);
         *pval = ((psScb->wTcpOption & optname) != 0);
         iRet = 0;
         break;
      default:
         errno = WSAENOPROTOOPT;
         break;
      }
      break;
   case SOL_SOCKET:
      if ((psScb->bType == SOCK_STREAM && (optname == SO_BROADCAST)) ||
         (psScb->bType == SOCK_DGRAM && (optname == SO_ACCEPTCONN ||
                               optname == (int)SO_DONTLINGER ||
                               optname == SO_KEEPALIVE ||
                               optname == SO_LINGER ||
                               optname == SO_OOBINLINE))) {
         errno = WSAENOPROTOOPT;
         goto Exit_getsockopt;
      }
      if (optname != SO_LINGER) {
         if (*optlen < sizeof(int)) {
            errno = WSAEFAULT;
            goto Exit_getsockopt;
         }
      }
      switch (optname) {
      case SO_LINGER:
         if (*optlen < sizeof(struct linger)) {
            errno = WSAEFAULT;
            goto Exit_getsockopt;
         }
         *optlen = sizeof(struct linger);
         ((struct linger *)optval)->l_linger = psScb->linger_time;
         ((struct linger *)optval)->l_onoff =
                                       (WORD)((psScb->wOption & optname) != 0);
         iRet = 0;
         break;
      case SO_ACCEPTCONN:
         *pval = psScb->wFlags & SF_LISTEN ? 1 : 0;
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_ERROR:
         *pval = 0;     // until I find out what it is supposed to be!
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_BROADCAST:
      case SO_DEBUG:
      case SO_DONTROUTE:
      case SO_KEEPALIVE:
      case SO_OOBINLINE:
      case SO_REUSEADDR:
         *pval = ((psScb->wOption & optname) != 0);
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_TYPE:
         *pval = psScb->bType;
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_RCVBUF:
         *pval = psScb->wRcvBufSize;
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_SNDBUF:
         *pval = psScb->wSndBufSize;
         *optlen = sizeof(int);
         iRet = 0;
         break;
      case SO_DONTLINGER:
         *pval = ((psScb->wOption & SO_LINGER) == 0);
         *optlen = sizeof(int);
         iRet = 0;
         break;
      default:
         errno = WSAENOPROTOOPT;
         break;
      }
      break;
   default:
      errno = WSAEINVAL;
      break;
   }

   Exit_getsockopt:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("getsockopt: %d, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_GETSOCKO

//===========================================================================
#ifdef _SAPI_HTONL
u_long   htonl (u_long hostlong)
{
   char tmp[4];

   tmp[3] = ((char *)&hostlong)[0];
   tmp[2] = ((char *)&hostlong)[1];
   tmp[1] = ((char *)&hostlong)[2];
   tmp[0] = ((char *)&hostlong)[3];
   return *(u_long *)&tmp;
}
#endif   // _SAPI_HTONL

//===========================================================================
#ifdef _SAPI_HTONS
u_short   htons (u_short hostshort)
{
   char tmp[2];

   tmp[1] = ((char *)&hostshort)[0];
   tmp[0] = ((char *)&hostshort)[1];
   return *((u_short *)&tmp);
}
#endif   // _SAPI_HTONS

//===========================================================================
#ifdef _SAPI_INETADDR
unsigned long inet_addr(const char * cp)
{
   int dots;
   char *p;
   char addr[4];
   unsigned long part[4];

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("inet_addr");
#endif
   memset(part,0,sizeof(part));
   for (p = (char *)cp,dots = 0;*p;p++) {
      if (isdigit(*p)) {
         part[dots] = part[dots] * 10 + (*p - '0');
      }
      else if (*p == '.') {
         if (++dots > 3)
            return INADDR_NONE;
      }
      else
         return INADDR_NONE;
   }
   switch (dots) {
   case 0:
      addr[0] = ((char *)&part[0])[3];
      addr[1] = ((char *)&part[0])[2];
      addr[2] = ((char *)&part[0])[1];
      addr[3] = ((char *)&part[0])[0];
      break;
   case 1:
      if (part[0] > 255 || part[1] > 0xffffffl)
         return INADDR_NONE;
      addr[0] = (char)part[0];
      addr[1] = ((char *)&part[1])[2];
      addr[2] = ((char *)&part[1])[1];
      addr[3] = ((char *)&part[1])[0];
      break;
   case 2:
      if (part[0] > 255 || part[1] > 255 || part[2] > 0xffffl)
         return INADDR_NONE;
      addr[0] = (char)part[0];
      addr[1] = (char)part[1];
      addr[2] = ((char *)&part[2])[1];
      addr[3] = ((char *)&part[2])[0];
      break;
   case 3:
      if (part[0] > 255 || part[1] > 255 || part[2] > 255 || part[3] > 255)
         return INADDR_NONE;
      addr[0] = (char)part[0];
      addr[1] = (char)part[1];
      addr[2] = (char)part[2];
      addr[3] = (char)part[3];
      break;
   default:
      return INADDR_NONE;
   }
   return *((unsigned long *)&addr);
}
#endif   // _SAPI_INETADDR

//===========================================================================
#ifdef _SAPI_INETNTOA
char *inet_ntoa (struct in_addr in)
{
   static char sz[18];

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("inet_ntoa");
#endif
   sprintf(sz,"%u.%u.%u.%u",in.S_un.S_un_b.s_b1,in.S_un.S_un_b.s_b2,
                            in.S_un.S_un_b.s_b3,in.S_un.S_un_b.s_b4);
   return sz;
}
#endif   // _SAPI_INETNTOA

#ifdef _SAPI_INETNTOP
const char *inet_ntop(int af, const void *src, char *dst, size_t size)
{
   char *psz = dst;
   BYTE *pb;
   int i,iZero = 0,iNumZeros = 0,iNumZerosCurrent = 0,iZeroCurrent = -1;
   WORD wVal;
   BYTE *pbIpAddress = (BYTE *)src;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("inet_ntop");
#endif
#define get16(p) ((WORD)((((WORD)p[0]) << 8) | p[1]))


   if (af == AF_INET) {
      if (size < INET_ADDRSTRLEN) {
         errno = WSAEFAULT;
         return 0;
      }
      sprintf(dst,"%u.%u.%u.%u",
         pbIpAddress[0],
         pbIpAddress[1],
         pbIpAddress[2],
         pbIpAddress[3]);
      return dst;
   }
   if (af != AF_INET6) {
      errno = WSAEFAULT;
      return 0;
   }
   if (size < INET6_ADDRSTRLEN) {
      errno = WSAEAFNOSUPPORT;
      return 0;
   }
   for (i = 0,pb = pbIpAddress;i < 17;i += 2,pb += 2) {
      if (i < 16 && *(WORD *)pb == 0) {
         iNumZerosCurrent += 2;
         if (iZeroCurrent < 0)
            iZeroCurrent = i;
      }
      else {
         if (iNumZerosCurrent > iNumZeros) {
            iNumZeros = iNumZerosCurrent;
            iZero = iZeroCurrent;
         }
         iNumZerosCurrent = 0;
         iZeroCurrent = -1;
      }
   }
   for (i = 0,pb = pbIpAddress;i < 16;i += 2,pb += 2) {
      if ((wVal = get16(pb)) != 0 || i <= iZero || i > iZero + iNumZeros) {
         if (!wVal && i == iZero) {
            *psz++ = ':';
            if (!i)
               *psz++ = ':';
         }
         else
            psz += sprintf(psz,"%X%s",wVal,i == 14 ? "" : ":");
      }
   }
   *psz = 0;
   return dst;
}
#endif   // _SAPI_INETNTOP

#ifdef _SAPI_GETADDRI
int getaddrinfo(const char *nodename, const char *servname,
                      const struct addrinfo *hints,
                      struct addrinfo **res)
{
   char *psz;
   struct servent *psServEnt;
   int iFlags;
   int iFamily;
   int iSockType;
   int iProtocol;
   int i = 0;
   WORD awPort[2] = {0,0}; // TCP - UDP ports
   BYTE abUse[4] = {0,0,0,0};
   enum {INET_TCP,INET_UDP,INET6_TCP,INET6_UDP};
   enum {FL_PROTO = 1,FL_FAM,FL_NONE = 4};
   enum {LEN_IP4 = 4,LEN_IP6 = 16};
   NET_ADDR asNetAddr[2];
   struct addrinfo *psAddrInfo;
   struct addrinfo *psAddrInfoTail;
   struct addrinfo *psAdInf;
   SOCKADDR *psSockAddr;

   if (res)
      *res = 0;
   errno = WSAEINVAL;
   if (!nodename && !servname)
      return EAI_NONAME;
   if (hints) {
      iFlags = hints->ai_flags;      /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
      iFamily = hints->ai_family;    /* PF_xxx */
      iSockType = hints->ai_socktype;/* SOCK_xxx */
      iProtocol = hints->ai_protocol;/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
   }
   else {
      iFlags = iSockType = iProtocol = 0;
      iFamily = PF_UNSPEC;
   }
   if (iFlags & ~(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST))
      return EAI_BADFLAGS;
   if (iSockType != 0 && iSockType != SOCK_STREAM && iSockType != SOCK_DGRAM)
      return EAI_SOCKTYPE;
   if (iFamily != PF_UNSPEC && iFamily != PF_INET && iFamily != PF_INET6)
      return EAI_FAMILY;
   if (iProtocol != 0 && iProtocol != IPPROTO_TCP && iProtocol != IPPROTO_UDP)
      return EAI_PROTOCOL;
   if (iSockType == SOCK_STREAM) {
      if (iProtocol == IPPROTO_UDP)
         return EAI_BADHINTS;
      iProtocol = IPPROTO_TCP;
   }
   else if (iSockType == SOCK_DGRAM) {
      if (iProtocol == IPPROTO_TCP)
         return EAI_BADHINTS;
      iProtocol = IPPROTO_UDP;
   }
   else if (iProtocol)
      iSockType = iProtocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
   psAddrInfo = psAddrInfoTail = 0;
   if (servname) {
      long lPort = 0;

      for (psz = (char *)servname;*psz;++psz) {
         if (!isdigit(*psz)) {
            lPort = -1;
            break;
         }
         lPort = lPort * 10 + *psz - '0';
      }
      if (lPort <= 0 || lPort > 0xffff) {
         if (!iProtocol || iProtocol == IPPROTO_TCP) {
            if ((psServEnt = getservbyname(servname,"tcp")) != 0) {
               awPort[INET_TCP] = psServEnt->s_port;
               abUse[INET_TCP] = abUse[INET6_TCP] = FL_PROTO;
            }
         }
         if (!iProtocol || iProtocol == IPPROTO_UDP) {
            if ((psServEnt = getservbyname(servname,"udp")) != 0) {
               awPort[INET_UDP] = psServEnt->s_port;
               abUse[INET_UDP] = abUse[INET6_UDP] = FL_PROTO;
            }
         }
         if (!abUse[INET_TCP] && !abUse[INET_UDP] &&
                                    !abUse[INET6_TCP] && !abUse[INET6_UDP])
            return EAI_SERVICE;
      }
      else {
         if (!iProtocol || iProtocol == IPPROTO_TCP) {
            awPort[INET_TCP] = htons((WORD)lPort);
            abUse[INET_TCP] = abUse[INET6_TCP] = FL_PROTO;
         }
         if (!iProtocol || iProtocol == IPPROTO_UDP) {
            awPort[INET_UDP] = htons((WORD)lPort);
            abUse[INET_UDP] = abUse[INET6_UDP] = FL_PROTO;
         }
      }
   }
   else
      abUse[INET_TCP] = abUse[INET_UDP] =
                                 abUse[INET6_TCP] = abUse[INET6_UDP] = FL_NONE;
   memset(asNetAddr,0,sizeof(asNetAddr));
   if (nodename) {
      if (iFamily == PF_UNSPEC || iFamily == PF_INET) {
         if (GetAddressInfo((char *)nodename,
                  iFlags & AI_NUMERICHOST ? AI_PARSE :
                                            (AI_PARSE | AI_HOSTTAB | DNS_IPV4),
                  &asNetAddr[0]) == LEN_IP4) {
            abUse[INET_TCP] |= FL_FAM;
            abUse[INET_UDP] |= FL_FAM;
         }
      }
      if (iFamily == PF_UNSPEC || iFamily == PF_INET6) {
         if (GetAddressInfo((char *)nodename,
                  iFlags & AI_NUMERICHOST ? AI_PARSE :
                                            (AI_PARSE | AI_HOSTTAB | DNS_IPV6),
                  &asNetAddr[1]) == LEN_IP6) {
            abUse[INET6_TCP] |= FL_FAM;
            abUse[INET6_UDP] |= FL_FAM;
         }
      }
   }
   else {
      if (iFamily == PF_UNSPEC || iFamily == PF_INET) {
         if (!(iFlags & AI_PASSIVE))
            asNetAddr[0].sIpAddr.dwAddr = 0x7f000001l;   // IPv4 loopback
         abUse[INET_TCP] |= FL_FAM;
         abUse[INET_UDP] |= FL_FAM;
      }
      if (iFamily == PF_UNSPEC || iFamily == PF_INET6) {
         if (!(iFlags & AI_PASSIVE))
            asNetAddr[1].sIpAddr.abAddr[0] = 1; // IPv6 loopback
         abUse[INET6_TCP] |= FL_FAM;
         abUse[INET6_UDP] |= FL_FAM;
      }
   }
   for (i = 0;i < 4;++i) if (abUse[i] > FL_FAM) {
      if ((psAdInf = calloc(1,
                  sizeof(struct addrinfo) + sizeof(SOCKADDR_IN6))) == 0) {
         freeaddrinfo(psAddrInfo);
         errno = WSAENOBUFS;
         return EAI_MEMORY;
      }
      psSockAddr = (SOCKADDR *)(psAdInf + 1);
      psAdInf->ai_flags = iFlags;
      if (i & INET6_TCP) {
         Convert128(asNetAddr[1].sIpAddr.abAddr,
                        &((SOCKADDR_IN6 *)psSockAddr)->sin6_addr);
         ((SOCKADDR_IN6 *)psSockAddr)->sin6_scope_id = asNetAddr[1].dwScopeId;
         psAdInf->ai_addrlen = sizeof(SOCKADDR_IN6);
         psAdInf->ai_family = AF_INET6;
      }
      else {
         ((SOCKADDR_IN *)psSockAddr)->sin_addr.s_addr =
                                       htonl(asNetAddr[0].sIpAddr.dwAddr);
         psAdInf->ai_addrlen = sizeof(SOCKADDR);
         psAdInf->ai_family = AF_INET;
      }
      ((SOCKADDR_IN *)psSockAddr)->sin_port = awPort[i & INET_UDP];
      psSockAddr->sa_family = psAdInf->ai_family;
      psAdInf->ai_socktype = (abUse[i] & FL_PROTO) ?
                              (i & INET_UDP ? SOCK_DGRAM : SOCK_STREAM) : 0;
      psAdInf->ai_protocol = (abUse[i] & FL_PROTO) ?
                              (i & INET_UDP ? IPPROTO_UDP : IPPROTO_TCP) : 0;
      psAdInf->ai_canonname = (char *)nodename;
      psAdInf->ai_addr = psSockAddr;
      if (psAddrInfo) {
         psAddrInfoTail->ai_next = psAdInf;
         psAddrInfoTail = psAdInf;
      }
      else {
         psAddrInfo = psAddrInfoTail = psAdInf;
      }
   }
   if (psAddrInfo == 0) {
      errno = WSAHOST_NOT_FOUND;
      return EAI_FAIL;
   }
   if (res)
      *res = psAddrInfo;
   errno = 0;
   return 0;
}

void freeaddrinfo(struct addrinfo *ai)
{
   struct addrinfo *psNext;

   while (ai) {
      psNext = ai->ai_next;
      free(ai);
      ai = psNext;
   }
}

#endif // _SAPI_GETADDRI

#ifdef _SAPI_GAISTRER

char *apszAddrInfoErrors[] = {
   "No error",
   "Bad flags",
   "No name",
   "Temporary failure",
   "Permanent failure",
   0,
   "Family not supported",
   "Type not supported",
   "Service not supported",
   0,
   "No memory",
   "System error",
   "Bad hints",
   "Protocol unknown"
};

char *gai_strerror(int ecode)
{
   static char szUnknown[30];
   char *psz;

   if (ecode > 0 || ecode < EAI_PROTOCOL ||
                                 (psz = apszAddrInfoErrors[-ecode]) == 0) {
      sprintf(szUnknown,"Unknown error %d",ecode);
      psz = szUnknown;
   }
   return psz;
}
#endif // _SAPI_GAISTRER

//===========================================================================
#ifdef _SAPI_LISTEN
int listen (SOCKET s, int backlog)
{
   SCB *psScb;
   int iRet = SOCKET_ERROR;
   NET_ADDR sNetAddr;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_listen;
   }
   if (!(psScb->wFlags & SF_BOUND)) {
      errno = WSAEINVAL;
      goto Exit_listen;
   }
   if (psScb->sIsockLocal.wPort == 0) {
      psScb->sIsockLocal.wPort = _GetNextPortNum();
   }
   if (backlog < 1)
      backlog = 1;
   else if (backlog > 5)
      backlog = 5;
   memset(&sNetAddr,0,sizeof(NET_ADDR));
   sNetAddr.bProtocol = STREAM;
   sNetAddr.wLocalPort = psScb->sIsockLocal.wPort;
   if (ListenAcceptSocket(s,STREAM,backlog,&sNetAddr) < 0) {
      errno = _MapError(iNetErrNo);
      goto Exit_listen;
   }
   psScb->wFlags |= SF_LISTEN;
   iRet = 0;

Exit_listen:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("listen: %d, return %d", s, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_LISTEN

//===========================================================================
#ifdef _SAPI_NTOHL
u_long ntohl (u_long netlong)
{
   char tmp[4];

   tmp[3] = ((char *)&netlong)[0];
   tmp[2] = ((char *)&netlong)[1];
   tmp[1] = ((char *)&netlong)[2];
   tmp[0] = ((char *)&netlong)[3];
   return *(u_long *)&tmp;
}
#endif   // _SAPI_NTOHL

//===========================================================================
#ifdef _SAPI_NTOHS
u_short ntohs (u_short netshort)
{
   char tmp[2];

   tmp[1] = ((char *)&netshort)[0];
   tmp[0] = ((char *)&netshort)[1];
   return *(u_short *)&tmp;
}
#endif   // _SAPI_NTOHS

//===========================================================================
#ifdef _SAPI_RECV
int recv(SOCKET s, char * buf, int len, int flags)
{
   return recvfrom(s,buf,len,flags,0,0);
}
#endif   // _SAPI_RECV

//===========================================================================
#ifdef _SAPI_RECVFROM
int recvfrom(SOCKET s, char * buf, int len, int flags,
                   struct sockaddr *from, int * fromlen)
{
   SCB *psScb;
   int iRet = SOCKET_ERROR;
   NET_ADDR sNetAddr,*psFrom;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_recvfrom;
   }
   if (!(psScb->wFlags & SF_BOUND)) {
      errno = WSAEINVAL;
      goto Exit_recvfrom;
   }
   if (from && fromlen) {
      if (*fromlen < sizeof(SOCKADDR)) {
         errno = WSAEFAULT;
         iRet = INVALID_SOCKET;
         goto Exit_recvfrom;
      }
      psFrom = &sNetAddr;
   }
   else
      psFrom = 0;

   if (psScb->bType == SOCK_DGRAM && _CheckUdpOpen(s,psScb) < 0)
      goto Exit_recvfrom;
   if ((len = ReadSocket(s,buf,len,psFrom,
                  (flags & (NET_FLG_OOB | NET_FLG_PEEK)) | NET_FLG_EXT)) >= 0)
      iRet = len;
   else if (iNetErrNo == ERR_EOF)
      errno = iRet = 0;
   else if (iNetErrNo == ERR_NOT_ESTAB)
      errno = WSAEWOULDBLOCK;
   else
      errno = _MapError(iNetErrNo);

   if (psFrom) {
      if (sNetAddr.dwRemoteHost == 4) {
         if (*fromlen < sizeof(SOCKADDR_IN)) {
            errno = WSAEFAULT;
            goto Exit_recvfrom;
         }
         ((SOCKADDR_IN *)from)->sin_addr.s_addr = htonl(sNetAddr.sIpAddr.dwAddr);
         ((SOCKADDR_IN *)from)->sin_port = htons(sNetAddr.wRemotePort);
         ((SOCKADDR_IN *)from)->sin_family = AF_INET;
         *fromlen = sizeof(SOCKADDR_IN);
      }
#ifdef IPV6
      else {
         if (*fromlen < sizeof(SOCKADDR_IN6)) {
            errno = WSAEFAULT;
            goto Exit_recvfrom;
         }
         Convert128(sNetAddr.sIpAddr.abAddr,&((SOCKADDR_IN6 *)from)->sin6_addr);
         ((SOCKADDR_IN6 *)from)->sin6_port = htons(sNetAddr.wRemotePort);
         ((SOCKADDR_IN6 *)from)->sin6_family = AF_INET6;
         *fromlen = sizeof(SOCKADDR_IN6);
      }
#endif
   }

Exit_recvfrom:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      char str[20],*ps,*pcData;
      int i;

      for (i = 0,ps = buf,pcData = str;i < iRet && i < 16;++i,++ps,++pcData)
         *pcData = (isprint(*ps)) ? *ps : '.';
      *pcData = 0;
      SockDebug("recvfrom: %d '%s', return %d", s, str, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_RECVFROM

//===========================================================================
typedef struct {     // TCB with extra space
   TCB sTcb;
   char cExtra[20];  // for future expansion
} TCBEX;


#ifdef _SAPI_SELECT
#define TCBVAL(m) ((_bIpVersionFlags & IPVF_IPV6) ? sTcb.sTcb.m : ((TCB4 *)&sTcb.sTcb)->m)
#define TCBVAL4(m) ((TCB4 *)&sTcb.sTcb)->m
#define TCBVAL6(m) sTcb.sTcb.m
#include <conio.h>
#define FD_STDIN  0  // keyboard file descriptor

#ifdef __TURBOC__
#pragma argsused
#endif
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, const struct timeval *timeout)
{
   int i,iOutRead,iOutWrite,iOutExcept;
   SOCKET s;
   int retv = 0;
   SCB *psScb;
   DWORD dwTime,dwTimeoutTarget;
   TCBEX sTcb;
   unsigned uLen;
#ifdef SOCK_DEBUG
   if(_cTracing) {
      if (!timeout)
         SockDebug("select: no_timeout %s %s %s",
                                 readfds ? "read" : "",
                                 writefds ? "write" : "",
                                 exceptfds ? "except" : "");
      else
         SockDebug("select: time = %ld,%ld %s %s %s",
                                 timeout->tv_sec,timeout->tv_usec,
                                 readfds ? "read" : "",writefds ? "write" : "",
                                 exceptfds ? "except" : "");
   }
#endif
   dwTime = timeout ? (timeout->tv_sec * 1000 + timeout->tv_usec / 1000) : 1;
   dwTimeoutTarget = Clock() +
                     (dwTime * CLOCK_DIVISOR) / (1000 * CLOCK_MULTIPLIER);
   while (1) {
      if (readfds) {
         for (i = 0,iOutRead = 0;i < readfds->fd_count;i++) {
            psScb = _psScbs[(s = readfds->fd_array[i]) - S_OFFSET];
            if (s == FD_STDIN) {
               if (kbhit()) {
                  readfds->fd_array[iOutRead++] = s;
               }
               continue;
            }
            if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
               errno = WSAENOTSOCK;
               retv = SOCKET_ERROR;
               goto exit;
            }
            if (psScb->wFlags & SF_LISTEN) {
               uLen = sizeof(TCBEX);
               if (GetKernelInformation(s, K_INF_TCP_CB, 0,
                              (char *)&sTcb, &uLen) >= 0 && TCBVAL(num_nacc)) {
                  readfds->fd_array[iOutRead++] = s;
               }
            }
            else if (_GetReadCount(s,psScb) >= 0)
               readfds->fd_array[iOutRead++] = s;
         }
         retv += iOutRead;
      }
      if (writefds) {
         for (i = 0,iOutWrite = 0;i < writefds->fd_count;i++) {
            psScb = _psScbs[(s = writefds->fd_array[i]) - S_OFFSET];
            if (s == FD_STDIN) {
               continue;
            }
            if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
               errno = WSAENOTSOCK;
               retv = SOCKET_ERROR;
               goto exit;
            }
            if (psScb->bType == SOCK_STREAM) {
               uLen = sizeof(TCBEX);
               if (GetKernelInformation(s, K_INF_TCP_CB, 0,
                                                (char *)&sTcb, &uLen) >= 0) {
                  BYTE state;
                  WORD window,sndcnt;

                  if (_bIpVersionFlags & IPVF_IPV6) {
                     state = TCBVAL6(state);
                     window = TCBVAL6(window);
                     sndcnt = TCBVAL6(sndcnt);
                  }
                  else {
                     state = TCBVAL4(state);
                     window = TCBVAL4(window);
                     sndcnt = TCBVAL4(sndcnt);
                  }
                  if ((state == ESTABLISHED || state == CLOSE_WAIT) &&
                                                            window > sndcnt)
                     writefds->fd_array[iOutWrite++] = s;
               }
            }
            else if (psScb->bType == SOCK_DGRAM) {
               writefds->fd_array[iOutWrite++] = s;
            }
         }
         retv += iOutWrite;
      }
      if (exceptfds) {
         for (i = 0,iOutExcept = 0;i < exceptfds->fd_count;i++) {
            psScb = _psScbs[(s = exceptfds->fd_array[i]) - S_OFFSET];
            if (s == FD_STDIN) {
               continue;
            }
            if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
               errno = WSAENOTSOCK;
               retv = SOCKET_ERROR;
               goto exit;
            }
            if (psScb->bType == SOCK_STREAM) {
               uLen = sizeof(TCBEX);
               if (GetKernelInformation(s, K_INF_TCP_CB, 0,
                                 (char *)&sTcb, &uLen) < 0 || TCBVAL(reason)) {
                  exceptfds->fd_array[iOutExcept++] = s;
               }
            }
         }
         retv += iOutExcept;
      }
      if (retv || !dwTime ||
                  (timeout && (long)(Clock() - dwTimeoutTarget) >= 0)) {
         if (readfds)
            readfds->fd_count = iOutRead;
         if (writefds)
            writefds->fd_count = iOutWrite;
         if (exceptfds)
            exceptfds->fd_count = iOutExcept;
         break;
      }
   }
   exit:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("select exit");
   }
#endif
   return retv;
}
#endif   // _SAPI_SELECT

//===========================================================================
#ifdef _SAPI_SEND
int send(SOCKET s, const char * buf, int len, int wFlags)
{
   return sendto(s,buf,len,wFlags,0,0);
}
#endif   // _SAPI_SEND

//===========================================================================
#ifdef _SAPI_SENDTO
int sendto(SOCKET s, const char *buf, int len, int flags,
                  const struct sockaddr *to, int tolen)
{
   SCB *psScb;
   NET_ADDR sNetAddr;
   int iRet = SOCKET_ERROR;

   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      goto Exit_sendto;
   }
   if (psScb->bType == SOCK_STREAM) {
      if (!(psScb->wFlags & SF_BOUND)) {
         errno = WSAEINVAL;
         goto Exit_sendto;
      }
      if ((len = WriteSocket(s,(char *)buf,len,flags)) < 0)
         errno = _MapError(iNetErrNo);
      else
         iRet = len;
      if (psScb->wTcpOption & TCP_NODELAY)
         FlushSocket(s);
   }
   else {      // DGRAM
      SOCKADDR_IN *psTo = (SOCKADDR_IN *)to;

      if (_CheckUdpOpen(s,psScb) >= 0) {
         if (to && tolen) {
            //sNetAddr.bProtocol = DATA_GRAM;
            sNetAddr.wLocalPort = psScb->sIsockLocal.wPort;
            if (psScb->wFlags & SF_INET6) {
               sNetAddr.dwRemoteHost = 16;
               Convert128(&((SOCKADDR_IN6 *)to)->sin6_addr,sNetAddr.sIpAddr.abAddr);
            }
            else {
               sNetAddr.dwRemoteHost = 4;
               sNetAddr.sIpAddr.dwAddr = ntohl(psTo->sin_addr.s_addr);
            }
            sNetAddr.wRemotePort = ntohs(psTo->sin_port);
            if (psScb->wOption & SO_BROADCAST)
               flags |= NET_FLG_BROADCAST;
            if ((len = WriteToSocket(s,(char *)buf,len,&sNetAddr,flags)) < 0)
               errno = _MapError(iNetErrNo);
            else
               iRet = len;
         }
         else if ((len = WriteSocket(s,(char *)buf,len,flags)) < 0)
            errno = _MapError(iNetErrNo);
         else
            iRet = len;
      }
   }

Exit_sendto:

#ifdef SOCK_DEBUG
   if(_cTracing)  {
      char str[20],*pcData;
      const char *ps;
      int i;

      for (i = 0,ps = buf,pcData = str;i < len && i < 16;++i,++ps,++pcData)
         *pcData = (isprint(*ps)) ? *ps : '.';
      *pcData = 0;
      SockDebug("sendto: %u %d '%s', return %d", s, len, str, iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_SENDTO

//===========================================================================
#ifdef _SAPI_SETSOCKO
int setsockopt (SOCKET s, int level, int optname,
                     const char *optval, int optlen)
{
   SCB *psScb;
   int val;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("setsockopt: %u %d %d",s,level,optname);
#endif
   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      return SOCKET_ERROR;
   }
   if (!optname) {
      errno = WSAEINVAL;
      return SOCKET_ERROR;
   }
   if (!optval) {
      errno = WSAEFAULT;
      return SOCKET_ERROR;
   }
   val = *(int *)optval;

   switch (level) {
   case IPPROTO_TCP:
      if (psScb->bType != SOCK_STREAM) {
         errno = WSAENOPROTOOPT;
         return SOCKET_ERROR;
      }
      switch (optname) {
      case TCP_NODELAY:
         if (optlen != sizeof(int)) {
            errno = WSAEFAULT;
            return SOCKET_ERROR;
         }
         if (val)
            psScb->wTcpOption |= optname;
         else
            psScb->wTcpOption &= ~optname;
         break;
      default:
         errno = WSAENOPROTOOPT;
         return SOCKET_ERROR;
      }
      break;
   case SOL_SOCKET:
      if ((psScb->bType == SOCK_STREAM && optname == SO_BROADCAST) ||
         (psScb->bType == SOCK_DGRAM && (optname == (int)SO_DONTLINGER ||
                               optname == SO_KEEPALIVE ||
                               optname == SO_LINGER ||
                               optname == SO_OOBINLINE))) {
         errno = WSAENOPROTOOPT;
         return SOCKET_ERROR;
      }
      if ((optname != SO_LINGER && optlen != sizeof(int)) ||
                  (optname == SO_LINGER && optlen != sizeof(struct linger))) {
         errno = WSAEFAULT;
         return SOCKET_ERROR;
      }
      switch (optname) {
      case SO_KEEPALIVE:
         if (SetSocketOption(s,0,NET_OPT_KEEPALIVE,val,sizeof(DWORD)) < 0) {
               errno = _MapError(iNetErrNo);
               return SOCKET_ERROR;
         }
         goto set_opt;
      case SO_LINGER:
         psScb->linger_time = ((struct linger *)optval)->l_linger;
         val = ((struct linger *)optval)->l_onoff;
      case SO_BROADCAST:
      case SO_DEBUG:
      case SO_DONTROUTE:
      case SO_OOBINLINE:
      case SO_REUSEADDR:
         set_opt:
         if (val)
            psScb->wOption |= optname;
         else
            psScb->wOption &= ~optname;
         break;
      case SO_RCVBUF:
         psScb->wRcvBufSize = val;
         break;
      case SO_SNDBUF:
         psScb->wSndBufSize = val;
         break;
      case SO_DONTLINGER:
         if (val)
            psScb->wOption &= SO_DONTLINGER;
         else
            psScb->wOption |= ~SO_DONTLINGER;
         break;
      default:
         errno = WSAENOPROTOOPT;
         return SOCKET_ERROR;
      }
      break;
   default:
      errno = WSAEINVAL;
      return SOCKET_ERROR;
   }
   return 0;
}
#endif   // _SAPI_SETSOCKO

//===========================================================================
#ifdef _SAPI_SHUTDOWN
int   shutdown (SOCKET s, int how)
{
   SCB *psScb;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("shutdown: %u %u",s,how);
#endif
   psScb = _psScbs[s - S_OFFSET];
   if (s >= (FD_SETSIZE + S_OFFSET) || !psScb) {
      errno = WSAENOTSOCK;
      return SOCKET_ERROR;
   }
   if (how > 0) {
      if (EofSocket(s) < 0) {
         errno = _MapError(iNetErrNo);
         return SOCKET_ERROR;
      }
   }
   return 0;
}
#endif   // _SAPI_SHUTDOWN

//===========================================================================
#ifdef _SAPI_SOCKET
SOCKET socket(int af, int type, int protocol)
{
   SCB *psScb;
   SOCKET s;
   SOCKET iRet = INVALID_SOCKET;

   if (!_bIpVersionFlags) {
#ifdef IPV6
      int i;
      if ((i = GetIpVersionFlags()) < 0)
         goto Exit_socket;
      _bIpVersionFlags = (BYTE)i;
#else
      _bIpVersionFlags = IPVF_IPV4;
#endif
   }
   if ((af != AF_INET && af != AF_INET6) ||
         (af == AF_INET && !(_bIpVersionFlags & IPVF_IPV4)) ||
         (af == AF_INET6 && !(_bIpVersionFlags & IPVF_IPV6))) {
      errno = WSAEAFNOSUPPORT;
      goto Exit_socket;
   }
   if (type != SOCK_STREAM && type != SOCK_DGRAM) {
      errno = WSAESOCKTNOSUPPORT;
      goto Exit_socket;
   }
   if (protocol) {
      if ((protocol != IPPROTO_TCP) &&
          (protocol != IPPROTO_UDP)) {
          errno = WSAEPROTONOSUPPORT;
          goto Exit_socket;
      }
      if ((type == SOCK_STREAM && protocol != IPPROTO_TCP) ||
          (type == SOCK_DGRAM && protocol != IPPROTO_UDP)) {
          errno = WSAEPROTOTYPE;
          goto Exit_socket;
      }
   }
   if ((s = GetSocket()) == INVALID_SOCKET) {
      errno = _MapError(iNetErrNo);
      goto Exit_socket;
   }
   psScb = _psScbs[s - S_OFFSET];
   if (psScb) {
      ReleaseSocket(s);
      errno = WSAEEXIST;
      goto Exit_socket;
   }
   if ((psScb = calloc(1,sizeof(SCB))) == 0) {
      ReleaseSocket(s);
      errno = WSAENOBUFS;
      goto Exit_socket;
   }
   _psScbs[s - S_OFFSET] = psScb;
   psScb->bType = type;
   iRet = s;
   if (af == AF_INET6)
      psScb->wFlags = SF_INET6;
   psScb->wRcvBufSize = 1460;
   psScb->wSndBufSize = 1460;
Exit_socket:
#ifdef SOCK_DEBUG
   if(_cTracing) {
      char *types[] = {"unknown","stream","dgram","raw"};
      SockDebug("socket: %s, return %d",
                           types[(unsigned)type > SOCK_RAW ? 0 : type], iRet);
   }
#endif
   return iRet;
}
#endif   // _SAPI_SOCKET

// Database functions

//===========================================================================
#ifdef __TURBOC__
#pragma argsused
#endif
#ifdef _SAPI_GHOSTBYA
struct hostent *gethostbyaddr(const char * addr, int len, int type)
{

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("gethostbyaddr");
#endif
   errno = WSANO_DATA;
   return 0;
}
#endif   // _SAPI_GHOSTBYA

#ifdef _SAPI_GHOSTBYN
typedef struct wshostentry {  // Winsock host entry
   struct hostent he;         // host entry
   BYTE buf[128];             // buffer for name
   DWORD taddr[2];            // return one address
   DWORD *tptr[2];            // for h_addr_list
} WSHE;


struct hostent *gethostbyname(const char * name)
{
   static WSHE *phe;                   // pointer to host entry
   WSHE *ph;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("gethostbyname: %s",name);
#endif
   if (!phe && (phe = (WSHE *)calloc(1,sizeof(WSHE))) == 0) {
      errno = WSATRY_AGAIN;
      return 0;
   }
   ph = phe;
   strncpy((char *)ph->buf,name,127);
   ph->he.h_name = (char *)ph->buf;
   ph->he.h_aliases = (char **)&ph->tptr[1];
   ph->he.h_addrtype = PF_INET;        // host address type
   ph->he.h_length = 4;                // length of address
   ph->he.h_addr_list = (char **)ph->tptr; // list of addresses
   ph->tptr[0] = ph->taddr;

   // a name obtained by gethostname() must always return valid
   if (strcmp(_szHostname,name) == 0) {
      unsigned uSize = sizeof(DWORD);
      GetKernelInformation(0,K_INF_IP_ADDR,0,ph->taddr,&uSize);
      return &ph->he;
   }
   if ((ph->taddr[0] = ResolveName((char *)name,(char *)ph->buf,
                                                      sizeof(ph->buf))) == 0) {
      errno = _MapError(iNetErrNo);
      return 0;
   }
   return &ph->he;
}
#endif   // _SAPI_GHOSTBYN

//===========================================================================
#ifdef _SAPI_GHOSTNAM
int gethostname(char *name, int namelen)
{

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("gethostname");
#endif
   if (GetKernelInformation(0,K_INF_HOSTNAME,0,name,(unsigned *)&namelen) < 0) {
      errno = WSAEFAULT;
      return SOCKET_ERROR;
   }
   strcpy(_szHostname,name);
   return 0;
}
#endif   // _SAPI_GHOSTNAM

#ifdef _SAPI_DATABASE
#define  TEST(c, i)  (isalpha(c) || (i && (isdigit(c)  || ((c) == '-') \
         || ((c) == '_') || ((c) == '.'))))

struct   servent  _sSe;
struct   protoent _sPe;
BYTE _bIpVersionFlags;

#define     BUFLEN 128

FILE *_OpenServiceFile(char *pszName)     // open services/protocol file
{
   char szServiceFile[100];
   char *pszEnv;

   if ((pszEnv = getenv("SOCKETS")) == 0)
      pszEnv = "\\dl\\sockets";
   if (pszEnv[strlen(pszEnv) - 1] != '\\')
      strcat(pszEnv,"\\");
   sprintf(szServiceFile,"%s%s",pszEnv,pszName);
   return fopen(szServiceFile,"rt");
}

//===========================================================================
int _GetServiceEntry(FILE *psFile,char **ppszName,char ***pppszAlias,
            short *psiNum,char **ppszProto)
{
   static char cBuf[BUFLEN], *pcPtr[10];
   char  *p, lbuf[BUFLEN],*pcData = cBuf;
   int   i, j, n, ptrlen;
   int buflen = BUFLEN;

   do {
      if(fgets(lbuf, BUFLEN, psFile) == 0) {
         return 0;
      }
   } while(lbuf[0] == '#' || lbuf[0] == '\n');

   *ppszName = cBuf;
   for(i = 0, p = lbuf;buflen > 0 && TEST(*p, i); ++i,--buflen)
      *pcData ++ = *p++;

   if(! i) {
      return 0;
   }
   if (buflen-- > 0) {
      *pcData++ = '\0';
      p++;
   }

   while(isspace(*p))
      ++p;

   for(n = i = 0; isdigit(*p); ++i)
      n = n * 10 + *p++ - '0';

   if(! i)
      return 0;

   *psiNum = n;

   if (ppszProto) {
      if(*p++ != '/')
         return 0;

      *ppszProto = pcData;

      for (i = 0; buflen > 0 && (isalpha(*p) || (i && isdigit(*p)));
                                                                  ++i,--buflen)
         *pcData++ = *p++;

      if(! i)
         return 0;
      if (buflen-- > 0) {
         *pcData++ = '\0';
         ++p;
      }
   }
   for(j = 0; *p && j < 9 && buflen > 0;) {
      while(isspace(*p))
         ++p;

      if(! *p || *p == '#')
         break;

      pcPtr[j] = pcData;

      for(i = 0; buflen > 0 && TEST(*p, i); ++i,--buflen)
         *pcData++ = *p++;
      if(! i)
         continue;
      if (buflen-- > 0) {
         *pcData++ = '\0';
         ++p;
      }
      ++j;
   }

   pcPtr[j++] = NULL;      // terminate array of ptr's
   ptrlen = sizeof(char *) * j;
   if (buflen > ptrlen)
      memcpy(pcData,pcPtr,ptrlen);
   *pppszAlias = (char **)pcData;
   return buflen;
}
#endif   // _SAPI_DATABASE


//===========================================================================
#ifdef _SAPI_GSERVBYP
struct servent *getservbyport(int port, const char * proto)
{
   int retv,iErrNo;
   FILE *psFile;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("getservbyport: %u %s",port,proto ? proto : "no proto");
#endif
   if ((psFile = _OpenServiceFile("services")) == 0) {
      errno = WSANO_RECOVERY;
      return 0;
   }
   while(1) {
      if ((retv = _GetServiceEntry(psFile,&_sSe.s_name,&_sSe.s_aliases,
                                          &_sSe.s_port,&_sSe.s_proto)) > 0) {
         if(_sSe.s_port == ntohs(port) &&
                        (proto == NULL || stricmp(_sSe.s_proto, proto) == 0)) {
            _sSe.s_port = htons(_sSe.s_port);
            fclose(psFile);
            return &_sSe;
         }
      }
      else {
         iErrNo = retv < 0 ? WSAENOBUFS : WSANO_DATA;
         break;
      }
   }
   fclose(psFile);
   errno = iErrNo;
   return 0;
}
#endif   // _SAPI_GSERVBYP

#ifdef _SAPI_GSERVBYN
struct servent *getservbyname(const char *name,const char *proto)
{
   int   retv,i,iErrNo;
   FILE *psFile;

#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("getservbyname: %s %s",name,proto ? proto : "no proto");
   }
#endif
   if ((psFile = _OpenServiceFile("services")) == 0) {
      errno = WSANO_RECOVERY;
      return 0;
   }
   while(1) {
      if ((retv = _GetServiceEntry(psFile,&_sSe.s_name,&_sSe.s_aliases,
                                          &_sSe.s_port,&_sSe.s_proto)) == 0) {
         iErrNo = WSANO_DATA;
         break;
      }
      else if (retv < 0) {
         iErrNo = WSAENOBUFS;
         break;
      }
      _sSe.s_port = htons(_sSe.s_port);
      if(proto != NULL && stricmp(_sSe.s_proto, proto) != 0)
         continue;
      if(stricmp(_sSe.s_name,name) == 0) {
         fclose(psFile);
         return &_sSe;
      }
      for(i = 0; _sSe.s_aliases[i]; ++i) {
         if(stricmp(_sSe.s_aliases[i],name) == 0) {
            fclose(psFile);
            return &_sSe;
         }
      }
   }
   fclose(psFile);
   errno = iErrNo;
   return 0;
}
#endif  // _SAPI_GSERVBYN

//===========================================================================
#ifdef _SAPI_GPROTONU
struct protoent *getprotobynumber(int proto)
{
   int retv;
   FILE *psFile;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("getprotobynumber: %d", proto);
#endif
   if ((psFile = _OpenServiceFile("protocol")) == 0) {
      errno = WSANO_RECOVERY;
      return 0;
   }
   while(1) {
      if ((retv = _GetServiceEntry(psFile,&_sPe.p_name,&_sPe.p_aliases,
                                                      &_sPe.p_proto,0)) > 0) {
         if(_sPe.p_proto == proto) {
            fclose(psFile);
            return &_sPe;
         }
      }
      else if (retv < 0) {
         errno = WSAENOBUFS;
         break;
      }
      else {
         errno = WSANO_DATA;
         break;
      }
   }
   fclose(psFile);
   return 0;
}
#endif   // _SAPI_GPROTONU

//===========================================================================
#ifdef _SAPI_GPROTONA
struct protoent *getprotobyname(const char * name)
{
   int iRetv,i,iErrNo;
   FILE *psFile;

#ifdef SOCK_DEBUG
   if(_cTracing)
      SockDebug("getprotobyname: %s",name);
#endif
   if ((psFile = _OpenServiceFile("protocol")) == 0) {
      errno = WSANO_RECOVERY;
      return 0;
   }
   while(1) {
      if ((iRetv = _GetServiceEntry(psFile,&_sPe.p_name,&_sPe.p_aliases,
                                                      &_sPe.p_proto,0)) == 0) {
         iErrNo = WSANO_DATA;
         break;
      }
      else if (iRetv < 0) {
         iErrNo = WSAENOBUFS;
         break;
      }
      if(stricmp(_sPe.p_name,name) == 0) {
         fclose(psFile);
         return &_sPe;
      }
      for (i = 0; _sPe.p_aliases[i]; ++i) {
         if(stricmp(_sPe.p_aliases[i],name) == 0) {
            fclose(psFile);
            return &_sPe;
         }
      }
   }
   fclose(psFile);
   errno = iErrNo;
   return 0;
}
#endif   // _SAPI_GPROTONA

// Microsoft Windows Extension functions
#ifdef _SAPI_WSASTART
static int iWSAStartupCalled;
#ifdef __TURBOC__
#pragma argsused
#endif
int WSAStartup(WORD wVersionRequired, PWSADATA psWSAData)
{
   SOCKET arso[FD_SETSIZE];
   int i,iVersion;
   char sz[40];

#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("WSAStartup");
   }
#endif
   if ((iVersion = GetSocketsVersion()) < 0)
      return WSASYSNOTREADY;
   if (psWSAData) {
      memset(psWSAData,0,sizeof(WSADATA));
      for (i = 0;i < FD_SETSIZE;++i) {
         if ((arso[i] = GetSocket()) == INVALID_SOCKET)
            break;
      }
      psWSAData->iMaxSockets = i;
      for (--i; i >= 0;--i)
         ReleaseSocket(arso[i]);
      psWSAData->wVersion =
      psWSAData->wHighVersion = 0x101;
      psWSAData->iMaxUdpDg = 2048;  // keep fragmentation down
      sprintf(sz,"Datalight Sockets Build %04u",iVersion);
      strcpy(psWSAData->szDescription,sz);
   }
   ++iWSAStartupCalled;
   return 0;
}

int WSACleanup(void)
{
   SOCKET so;

#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("WSACleanup");
   }
#endif
   if (--iWSAStartupCalled <= 0) {
      // reset all active connections
      for (so = 0; so < FD_SETSIZE;++so) {
         if (_psScbs[so])
            AbortSocket(so + S_OFFSET);
      }
   }
   return 0;
}
#endif   // _SAPI_WSASTART

#ifdef _SAPI_WSASETER
void WSASetLastError(int iError)
{
   errno = iError;
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("WSASetLastError: %d",errno);
   }
#endif
}
#endif   // _SAPI_WSASETER

#ifdef _SAPI_WSAGETER
int WSAGetLastError(void)
{
#ifdef SOCK_DEBUG
   if(_cTracing) {
      SockDebug("WSASetLastError: return %d",errno);
   }
#endif
   return errno;
}
#endif   // _SAPI_WSAGETER

/* function needed for FD_ISSET macro */
#ifdef _SAPI_ISSET
int __WSAFDIsSet(SOCKET fd, fd_set *set)
{
   int i = set->fd_count;

#ifdef SOCK_DEBUG
   if (_cTracing)
      SockDebug("__WSAFDIsSet %u",fd);
#endif
   while (i--)
    if (set->fd_array[i] == fd)
       return 1;

   return 0;
}
#endif   // _SAPI_ISSET

#ifdef _SAPI_FDCLR
#ifndef FD_MACRO_INLINE
void __WSAFDClr(SOCKET fd, fd_set *set)
{
   u_int ui;

#ifdef SOCK_DEBUG
   if (_cTracing)
      SockDebug("__WSAFDClr %u",fd);
#endif
   for (ui = 0; ui < set->fd_count ; ui++) {
      if (set->fd_array[ui] == fd) {
         while (ui < set->fd_count-1) {
            set->fd_array[ui] = set->fd_array[ui + 1];
            ui++;
         }
         set->fd_count--;
         break;
      }
   }
}
#endif   // FD_MACRO_INLINE
#endif   // _SAPI_FDCLR

#ifdef _SAPI_FDSET
#ifndef FD_MACRO_INLINE
void __WSAFDSet(SOCKET fd, fd_set *set)
{
   u_int ui;

#ifdef SOCK_DEBUG
   if (_cTracing)
      SockDebug("__WSAFDSet %u",fd);
#endif
   for (ui = 0; ui < set->fd_count; ui++) {
      if (set->fd_array[ui] == (fd)) {
         break;
      }
   }
   if (ui == set->fd_count) {
      if (set->fd_count < FD_SETSIZE) {
         set->fd_array[ui] = (fd);
         set->fd_count++;
      }
   }
}
#endif   // FD_MACRO_INLINE
#endif   // _SAPI_FDSET

#ifdef _SAPI_MAPERROR
int _MapError(int iErrNum) // map CAPI to winsock errors
{
   WORD iSocketErr;

   switch (iErrNum) {
   case NO_ERR:               // 0    No error
      iSocketErr = WSABASEERR;
      break;
   case ERR_IN_USE:           // 1    A connection already exists
      iSocketErr = WSAEISCONN;
      break;
   case ERR_DOS:              // 2    A DOS error occured
      iSocketErr = WSAEBADF;
      break;
   case ERR_NO_MEM:           // 3    No memory to perform function
      iSocketErr = WSAENOBUFS;
      break;
   case ERR_NOT_NET_CONN:     // 4    Connection does not exist
      iSocketErr = WSAENOTCONN;
      break;
   case ERR_ILLEGAL_OP:       // 5    Protocol or mode not supported
      iSocketErr = WSAEOPNOTSUPP;
      break;
   case ERR_NO_HOST:          // 7    No host address specified
      iSocketErr = WSAEDESTADDRREQ;
      break;
   case ERR_TIMEOUT:          // 13   The function timed out
      iSocketErr = WSAETIMEDOUT;
      break;
   case ERR_HOST_UNKNOWN:     // 14   Unknown host has been specified
      iSocketErr = WSAHOST_NOT_FOUND;
      break;
   case ERR_BAD_ARG:          // 18   Bad arguments
      iSocketErr = WSAEINVAL;
      break;
   case ERR_EOF:              // 19  Connection closed by peer
   case ERR_RESET:            // 20  Connection reset by peer
      iSocketErr = WSAECONNRESET;
      break;
   case ERR_WOULD_BLOCK:      // 21   Operation would block
      iSocketErr = WSAEWOULDBLOCK;
      break;
   case ERR_UNBOUND:          // 22   Descriptor not yet assigned
      iSocketErr = WSAEADDRNOTAVAIL;
      break;
   case ERR_NO_SOCKET:        // 23   No descriptor is available
      iSocketErr = WSAEMFILE;
      break;
   case ERR_BAD_SYS_CALL:     // 24   Bad parameter in call
      iSocketErr = WSAEINVAL;
      break;
   case ERR_NOT_ESTAB:        // 26   Connection not yet established
      iSocketErr = WSAENOTCONN;
      break;
   case ERR_RE_ENTRY:         // 27   Kernel in use, try again later
      iSocketErr = WSAEINPROGRESS;
      break;
   case ERR_NETWORK:          // 28 Network error eg ICMP message
      iSocketErr = WSAENETRESET;
      break;
   default:
      iSocketErr = WSAENETDOWN;
   }
   return iSocketErr;
}
#endif   // _SAPI_MAPERROR

#ifdef _SAPI_GETNEXTP
u_short _GetNextPortNum(void)
{
   WORD wPort = 0;
   unsigned uSize = sizeof(WORD);

   GetKernelInformation(0,K_INF_LOCAL_PORT,0,&wPort,&uSize);
   return wPort;
}
#endif   // _SAPI_GETNEXTP

#ifdef _SAPI_GETREADC
int _GetReadCount(SOCKET s,SCB *psScb)
{
   int iVal;

   if (!(psScb->wFlags & SF_NONBLOCK)) {
      if (SetSocketOption(s,0,NET_OPT_NON_BLOCKING,1,sizeof(u_long)) < 0) {
         iVal = 0;
         errno = 0;
         return iVal;
      }
   }
   if (psScb->bType == SOCK_DGRAM)
      _CheckUdpOpen(s,psScb);
   if ((iVal = ReadSocket(s,0,0,0,NET_FLG_PEEK)) < 0)
      iVal = iNetErrNo == ERR_WOULD_BLOCK ? - 1 : 0;
   if (!(psScb->wFlags & SF_NONBLOCK))
      SetSocketOption(s,0,NET_OPT_NON_BLOCKING,0,sizeof(u_long));
   errno = 0;
   return iVal;
}
#endif   // _SAPI_GETREADC

#ifdef _SAPI_CHECKUDP
int _CheckUdpOpen(SOCKET s,SCB *psScb)
{
   NET_ADDR sNetAddr;

   if (!(psScb->wFlags & SF_CONNECTED)) {
      sNetAddr.dwRemoteHost = INADDR_NONE;
      sNetAddr.wRemotePort  = 0;
      sNetAddr.bProtocol = DATA_GRAM;
      sNetAddr.wLocalPort = psScb->sIsockLocal.wPort;
      if (ConnectSocket(s,sNetAddr.bProtocol,&sNetAddr) < 0) {
         errno = _MapError(iNetErrNo);
         return -1;
      }
      psScb->wFlags |= (SF_BOUND | SF_CONNECTED);
   }
   return 0;
}
#endif   // _SAPI_CHECKUDP


