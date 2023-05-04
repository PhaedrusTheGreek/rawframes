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


/*
 * rawframes.h
 */

int main (int argc, char **argv);
int send_frame (CFrame Frame, int iface);
int readargs (int argc, char **argv);
int checkargs ();

// Argument Variables

namespace args {

 //Ethernet
 static char mac_smac[50];
 static char mac_dmac[50];
 static char mac_etype[10];
 int mac_len; // available extern

 //IPv4
 static int ip_version;
 static int ip_tos;
 static int ip_id;
 static int ip_frag_off;
 static int ip_ttl;
 static int ip_protocol;
 static char ip_saddr[50];
 static char ip_daddr[50];

 //802.1q
 static int dot1q_tag;
 static int dot1q_cfi;
 static int dot1q_priority;
 static int dot1q_vlan;

 //LLC
 static char llc_ssap[10];
 static char llc_dsap[10];
 static char llc_cl[10];

 //SNAP
 static char snap_oui[10];
 static char snap_etype[10];

 //Misc
 static char iface[10];
 static int count;
 int verbosity; // available extern
 
 static char iso_layer2[10];
 static char iso_layer3[10];
 static char iso_layer4[10];

}


