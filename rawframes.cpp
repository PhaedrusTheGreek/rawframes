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
 * rawframes.cpp
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <features.h>    /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <linux/if_ether.h>   /* the L2 protocols */
#endif

#include <arpa/inet.h>
#include <net/if.h>

#include "frame.h"
#include "ip.h"
#include "rawframes.h"

#include "argtable.h"

int main (int argc, char **argv) {

printf("\n  RawFrames version %s Copyright (C) 2003 Jay Greenberg\n\n  This Program comes with ABSOLUTELY NO WARRANTY\n  This is free software, and you are welcome\n  to redistribute it under certain conditions\n  See http://www.gnu.org/copyleft/gpl.html for details.\n\n  Send comments & bugs to %s\n\n", PACKAGE_VERSION, PACKAGE_BUGREPORT);

  int status;
  char payload[1500];  // The running version of the frame
  int payload_length=0;  // the length of such

  memset (payload, 0, 1500);       /* zero out the buffer */

  
  if (!readargs(argc, argv)) { exit(1); }
  (args::verbosity > 4) && printf("Done Reading CLI Arguments.\n");
  if (!checkargs()) { exit(1); }

  (args::verbosity > 1) && printf("Initializing...\n");
  (args::verbosity > 2) && printf("payload length:  %d\n", payload_length);

  //Layer 3 Encap

  if (strcmp(args::iso_layer3, "ipv4")==0) { 

   if ((strcmp(args::iso_layer2, "802.3")==0)||(strcmp(args::iso_layer2, "LLC")==0)) {
    	  fprintf(stderr, "Frame type %s not compatible with ipv4.  Use ETH_II or LLC_SNAP. \n", 
		args::iso_layer2);
    	  exit(1);
   }

   CIPPacket objIP;

   objIP.set_version(args::ip_version);
   objIP.set_tos(args::ip_tos);
   objIP.set_id(args::ip_version);
   objIP.set_frag_off(args::ip_frag_off);
   objIP.set_ttl(args::ip_ttl);
   objIP.set_protocol(args::ip_protocol);
   objIP.set_saddr(inet_addr (args::ip_saddr));
   objIP.set_daddr(inet_addr (args::ip_daddr));
  
   objIP.set_check();
   objIP.encapsulate(payload, &payload_length);

   (args::verbosity > 1) && printf("Added IP Header...\n");
   (args::verbosity > 2) && printf("payload length:  %d\n", payload_length);

  } else if (strcmp(args::iso_layer3, "ipv6")==0) {
    //todo
  }
   
  if ((strcmp(args::iso_layer2, "LLC")==0) ||(strcmp(args::iso_layer2, "LLC_SNAP")==0)){
  	// If this is an 802.2 (LLC) frame, then let's put LLC headers in...

  	if (strcmp(args::iso_layer2, "LLC_SNAP")==0){
  	// If this is an LLC/SNAP frame, then let's put SNAP headers in...

		CSNAP_Header objSNAP;

		objSNAP.set_oui(args::snap_oui);
		objSNAP.set_etype(args::snap_etype);

   		objSNAP.encapsulate(payload, &payload_length);

	   	(args::verbosity > 1) && printf("Added SNAP Header...\n");
   		(args::verbosity > 2) && printf("payload length:  %d\n", payload_length);

  	}

 	CLLC_Header objLLC;

	objLLC.set_SSAP(args::llc_ssap);
	objLLC.set_DSAP(args::llc_dsap);
	objLLC.set_Control(args::llc_cl);

   	objLLC.encapsulate(payload, &payload_length);

   	(args::verbosity > 1) && printf("Added LLC Header...\n");
   	(args::verbosity > 2) && printf("payload length:  %d\n", payload_length);
  
  }

  // Now that we're down to the frame level, we have to do a type/len determination
  int typelen = strcmp(args::iso_layer2, "ETH_II");  // 0 = Type, !0 = Len 
  char ethertype[7]; strcpy(ethertype, args::mac_etype);

  if (args::dot1q_tag) {

   	(args::verbosity > 4) && printf("Creating Dot1q Tag...\n");
 	CDot1q objDot1q;

	// Tag Control Information
	objDot1q.set_tci(args::dot1q_priority,	//802.1p
			 args::dot1q_cfi,	//Canonical Format ID
			 args::dot1q_vlan);	//VLAN ID

	objDot1q.set_ptid(ethertype, payload_length, typelen); typelen=0;

       	strcpy(ethertype,"0x8100");	//for the ethernet_II type frame (8100=802.1q)

   	objDot1q.encapsulate(payload, &payload_length);

   	(args::verbosity > 1) && printf("Added Dot1q Tag...\n");
   	(args::verbosity > 2) && printf("payload length:  %d\n", payload_length);

 }

 CFrame objFrame;
 objFrame.set_ethertype(ethertype, payload_length, typelen);

 if (!objFrame.set_dmac(args::mac_dmac)){
   	fprintf(stderr, "%s: Invalid Hardware Address\n", args::mac_dmac);
   	exit(1);
 }

 if (!objFrame.set_smac(args::mac_smac)){
  	fprintf(stderr, "%s: Invalid Hardware Address\n", args::mac_smac);
 	exit(1);
 }

 objFrame.set_ifindex(args::iface);

 //Bling Bling
 status = objFrame.Transmit(payload,payload_length,args::count);

 (args::verbosity > 1) && printf("Done Transmitting Frames...\n");



 exit(status);


}

int readargs (int argc, char **argv) {

 arg_rec argtable[] =
      {
      {"-i ", "<ifname>",  arg_str, args::iface, NULL, "\t\t\t Interface to Transmit From"},
      {"-mac_smac ", "<hh:hh:hh:hh:hh:hh>",  arg_str, args::mac_smac, NULL, " Source Mac Address"},
      {"-mac_dmac ", "<hh:hh:hh:hh:hh:hh>",  arg_str, args::mac_dmac, NULL, " Destination Mac Address"},
      {"-iso_layer2 ", "<ETH_II|802.3|LLC|LLC_SNAP>",  arg_str, args::iso_layer2, "ETH_II", "\t Frame Type"},
      {"-mac_etype ", "<0xhhhh>",  arg_str, args::mac_etype, "0x0800", "\t\t 802.3 Type"},
      {"-mac_len ", "<bytes>",  arg_int, &args::mac_len, "0", "\t\t Frame Size "},
      {"-dot1q_tag ", "<0|1>",  arg_int, &args::dot1q_tag, "0", "\t\t Add 802.1q Tag "},
      {"-dot1q_priority ", "<0-7>",  arg_int, &args::dot1q_priority, "0", "\t 802.1q Priority (802.1p)"},
      {"-dot1q_cfi ", "<0|1>",  arg_int, &args::dot1q_cfi, "0", "\t\t 802.1q Canonical Format Identifier "},
      {"-dot1q_vlan ", "<0-4095>",  arg_int, &args::dot1q_vlan, "0", "\t\t 802.1q Vlan ID "},
      {"-llc_ssap ", "<0xhh>",  arg_str, &args::llc_ssap, "0xAA", "\t\t LLC SSAP "},
      {"-llc_dsap ", "<0xhh>",  arg_str, &args::llc_dsap, "0xAA", "\t\t LLC DSAP "},
      {"-llc_cl ", "<0xhh>",  arg_str, &args::llc_cl, "0x03", "\t\t LLC Control "},
      {"-snap_oui ", "<0xhhhhhh>",  arg_str, &args::snap_oui, "0x000000", "\t\t SNAP OUI "},
      {"-snap_etype ", "<0xhhhh>",  arg_str, &args::snap_etype, "0x0800", "\t\t SNAP Type "},
      {"-iso_layer3 ", "<ipv4|none>",  arg_str, args::iso_layer3, "none", "\t Packet Type"},
      {"-ip_version ", "<4|6>",  arg_int, &args::ip_version, "4", "\t\t IP Version"},
      {"-ip_tos ", "<tos>",  arg_int, &args::ip_tos, "0", "\t\t IP Type of Service field / DSCP"},
      {"-ip_id ", "<id>",  arg_int, &args::ip_id, "1", "\t\t\t IP ID"},
      {"-ip_frag_off ", "<offset>",  arg_int, &args::ip_frag_off, "0", "\t IP Fragmentation Offset"},
      {"-ip_ttl ", "<ttl>",  arg_int, &args::ip_ttl, "255", "\t\t IP Time To Live"},
      {"-ip_protocol ", "<protocol>",  arg_int, &args::ip_protocol, "114", "\t IP Protocol Number"},
      {"-ip_saddr ", "<ipaddr>",  arg_str, &args::ip_saddr, "0.0.0.0", "\t\t IP Source Address"},
      {"-ip_saddr ", "<ipaddr>",  arg_str, &args::ip_daddr, "0.0.0.0", "\t\t IP Destination Address"},
      {"-count ", "<number>",  arg_int, &args::count, "1", "\t\t Number of Times to Send the Frame"},
      {"-v ", "<1-5>",  arg_int, &args::verbosity, "1", "\t\t\t Verbose: Use -v 0 to Supress Output (df=1)"}
      };
 
  const size_t narg = (sizeof(argtable)/sizeof(arg_rec));

/*-- process the command line args --*/
  if (argc==1)
     {
     /*-- display program usage and exit. --*/
     printf("Usage: %s %s\n", argv[0], arg_syntax(argtable,narg));
     printf("%s\n",arg_glossary(argtable,narg,"   "));
     return 0;
     }
  else
     {
     /*-- scan command line arguments from argv[] --*/
     char cmdline[200], errmsg[200], errmark[200];
     if (!arg_scanargv(argc,argv,argtable,narg,cmdline,errmsg,errmark))
        {
        /*-- arg error occurred, print error message and exit --*/
        printf("ERROR: %s\n", cmdline);
        printf("       %s %s\n", errmark, errmsg);
        return 0;
        }
      }

 // only get here if the arguments were scanned successfully
 return 1;

}

int checkargs() {
 return 1;
}
