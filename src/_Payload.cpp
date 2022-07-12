#include <iostream>

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

int main() {

	unsigned long sequential, price, coil, crc_7 = 0;
	char payload[16], key[]= "...";

	std::cin.read(payload, sizeof(payload));

	for (char x = 0; x < sizeof(payload) - 1; x++) {

		calc_crc_7(&crc_7, payload[x] ^= key[x]);
	}

	if (payload[sizeof(payload) - 1] == crc_7) {

		sequential = payload[0] << 21 | payload[1] << 14 | payload[2] << 7 | payload[3];
		price = payload[4] << 14 | payload[5] << 7 | payload[6];
		coil = payload[7] << 14 | payload[8] << 7 | payload[9];

		std::cout << "Sequential: " << sequential << ", Price: " << price << ", Coil: " << coil << "\n";

	} else std::cerr << "corrupted\n";

	return 0;
}
