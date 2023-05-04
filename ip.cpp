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
                                                                                                              
 * ip.cpp
 */

#include <string.h>
#include <netinet/in.h>

#include "ip.h"

 CIPPacket::CIPPacket(){
	ip_packet_header = (struct iphdr *) Packet; 
	memset (Packet, 0, sizeof(struct iphdr));	/* zero out the buffer */
 }

 CIPPacket::~CIPPacket(){
	
 }

 void CIPPacket::set_version(int version){
  ip_packet_header->version=version;
 }

 void CIPPacket::set_tos(int tos){
  ip_packet_header->tos=tos; 
 }	 

 void CIPPacket::set_id(int id){
  ip_packet_header->id=htons(id);
 }	 

 void CIPPacket::set_frag_off(int frag_off){
  ip_packet_header->frag_off=htonl(frag_off);
 }	 

 void CIPPacket::set_ttl(int ttl){
  ip_packet_header->ttl=ttl;
 }	 

 void CIPPacket::set_protocol(int protocol){
  ip_packet_header->protocol=protocol;
 }	 

 void CIPPacket::set_check(){
  ip_packet_header->check= csum ((unsigned short *)Packet, ip_packet_header->tot_len >> 1);
 }	 

 void CIPPacket::set_saddr(int saddr){
	 ip_packet_header->saddr=saddr;
 }	 

 void CIPPacket::set_daddr(int daddr){
	 ip_packet_header->daddr=daddr;
 }	 


 unsigned short CIPPacket::csum (unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
 }



/*
 void CIPPacket::set_options(int options){
	 ip_packet_header._options=options;
 }	 

 void CIPPacket::set_payload(int payload){
	 ip_packet_header._payload=payload;
 }	 
*/

 void CIPPacket::set_ihl(int ihl){
	 ip_packet_header->ihl= (ihl >> 2);
 }	 

 void CIPPacket::set_tot_len(int tot_len){
  ip_packet_header->tot_len=htons(tot_len);
 }	 




void CIPPacket::encapsulate(char *payload, int *payload_length) {

  // requires a *pointer* to the current payload and the length, so that 
  // *bota*h can be updated by this function.

  // Set Lengths
  HeaderLength=sizeof(struct iphdr);
  TotalLength= (HeaderLength + *payload_length);

  set_ihl(HeaderLength);
  set_tot_len(TotalLength);

  // checksum
  ip_packet_header->check= csum ((unsigned short *)Packet, TotalLength >> 1);

  // Encapsulate the payload
  memcpy(Packet+HeaderLength,payload,*payload_length);

  // Set the new payload
  memcpy(payload,Packet,TotalLength);
 
  // Set the new length
  *payload_length = TotalLength;

}




