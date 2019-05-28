//===CLIENT===//
#include "client.h"
/*
Operacja	Odpowiedzi
Po³¹czenie		  000000
-¿¹danie			0000	0 klient
-przyjêcie			0001	1 server
-odmowa				0010	2 server
-b³¹d				0011	3 server
-¿¹danie ID			0100	4 klient
-przydzial ID		0101	5 server
Info o czasie	  000001
-pozosta³y T		0000	0 server
-koniec T			0001	1 server
Odgadywanie		  000010
-test liczby		0000	0 klient
-nie zgad³eœ		0001	1 server
Wynik			  000011
-wygrana			0000	0 server
-przegrana-czas		0001	1 server
-przeciwnik wygra³	0010	2 server
*/


void client::odbierz_wiadomosc(SOCKET client_socket, char buffer[4], SOCKADDR_IN dest_addr, int sin_size)
{

	while (true)
	{
		recvfrom(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, &sin_size);
		odpakuj();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void client::spakuj() {

	std::string pomocnicza;

	pomocnicza = OP.to_string();
	pomocnicza += AN.to_string();
	pomocnicza += id.to_string();
	pomocnicza += czas.to_string();
	pomocnicza += liczba.to_string();
	for (int i = 0; i < 4; i++) {
		std::string bajt_1 = pomocnicza.substr(i * 8, 8);
		buffer[i] = bit_to_int(bajt_1);
	}
}

void client::odpakuj() {

	std::string pomocnicza = "";

	for (int i = 0; i < 4; i++) {
		pomocnicza += zCna2(buffer[i]);
	}

	OP = bit_to_int(pomocnicza.substr(0, 6));
	AN = bit_to_int(pomocnicza.substr(6, 4));
	id = bit_to_int(pomocnicza.substr(10, 8));
	czas = bit_to_int(pomocnicza.substr(18, 7));
	liczba = bit_to_int(pomocnicza.substr(25, 7));
}

int client::bit_to_int(const std::string &s) {
	int liczba = 0;
	for (int i = s.size() - 1, p = 1; i >= 0; i--, p *= 2)
	{
		if (s[i] == '1')
			liczba += p;
	}
	return liczba;
}

void client::wyczysc() {
	OP = 0;
	AN = 0;
	id = 0;
	czas = 0;
	liczba = 0;
}

void client::zadaj_polaczenia() {
	//ZADANIE NAWIAZANIA POLACZENIA
	OP = 0;
	AN = 0;
	id = identyfikator;
	czas = 0;
	liczba = 0;
	spakuj();
}

void client::zadaj_id() {
	//ZADANIE PRZYDZIELENIA IDENTYFIKATORA
	OP = 0;
	AN = 4;
	id = identyfikator;
	czas = 0;
	liczba = 0;
	spakuj();
}


void client::zadaj_zakonczenia() {
	//ZADANIE ZAKONCZENIA SESJI
	OP = 3;
	AN = 0;
	id = identyfikator;
	czas = 0;
	liczba = 0;
	spakuj();
}


int client::UDP() {
	//INICJALIZACJA WINSOCK'A
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(20155);
	inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

	//ZADANIE POLACZENIA
	std::cout << "<---Proba nawiazania polaczenia--->\n";
	zadaj_polaczenia();
	sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);

	//ODBIERANIE WIADOMOSCI
	std::thread ODBIOR(&client::odbierz_wiadomosc, this, client_socket, buffer, dest_addr, sin_size);
	ODBIOR.detach();



	while (1) {

		if (OP == 0 && AN == 0) { //wysy³am takie ¿¹danie do servera
		}

		if (OP == 0 && AN == 1) { // otrzyma³em potwierdzenie pol¹czenia z servera
			std::cout << "<---Nawiazano polaczenie--->\n";
			std::cout << "<---Prosze o ID--->\n";
			zadaj_id(); // wysy³am proœbê o ID sesji
			sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
		}

		if (OP == 0 && AN == 2) { // odmówiono mi po³¹czenie - za du¿o clientów
			std::cout << "<---Odmowa nawiazania polaczenia--->\n";
			std::cout << "Co chcesz zrobic?\n[1] Sprobuj ponownie\n[2] Rozlacz\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<--Trwa proba ponownego polaczenia-->\n";
				wyczysc();
				zadaj_polaczenia();
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
			}
			if (reakcja == '2') {
				std::cout << "<--Zadanie zakonczenia polaczenia-->\n";
				wyczysc();
				zadaj_zakonczenia();
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
			}
		}

		if (OP == 0 && AN == 3) { // odmówiono mi po³¹czenia - b³¹d serwera
			std::cout << "<---B³ad servera - odmowa nawiazania polaczenia--->\n";
			std::cout << "Co chcesz zrobic?\n[1] Sprobuj ponownie\n[2] Rozlacz\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<--Trwa proba ponownego polaczenia-->\n";
				wyczysc();
				zadaj_polaczenia();
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
			}
			if (reakcja == '2') {
				std::cout << "<--Zadanie zakonczenia polaczenia-->\n";
				wyczysc();
				zadaj_zakonczenia();
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
			}
		}

		if (OP == 0 && AN == 4) {
		}

		if (OP == 0 && AN == 5) { // przydzielono mi ID z servera
			odpakuj();
			std::cout << "<----Przydzielono id--->" << z2na10(id.to_string()) << "\n";

			std::cout << "Co chcesz zrobic?\n[1] Wyslij liczbe do sprawdzenia\n[2] Czekaj\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				int TestowanaLiczba;
				std::cout << "<--Podaj liczbe do sprawdzenia:-->\n";
				std::cin >> TestowanaLiczba;
				
				OP = 2;
				AN = 0;
				liczba = TestowanaLiczba;
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
				
			}
			if (reakcja == '2') {
				wyczysc();
				while (OP != 1 && AN != 0) {
				}
			}
		}

		if (OP == 1 && AN == 0) {
			odpakuj();
			std::cout << "<----Czas do konca gry: " << z2na10(czas.to_string()) << ">\n";
		}

		if (OP == 1 && AN == 1) {
			odpakuj();
			std::cout << "<----Czas gry up³yna³ !: " << z2na10(czas.to_string()) << ">\n";

		}

		if (OP == 1 && AN == 2) {
		}

		if (OP == 2 && AN == 1) {
			odpakuj();
			std::cout << "<----Nie zgadles - to nie jest liczba: " << z2na10(liczba.to_string()) << ">\n";
			std::cout << "Co chcesz zrobic?\n[1] Wyslij liczbe do sprawdzenia\n[2] Czekaj\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				int TestowanaLiczba;
				std::cout << "<--Podaj liczbe do sprawdzenia:-->\n";
				std::cin >> TestowanaLiczba;
				OP = 2;
				AN = 0;
				liczba = TestowanaLiczba;
				spakuj();
				sendto(client_socket, buffer, 4, 0, (sockaddr*)&dest_addr, sin_size);
			}
			if (reakcja == '2') {
				wyczysc();
				while (OP != 1 && AN != 0) {
				}
			}
		}


		if (OP == 3 && AN == 0) {
			odpakuj();
			std::cout << "<----Zgadles !!! To jest liczba: " << z2na10(liczba.to_string()) << ">\n";
			wyczysc();
		}

		if (OP == 3 && AN == 2) {
			odpakuj();
			std::cout << "<----Przeciwnik odgadl liczbe i wygral !!!---->\n";
			wyczysc();
		}
		int i = sizeof(dest_addr);
	}

	closesocket(client_socket);
	WSACleanup();
	std::cout << "<----Rozlaczono--->\n";
	return 1;
}


void main() {
	client c;
	c.UDP();
}