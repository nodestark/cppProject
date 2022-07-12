#include <iostream>
#include <fstream>
#include <string.h>

char calc_crc_7(unsigned long *pCrc, char uData) {

  char oldData = uData;

  int iBit;

  for (iBit = 0; iBit < 8; iBit++, uData >>= 1) {

    if ((uData ^ *pCrc) & 0x01) {

      *pCrc >>= 1;
      *pCrc ^= 0x48;

    } else
      *pCrc >>= 1;
  }

  return oldData;
}

int main(int argc, char *argv[]) {

	// ./payload_ -s 123 -c 11 -p 150

	char payload[16], key[]= "...";

	std::ifstream devrandom("/dev/random");

	devrandom.read( payload, sizeof( payload ) );

	devrandom.close();

	unsigned long sequential = 0, price= 0, coil= 0, crc_7= 0;

	for( int x= 1; x < argc; x++ ){

		if( strcmp( "-s", argv[x] ) == 0 )
			sequential= atoi( argv[x + 1] );

		if( strcmp( "-c", argv[x] ) == 0 )
			coil= atoi( argv[x + 1] );

		if( strcmp( "-p", argv[x] ) == 0 )
			price= atoi( argv[x + 1] );
	}

	payload[0] = calc_crc_7( &crc_7, (sequential >> 21) & 0x7f) ^ key[ 0 ];
	payload[1] = calc_crc_7( &crc_7, (sequential >> 14) & 0x7f) ^ key[ 1 ];
	payload[2] = calc_crc_7( &crc_7, (sequential >> 7) & 0x7f) ^ key[ 2 ];
	payload[3] = calc_crc_7( &crc_7, sequential & 0x7f) ^ key[ 3 ];

	payload[4] = calc_crc_7( &crc_7, (price >> 14) & 0x7f) ^ key[ 4 ];
	payload[5] = calc_crc_7( &crc_7, (price >> 7) & 0x7f) ^ key[ 5 ];
	payload[6] = calc_crc_7( &crc_7, price & 0x7f) ^ key[ 6 ];

	payload[7] = calc_crc_7( &crc_7, (coil >> 14) & 0x7f) ^ key[ 7 ];
	payload[8] = calc_crc_7( &crc_7, (coil >> 7) & 0x7f) ^ key[ 8 ];
	payload[9] = calc_crc_7( &crc_7, coil & 0x7f) ^ key[ 9 ];

	payload[10] = calc_crc_7( &crc_7, payload[10] ) ^ key[ 10 ];
	payload[11] = calc_crc_7( &crc_7, payload[11] ) ^ key[ 11 ];
	payload[12] = calc_crc_7( &crc_7, payload[12] ) ^ key[ 12 ];
	payload[13] = calc_crc_7( &crc_7, payload[13] ) ^ key[ 13 ];
	payload[14] = calc_crc_7( &crc_7, payload[14] ) ^ key[ 14 ];

	payload[15] = crc_7;

	std::cout.write( payload, sizeof(payload) );

	return 0;
}
