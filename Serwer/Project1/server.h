//===SERVER===//
#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <string>
#include <array>
#include <vector>
#include <thread>

#include <bitset>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "converters.h"

#pragma comment(lib, "Ws2_32.lib")

class server {
private:
	WSADATA wsaData;
	SOCKET server_socket;
	SOCKADDR_IN my_address, current_address;
	SOCKADDR_IN client1_address, client2_address;
	SOCKET soc;

	int sin_size = sizeof(sockaddr_in);
	char buffer[4];
	int users_counter = 0;
	std::string text;
	int buffer_size;

	std::bitset<6> OP;
	std::bitset<4> AN;
	std::bitset<8> id;
	std::bitset<7> czas;
	std::bitset<7> liczba;

	int zajety_identyfikator;
	int wylosowany_identyfikator = 300; // losowy identyfikator ponad skale
	int czas_gry = 0;

	int ID1 = 0;
	int ID2 = 0;

	void spakuj();
	void odpakuj();
	void odczytaj();
	void wyczysc();
	int bit_to_int(const std::string &s);

	void ustaw_czas_gry();
	int ustaw_id();
	void przyjmij_polaczenie();
	void odmow_polaczenie();
	void przydziel_ID(int NewID);
	void odmow_ID();
	void potwierdz_zakonczenie();
	void wysylaj_czas();

public:
	int UDP();
};