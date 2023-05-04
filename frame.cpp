/*
                                                                                                              
 RAWFRAMES 
 Written by Jay Greenberg, CCIE #11021 in May 2003
 Send comments / bugs to me at jay@greenberg.on.ca
 Visit My homepage at http://www.greenberg.on.ca/
                                                                                                              
 Copyright (C) 2003 Jay Greenberg
                                                                                                              
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
                                                                                                              
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
                                                                                                              
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
                                                                                                              
 */


/* frame.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <features.h>    /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* the L2 protocols */
#endif

#include "frame.h"

#include <net/if.h>

CFrame::CFrame() {
	// printf("Creating frame...");
}

CFrame::~CFrame() {
	// printf("Destroying frame...");
}


int CFrame::set_dmac (char *dmac) {
 return mac_aton(dmac, (char*)_ether_header.h_dest);
}

int CFrame::set_smac (char *smac) {
 return mac_aton(smac, (char*)_ether_header.h_source);
}

void CFrame::set_ethertype(char *ethertype, int etherlength, int typelen) {
 (args::verbosity > 4) &&  printf("typelen: %d\n", typelen);
 typelen 
   ? ( _ether_header.h_proto = htons(etherlength) ) // !0
   : ( _ether_header.h_proto = htons(strtoul(ethertype,NULL,16)) ); // 0
}

void CFrame::set_ifindex (char *iface) {
 _ifindex = if_nametoindex(iface);
}

int CFrame::Transmit(char *packet_buffer, int packet_length, int count) {

  char* msg;
  msg = _data;

  int i;
  int msg_size;
  int packet_socket;
  struct sockaddr_ll ll_source_addr;
  struct sockaddr_ll ll_dest_addr;
 
  (args::verbosity > 4) &&  printf("Opening Socket...\n");
  if ( (packet_socket = socket(PF_PACKET, SOCK_RAW, _ether_header.h_proto)) == -1 ) {
    perror("rp_send: main(): socket()");
    exit (1);
  }

  /* source address for bind */
  (args::verbosity > 4) &&  printf("Setting SADDR for BIND...\n");
  ll_source_addr.sll_family = AF_PACKET;
  ll_source_addr.sll_protocol = _ether_header.h_proto;
  ll_source_addr.sll_ifindex = _ifindex;
  ll_source_addr.sll_halen = ETH_ALEN;
  memcpy (ll_source_addr.sll_addr, _ether_header.h_source, ETH_ALEN);

  /* destination address for sendto */
  (args::verbosity > 4) &&  printf("Setting DADDR for SendTo...\n");
  ll_dest_addr.sll_family = AF_PACKET;
  ll_dest_addr.sll_protocol = _ether_header.h_proto;
  ll_dest_addr.sll_ifindex = _ifindex;
  ll_dest_addr.sll_halen = ETH_ALEN;
  memcpy (ll_dest_addr.sll_addr, _ether_header.h_dest, ETH_ALEN);

  (args::verbosity > 4) &&  printf("Binding...\n");
  if (bind (packet_socket,
            (struct sockaddr*)&ll_source_addr,
            sizeof(ll_source_addr)) == -1 ) {
    perror("rp_send: main(): bind()");
    exit (1);
  }

  /* msg */
  msg_size=0;

  // note msg size is probably 0 here, but + msg_size for future...
  (args::verbosity > 4) &&  printf("Building Frame...\n"); 
  memcpy(msg + msg_size, &_ether_header, sizeof(_ether_header)); 
  msg_size += sizeof(_ether_header);

  memcpy(msg + msg_size, packet_buffer, packet_length);
  msg_size += packet_length;

  // We do this here because we need to know the current data size
  /*
  (args::verbosity > 4) &&  printf("Padding Length...\n"); 
  char data[(args::mac_len - msg_size)];
  for (i = 0; i < (args::mac_len - msg_size); i++) {
    if (i >= 1500) { continue; } 
    data[i] = i;
  }

  (args::verbosity > 4) &&  printf("Building Frame Again...\n"); 
  memcpy (msg + msg_size, data, sizeof(data));
  msg_size += sizeof(data);
  */

  for (i = 0; i < count; i++) {

    (args::verbosity > 3) &&  printf("Preparing to send frame %d\n", i+1); 
    if ( sendto(packet_socket, msg, msg_size, 0,
                (struct sockaddr*)&ll_dest_addr,
                sizeof(ll_dest_addr)) == -1 ) {
      perror("rp_send: main(): sendto()");
      exit (1);
    }

    (args::verbosity > 0) &&  printf("Sent Frame %d\n", i+1); 

  } // for i

  (args::verbosity > 4) &&  printf("Closing Socket...\n"); 
  close (packet_socket);

  return 0;

}
// LLC

                                                                                
 CLLC_Header::CLLC_Header(){
        llc_header = (struct LLChdr *) LLC;
        memset (LLC, 0, sizeof(struct LLChdr));       /* zero out the buffer */
 }
                                                                                
 CLLC_Header::~CLLC_Header(){
                                                                                
 }


 void CLLC_Header::set_SSAP(char *SSAP){
  (args::verbosity > 4) &&  printf("Setting SSAP... %s\n", SSAP); 
  llc_header->SSAP=strtoul(SSAP,NULL,16);
 }
                                                                                
 void CLLC_Header::set_DSAP(char *DSAP){
  (args::verbosity > 4) &&  printf("Setting DSAP...%s\n", DSAP); 
  llc_header->DSAP=strtoul(DSAP,NULL,16);
 }
                                                                                
 void CLLC_Header::set_Control(char *Control){
  (args::verbosity > 4) &&  printf("Setting Control...%s\n", Control); 
  llc_header->Control=strtoul(Control,NULL,16);
 }

void CLLC_Header::encapsulate(char *payload, int *payload_length) {
                                                                                
  // requires a *pointer* to the current payload and the length, so that
  // *bota*h can be updated by this function.
                                                                                
  // Set Lengths
  HeaderLength=sizeof(struct LLChdr);
  TotalLength= (HeaderLength + *payload_length);

  (args::verbosity > 4) &&  printf("LLC Header Length... %d\n", HeaderLength);
  (args::verbosity > 4) &&  printf("LLC Total Length... %d\n", TotalLength);
                                                                                
  // Encapsulate the payload
  memcpy(LLC+HeaderLength,payload,*payload_length);

  // Set the new payload
  memcpy(payload,LLC,TotalLength);
                                                                                
  // Set the new length
  *payload_length = TotalLength;
                                                                                
}

// SNAP

                                                                                
 CSNAP_Header::CSNAP_Header(){
        snap_header = (struct SNAPhdr *) SNAP;
        memset (SNAP, 0, sizeof(struct SNAPhdr));       /* zero out the buffer */
 }
                                                                                
 CSNAP_Header::~CSNAP_Header(){
                                                                                
 }


 void CSNAP_Header::set_oui(char *oui){
  (args::verbosity > 4) &&  printf("Setting OUI... %s\n", oui); 
  //memcpy(&snap_header->oui, oui, sizeof(snap_header->oui));
  // snap_header->oui, oui, sizeof(snap_header->oui));
 }
                                                                                
 void CSNAP_Header::set_etype(char *etype){
  (args::verbosity > 4) &&  printf("Setting ETYPE...%s\n", etype); 
  snap_header->etype=htons(strtoul(etype,NULL,16));
 }
                                                                                
void CSNAP_Header::encapsulate(char *payload, int *payload_length) {
                                                                                
  // requires a *pointer* to the current payload and the length, so that
  // *bota*h can be updated by this function.
                                                                                
  // Set Lengths
  HeaderLength=sizeof(struct SNAPhdr);
  TotalLength= (HeaderLength + *payload_length);

  (args::verbosity > 4) &&  printf("SNAP Header Length... %d\n", HeaderLength);
  (args::verbosity > 4) &&  printf("SNAP Total Length... %d\n", TotalLength);
                                                                                
  // Encapsulate the payload
  memcpy(SNAP+HeaderLength,payload,*payload_length);

  // Set the new payload
  memcpy(payload,SNAP,TotalLength);
                                                                                
  // Set the new length
  *payload_length = TotalLength;
                                                                                
}

// .1q

                                                                                
 CDot1q::CDot1q(){
     _dot1q_header = (struct dot1qhdr *) DOT1Q;
     memset (DOT1Q, 0, sizeof(struct dot1qhdr));       /* zero out the buffer */
 }
                                                                                
 CDot1q::~CDot1q(){
                                                                                
 }

 // Protocol Type ID
 void CDot1q::set_ptid(char *ethertype, int etherlength, int typelen) {
  (args::verbosity > 4) &&  printf("dot1q typelen: %d\n", typelen);
  typelen 
    ? ( _dot1q_header->h_ptid = htons(etherlength) ) // !0
    : ( _dot1q_header->h_ptid = htons(strtoul(ethertype,NULL,16)) ); // 0
 }

 // Tag Control Information
 void CDot1q::set_tci(int priority,  int cfi, int vid){
  (args::verbosity > 4) &&  printf("Setting dot1q Priority... %d\n", priority); 
  (args::verbosity > 4) &&  printf("Setting dot1q CFI... %d\n", cfi); 
  (args::verbosity > 4) &&  printf("Setting dot1q VID (Vlan)... %d\n", vid); 
  _dot1q_header->h_tci = htons(vid + (priority << 13 ) + (cfi << 12));
 }
                                                                                
 void CDot1q::encapsulate(char *payload, int *payload_length) {
                                                                                
  // requires a *pointer* to the current payload and the length, so that
  // *bota*h can be updated by this function.
                                                                                
  // Set Lengths
  HeaderLength=sizeof(struct dot1qhdr);
  TotalLength= (HeaderLength + *payload_length);

  (args::verbosity > 4) &&  printf("Dot1q Header Length... %d\n", HeaderLength);
  (args::verbosity > 4) &&  printf("Dot1q Total Length... %d\n", TotalLength);
                                                                                
  // Encapsulate the payload
  memcpy(DOT1Q+HeaderLength,payload,*payload_length);

  // Set the new payload
  memcpy(payload,DOT1Q,TotalLength);
                                                                                
  // Set the new length
  *payload_length = TotalLength;
                                                                                
 }


/*
===============================================================
  mac_aton()
===============================================================
*/
int mac_aton(char *amac, char *nmac)
{
  char c;
  int i;
  unsigned int val;

  i = 0;
  while ((*amac != '\0') && (i < ETH_ALEN)) {
    val = 0;
    c = *amac++;
    if (c >= '0' && c <= '9') {
      val = c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
      val = c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
      val = c - 'A' + 10;
    }
    else {
      errno = EINVAL;
      return 0;
    }
    val <<= 4;

    c = *amac;
    if (c >= '0' && c <= '9') {
      val |= c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
      val |= c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
      val |= c - 'A' + 10;
    }
    else if (c == ':' || c == '\0') {
      val >>= 4;
    }
    else {
      errno = EINVAL;
      return 0;
    }
    if (c != 0) {
      amac++;
    }
    *nmac++ = val & 0xff;
    i++;

    /* We might get a semicolon here - not required. */
    if (*amac == ':') {
      amac++;
    }
  }
  return 1;
}



