//===CLIENT===//
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
#include <conio.h>
#include <bitset>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "converts.h"

#pragma comment(lib, "Ws2_32.lib")

class client
{
private:

	WSADATA wsaData;
	SOCKET client_socket;
	SOCKADDR_IN dest_addr;

	char buffer[4];
	int sin_size = sizeof(sockaddr_in);
	int identyfikator = 0;
	std::string text;
	int zadanie;
	int buffer_size;

	std::bitset<6> OP;
	std::bitset<4> AN;
	std::bitset<8> id;
	std::bitset<7> czas;
	std::bitset<7> liczba;

	void odbierz_wiadomosc(SOCKET client_socket, char buffer[4], SOCKADDR_IN dest_addr, int sin_size);
	void spakuj();
	void odpakuj();
	void wyczysc();
	int bit_to_int(const std::string &s);

	void zadaj_polaczenia();
	void zadaj_id();
	void zadaj_zakonczenia();

public:
	int UDP();
};
