// Socket.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")
#define DEFAULT_BUFLEN 512

using namespace std;

int main() {
	WSADATA wsaData;

	//Initalisation librairies

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		cerr << stderr << "WSAStartup failed." << endl;
		return 1;
	}

	//Paramétrage socket
	struct addrinfo hints;
	struct addrinfo *servinfo; 

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_protocol = IPPROTO_TCP;

	int iResult = getaddrinfo("127.0.0.1", "8080", &hints, &servinfo);
	if (iResult != 0) {
		cout << "getaddrinfo failed: " << iResult <<endl;
		WSACleanup();
		return 1;
	}

	//Listen
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(servinfo);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, servinfo->ai_addr, (int)servinfo->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(servinfo);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket = INVALID_SOCKET;

	// Connexion client
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "accept failed: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	char recvbuf[DEFAULT_BUFLEN];
	//char chemin[];
	int recvbuflen = DEFAULT_BUFLEN;

	// Réception des données
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			int i = 0;
			printf_s("\nBytes received: ");
			for (i = 0; i < recvbuflen; i++) {
				printf_s("%c", recvbuf[i]);
				if ((recvbuf[i] == 108) && (recvbuf[i - 1] == 109) && (recvbuf[i - 2] == 120) && (recvbuf[i - 3] == 46)) {
					i = recvbuflen;
				}
			}
		}
		else if (iResult == 0) {
			cout << "\nClosing connexion..." << endl;
		}
		else {
			cout << "recv failed: " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);
	cout << "Connection closing..." << endl;
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}


	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}