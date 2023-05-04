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


/* frame.h			*/
/* Class Def the frame object   */

//# include <rtl.h>

#define PACKET_SIZE 1500
#define MS 1000000              /* one ms in ns */
#define PERIOD (2*MS)           /* send period; recv period is half of that */

namespace args {
 extern int verbosity;
 extern int mac_len;
}

struct dot1qhdr // as an add-on to the ethhdr
{
        unsigned short  h_tci;                  /* Tag Control Info	*/
        unsigned short  h_ptid;                 /* packet type ID field */
} ;


struct LLChdr {

 	char SSAP; //  Source Service Access Point;
 	char DSAP; //  Dest SAP
 	char Control;   //  

};

class CLLC_Header {

	private:
	
		struct LLChdr *llc_header;

	public:

		 CLLC_Header();
 		 ~CLLC_Header();

		char LLC[1500];

		int HeaderLength;;
 		int TotalLength;

 		void set_SSAP(char *SSAP);
 		void set_DSAP(char *DSAP);
 		void set_Control(char *Control);

		void encapsulate(char *payload, int *payload_length); // encapsulates

} ;

struct SNAPhdr {

    unsigned char oui[3];
    unsigned short etype __attribute__ ((packed));

};

class CSNAP_Header {

	private:
	
		struct SNAPhdr *snap_header;

	public:

		 CSNAP_Header();
 		 ~CSNAP_Header();

		char SNAP[1500];

		int HeaderLength;;
 		int TotalLength;

 		void set_oui(char *oui);
 		void set_etype(char *etype);

		void encapsulate(char *payload, int *payload_length); // encapsulates

} ;


class CFrame {

	private:
		char _data[PACKET_SIZE];
		struct ethhdr _ether_header; 
		int _ifindex;     	// interface index
		
	public:
		int  set_dmac(char *dmac);		
		int  set_smac(char *smac);		
		void set_ethertype(char *ethertype, int etherlength, int typelen);
		void set_ifindex(char *iface);		

		int  Transmit(char *packet_buffer, int packet_length, int count);		

		CFrame();  // constructor
		~CFrame(); // de-constructor

};

class CDot1q { 

	private: 
		struct dot1qhdr	*_dot1q_header;

	public:
		CDot1q();
		~CDot1q();

		char DOT1Q[ETH_FRAME_LEN];

                int HeaderLength;;
                int TotalLength;

		void set_ptid(char *ethertype, int etherlength, int typelen); // Protocol Type
		void set_tci(int priority, int cfi, int vid); // Tag Control	

		void encapsulate(char *payload, int *payload_length); 

};

int mac_aton(char *amac, char *nmac);
