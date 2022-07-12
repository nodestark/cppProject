#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {

	int fd_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	char buffer[65535];
	struct sockaddr saddr;

	int saddr_len = sizeof(saddr);

	while( true ){

		int buflen = recvfrom(fd_socket, buffer, sizeof(buffer), 0, &saddr, (socklen_t*) &saddr_len);

		struct EthernetHeader {

			unsigned char h_source[6];
			unsigned char h_dest[6];

			uint16_t h_proto;
		};
		EthernetHeader *eth = (EthernetHeader*) &buffer[0]; // 14

		// long == 8bytes
		// int == 4bytes
		// short == 2bytes
		// char == 1byte

		struct IpHeader {

			unsigned char version_ihl;
			unsigned char dscp_ecn;
			unsigned short length;
			unsigned short id;
			unsigned short flags;
			unsigned char ttl;
			unsigned char protocol;
			unsigned short chk;
			unsigned char src[4];
			unsigned char dst[4];
		};
		IpHeader *ip = (IpHeader*) &buffer[ sizeof(EthernetHeader) ]; // 20

		if( ip->protocol == 0x06 /*tcp*/ ){

			struct TcpHeader {

				unsigned short srcPort;
				unsigned short dstPort;

				unsigned int sequential;
				unsigned int ack_seq;

				unsigned char doff;
				unsigned char flag;

				unsigned short wsize;
				unsigned short chksum;

				unsigned short urgent;
			};
			TcpHeader *tcpl = (TcpHeader*) &buffer[ sizeof(EthernetHeader) + sizeof(IpHeader) ];

			if( (((tcpl->dstPort << 8) & 0xff00) | ((tcpl->dstPort >> 8) & 0x00ff)) != 8088 ) continue;

			printf("\nEthernet Header\n");
			printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
			printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
			printf("\t|-Protocol : %d\n", eth->h_proto);

			printf("\nIP Header\n");
			printf("\t|-Version : %d\n", (ip->version_ihl >> 4) & 0xf );
			printf("\t|-Internet Header Length : %d Bytes\n", ip->version_ihl & 0xf );
			printf("\t|-Total Length : %d Bytes\n", ntohs(ip->length) );
			printf("\t|-Identification : %d\n", ntohs(ip->id ) );
			printf("\t|-Time To Live : %d\n", ip->ttl);
			printf("\t|-Protocol : %d\n", ip->protocol );
			printf("\t|-Source Address : %d.%d.%d.%d\n", ip->src[0], ip->src[1], ip->src[2], ip->src[3]);
			printf("\t|-Destination Address : %d.%d.%d.%d\n", ip->dst[0], ip->dst[1], ip->dst[2], ip->dst[3]);

			printf("\nTCP Header\n");
			printf("\t|-Source Port : %d\n", ((tcpl->srcPort << 8) & 0xff00) | ((tcpl->srcPort >> 8) & 0x00ff) );
			printf("\t|-Destination Port : %d\n", ((tcpl->dstPort << 8) & 0xff00) | ((tcpl->dstPort >> 8) & 0x00ff) );
		   	printf("\t|-Sequence Number      : %u\n", ntohl(tcpl->sequential) );
		   	printf("\t|-Acknowledge Number   : %u\n", ntohl(tcpl-> ack_seq) );
		   	printf("\t|----------Flags-----------\n");
		   	printf("\t\t|-Urgent Flag          : %d\n", tcpl->flag >> 5 & 1 );
		   	printf("\t\t|-Acknowledgement Flag : %d\n", tcpl->flag >> 4 & 1 );
		   	printf("\t\t|-Push Flag            : %d\n", tcpl->flag >> 3 & 1 );
		   	printf("\t\t|-Reset Flag           : %d\n", tcpl->flag >> 2 & 1 );
		   	printf("\t\t|-Synchronise Flag     : %d\n", tcpl->flag >> 1 & 1 );
		    printf("\t\t|-Finish Flag          : %d\n", tcpl->flag >> 0 & 1 );
		    printf("\t|-Window size          : %d\n", ntohs(tcpl->wsize));
		    printf("\t|-Checksum             : %d\n", ntohs(tcpl->chksum));
		    printf("\t|-Urgent Pointer       : %d\n", tcpl->urgent);

			printf("\t|-Urgent Pointer       : %b\n", tcpl->flag );

//			break;
		}
	}

	std::cout << "end" << std::endl;

	return 0;
}
