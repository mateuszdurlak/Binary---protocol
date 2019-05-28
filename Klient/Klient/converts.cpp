//===CONVERTERS===//
#include "converts.h"

int z2na10(std::string z2na10_input) {
	int wynik10 = 0;
	for (int i = z2na10_input.size() - 1; i >= 0; i--) {
		wynik10 = wynik10 + (z2na10_input[i] - '0') * pow(2, z2na10_input.size() - 1 - i);
	}
	return wynik10;
}

std::string z10na2(int z10na2_input) {
	if (z10na2_input == 0) {
		return "00000000";
	}
	std::string wynik2 = "";
	while (z10na2_input > 0) {
		wynik2 = wynik2.insert(0, std::to_string(z10na2_input % 2));
		z10na2_input = z10na2_input / 2;
	}
	if (wynik2.size() < 8) {
		for (int i = wynik2.size(); i < 8; i++) {
			wynik2.insert(0, "0");
		}
	}
	return wynik2;
}

std::string zCna2(char z10na2_input) {
	int pom = z10na2_input;
	if (pom == '0') {
		return "00000000";
	}
	std::string wynik2 = "";

	if (pom >= 0) {
		while (pom > 0) {
			wynik2 = wynik2.insert(0, std::to_string(pom % 2));
			pom = pom / 2;
		}
		if (wynik2.size() < 8) {
			for (int i = wynik2.size(); i < 8; i++) {
				wynik2.insert(0, "0");
			}
		}
	}
	else {
		pom = 256 + pom;
		while (pom > 0) {
			wynik2 = wynik2.insert(0, std::to_string(pom % 2));
			pom = pom / 2;
		}
		if (wynik2.size() < 8) {
			for (int i = wynik2.size(); i < 8; i++) {
				wynik2.insert(0, "0");
			}
		}
	}

	return wynik2;
}

int bit_to_int(const std::string &s)
{
	int liczba = 0;
	for (int i = s.size() - 1, p = 1; i >= 0; i--, p *= 2)
	{
		if (s[i] == '1')
			liczba += p;
	}
	return liczba;
}

char bit_to_char(std::string test) {
	char pomocnicza;
	pomocnicza = bit_to_int(test);

	return pomocnicza;
}