//===SERVER===//
#include "server.h"

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
-przegrana			0001	1 server
-przeciwnik wygra³	0010	2 server
*/


int server::UDP() {

	//INICJALIZACJA WINSOCK'A
	WSAStartup(MAKEWORD(2, 2), &wsaData); // Inicjalizacja WinSock

	my_address.sin_family = AF_INET; //wybor rodziny adresow IPv4
	my_address.sin_port = htons(20155); // przypisanie numeru portu 20155
	inet_pton(AF_INET, "127.0.0.1", &my_address.sin_addr); //przypisanie adresu IP 127.0.0.1

	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //tworzenie socketa
	bind(server_socket, (sockaddr*)&my_address, sizeof(my_address)); 

	srand(time(NULL));
	int LOSOWA_LICZBA;
	LOSOWA_LICZBA = rand() % 128; // wygeneruj liczbe do odgadniecia

	std::thread watek1;
	std::thread watek2;

	std::cout << "<--SERVER-->\n";

	while (1) {

		recvfrom(server_socket, buffer, 4, 0, (struct sockaddr *) &current_address, &sin_size);
		odpakuj();

		//  *******************   BLOK OBSLUGI POLACZEN ********************

		// ¿¹danie po³¹czenia od klienta
		if (OP == 0 && AN == 0) {
			std::cout << "<--Uzytkownik probuje sie polaczyc-->\n";
			if (users_counter == 0) { // jeœli to pierwszy klient to:
				client1_address = current_address;
				users_counter++;
				wyczysc();
				przyjmij_polaczenie();
				spakuj();
				sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
				std::cout << "<--Zezwolono na polaczenie uzytkownikowi #01-->\n";
			}
			else {
				if (users_counter == 1) { // a jeœli jeden ju¿ jest, to przyjmij drugiego
					if (current_address.sin_port != client1_address.sin_port) {
						client2_address = current_address;
						users_counter++;
						wyczysc();
						przyjmij_polaczenie();
						spakuj();
						sendto(server_socket, buffer, 4, 0, (sockaddr*)&client2_address, sin_size);
						std::cout << "<--Zezwolono na polaczenie uzytkownikowi #02-->\n";
					}
				}
				else { // a jeœli userów juz jest wiêcej 
					wyczysc();
					odmow_polaczenie(); // to odmów po³¹czenia
					sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
					std::cout << "<--Nie zezwolono na polaczenie uzytkownikowi #03-->\n";
				}
			}
		}


		if (OP == 0 && AN == 4) { // klient przyslal prosbe od ID
			std::cout << "<----Uzytkownik chce przydzielenia id--->\n";
			wyczysc();
			if (users_counter == 1) {
				ID1 = rand() % 255 + 1; // wygeneruj 1-sze ID 8 bitowe
				przydziel_ID(ID1);
				std::cout << "<----Przydzielono ID--->" << ID1 << "\n";
			}
			if (users_counter == 2) {
				ID2 = rand() % 255 + 1; // wygeneruj 2-gie ID 8 bitowe
				while (ID2 == ID1) // sprawdŸ czy przypadkiem nie jest takie samo i tak dlugo generuj, az bedzie inne
				{
					ID2 = rand() % 255 + 1;
				}
				przydziel_ID(ID2);
				std::cout << "<----Przydzielono ID--->" << ID2 << "\n";
			}
			spakuj();
			sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
			std::cout << "<----Przydzielono ID--->\n";
			if (users_counter == 2) { // jak sie pod³¹czy³o dwóch - oblicz game time
				ustaw_czas_gry();
				std::cout << "<----Ustawiono czas gry na: " << czas_gry << "\n";
				std::cout << "<----Ustawiono liczbe do odgadniecia na: " << LOSOWA_LICZBA << "\n";
				std::thread th = std::thread(&server::wysylaj_czas, this);
				th.detach();
			}
		}

		// ********************  KONIEC BLOKU OBSLUGI POLACZEN ****************


		// ********************  START BLOKU OBSLUGI ODGADYWANIA LICZBY ****************
		if (OP == 2 && AN == 0) {
			if (liczba != LOSOWA_LICZBA) {
				// jeœli nie zgad³, to wyœwietl info i wyœlij mu ¿e nie trafi³
				std::cout << "<--Klient z sesji: " << z2na10(id.to_string()) << " testowal liczbe: " << z2na10(liczba.to_string()) << "\n";
				OP = 2;
				AN = 1;
				spakuj();
				sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
			}
			else {
				// jeœli zgad³, to wyœwietl info i wyœlij mu ¿e wygral
				std::cout << "<--Klient z sesji: " << z2na10(id.to_string()) << " odgadl liczbe: " << z2na10(liczba.to_string()) << "\n";
				OP = 3;
				AN = 0;
				spakuj();
				sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
				OP = 3;
				AN = 2;
				spakuj();
				if (&current_address == &client1_address) {
					sendto(server_socket, buffer, 4, 0, (sockaddr*)&client2_address, sin_size);
				}
				else {
					sendto(server_socket, buffer, 4, 0, (sockaddr*)&client1_address, sin_size);
				}
				closesocket(server_socket);
				WSACleanup();

			}
		}


	}

	closesocket(server_socket);
	WSACleanup();

	return 1;
}

void server::spakuj() {

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

void server::odpakuj() {

	std::string pomocnicza;

	for (int i = 0; i < 4; i++) {
		pomocnicza += zCna2(buffer[i]);
	}

	OP = bit_to_int(pomocnicza.substr(0, 6));
	AN = bit_to_int(pomocnicza.substr(6, 4));
	id = bit_to_int(pomocnicza.substr(10, 8));
	czas = bit_to_int(pomocnicza.substr(18, 7));
	liczba = bit_to_int(pomocnicza.substr(25, 7));

}


void server::wyczysc() {
	OP = 0;
	AN = 0;
	id = 0;
	czas = 0;
	liczba = 0;
}

int server::bit_to_int(const std::string &s) {
	int liczba = 0;
	for (int i = s.size() - 1, p = 1; i >= 0; i--, p *= 2)
	{
		if (s[i] == '1')
			liczba += p;
	}
	return liczba;
}


void server::ustaw_czas_gry() {
	int czas_temp = ((ID1 + ID2) * 99) % 100 + 30;
	czas_gry = czas_temp;
}

int server::ustaw_id() {
	if (&current_address == &client1_address) {
		return ID1;
	}
	else {
		return ID2;
	}
}

void server::przyjmij_polaczenie() {
	//WYSLANIE INFORMACJI O PRZYJECIU POLACZENIA
	OP = 0;
	AN = 1;
	id = 0;
	czas = 0;
	liczba = 0;
	spakuj();
}

void server::odmow_polaczenie() {
	//WYSLANIE INFORMACJI O ODMOWIE POLACZENIA
	OP = 0;
	AN = 2;
	id = 0;
	czas = 0;
	liczba = 0;
	spakuj();
	
}


void server::przydziel_ID(int NewID) {
	//WYSLANIE POTWIERDZENIA O PRZYDZIELENIU ID
	OP = 0;
	AN = 5;
	id = NewID;
	czas = 0;
	liczba = 0;
	spakuj();
}

//dod zabezpieczenie
void server::odmow_ID() {
	//WYSLANIE ZADANIA POLACZENIA
	OP = 0;
	AN = 6;
	id = 0;
	czas = 0;
	liczba = 0;
	spakuj();
	sendto(server_socket, buffer, 4, 0, (sockaddr*)&current_address, sin_size);
}


void server::wysylaj_czas() {
	int t1 = czas_gry;
	int przerwa = 0;
	while (t1 > 0) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		t1--;
		przerwa++;
		if (przerwa == 10) {
			OP = 1;
			AN = 0;
			// przygotuj dane dla klienta 1
			id = ID1;
			czas = t1;
			liczba = 0;
			spakuj();
			// i wyslij mu czas
			sendto(server_socket, buffer, 4, 0, (sockaddr*)&client1_address, sin_size);
			id = ID2;
			spakuj();
			sendto(server_socket, buffer, 4, 0, (sockaddr*)&client2_address, sin_size);
			przerwa = 0;
		}
	}

	// a jak siê skonczy , to wyslij do obu "koniec czasu"
	OP = 1;
	AN = 1;
	id = ID1;
	czas = 0;
	liczba = 0;
	spakuj();
	sendto(server_socket, buffer, 4, 0, (sockaddr*)&client1_address, sin_size);
	id = ID2;
	spakuj();
	sendto(server_socket, buffer, 4, 0, (sockaddr*)&client2_address, sin_size);
}

void main() {
	server s;
	s.UDP();

}