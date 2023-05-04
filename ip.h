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

 *
 ip.h
 *

*/

 struct iphdr {
        unsigned char ihl:4, version:4;
        unsigned char tos;
        unsigned short int tot_len;
        unsigned short int id;
        unsigned short int frag_off;
        unsigned char ttl;
        unsigned char protocol;
        unsigned short int check;
        unsigned int saddr;
        unsigned int daddr;
 }; /* 20 bytes */



class CIPPacket {

private:

 struct iphdr *ip_packet_header;

public:

 CIPPacket();
 ~CIPPacket();

 char Packet[1500];
 int HeaderLength;;
 int TotalLength;

 void set_version(int version);
 void set_tos(int tos);
 void set_id(int id);
 void set_frag_off(int frag_off);
 void set_ttl(int ttl);
 void set_protocol(int protocol);
 void set_check();
 void set_saddr(int saddr);
 void set_daddr(int daddr);
 void set_ihl(int ihl);
 void set_tot_len(int tot_len);

 void encapsulate(char *payload, int *payload_length); // encapsulates 

private:

 unsigned short csum (unsigned short *buf, int nwords); // calc header checksum
 
};
