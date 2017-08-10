//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS﻿
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <mswsock.h>
#include <iostream>
#include "Header.h"
#include "kf\kf_time.h"

#pragma comment(lib, "ws2_32.lib")



void RecvPacket(SOCKET recieveSocket)
{
	char buffer[10000];
	sockaddr_in from;
	int fromLength = sizeof(sockaddr_in);
	int ret = recvfrom(recieveSocket, buffer, sizeof(buffer), NULL, (SOCKADDR*)&from, &fromLength);
	int e = WSAGetLastError();
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
	Packet *p = (Packet *)buffer;

	switch(p->type)
	{
		case 7:
		{
			PacketServerInfo *pi = (PacketServerInfo*)p;
			std::cout << "Pack Server info: " << "Width: " << pi->width << " Height: " << pi->height << std::endl;
			break;
		}
	}

}

int main()
{
	unsigned short port = 1300;

	//WinSock Setup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);
	if (WSAStartup(DllVersion, &wsaData) != 0) // If WSAStartup returns anything other than 0, Then an error has occured
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	sockaddr_in send_address;
	send_address.sin_family = AF_INET;
	send_address.sin_port = htons(port);
	send_address.sin_addr.s_addr = inet_addr("255.255.255.255");

	sockaddr_in recv_address;
	recv_address.sin_family = AF_INET;
	recv_address.sin_port = htons(port);
	recv_address.sin_addr.s_addr = INADDR_ANY;

	SOCKET _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (_socket == SOCKET_ERROR)
	{
		std::cout << "Error Opening socket: Error " << WSAGetLastError();
		return 1;
	}
	kf::Time timer;

	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	WSAIoctl(_socket,
		SIO_UDP_CONNRESET,
		&bNewBehavior,
		sizeof(bNewBehavior),
		NULL,
		0,
		&dwBytesReturned,
		NULL,
		NULL);

	BOOL bOptVal = TRUE;
	setsockopt(_socket,
		SOL_SOCKET,
		SO_BROADCAST,
		(const char *)&bOptVal,
		sizeof(BOOL));

	PacketClientAnnounce pca;
	pca.type = Packet::e_clientAnnounce;

	sendto(_socket, (const char*)&pca, sizeof(pca), NULL, (SOCKADDR*)&send_address, sizeof(send_address));
	long long t1 = timer.getTicks();

	int wait = 0;
	do
	{

		fd_set checkSockets;
		checkSockets.fd_count = 1;
		checkSockets.fd_array[0] = _socket;

		struct timeval t;
		t.tv_sec = 0;
		t.tv_usec = 0;

		wait = select(NULL, &checkSockets, NULL, NULL, &t);

		if (wait > 0)
		{
			long long t2 = timer.getTicks();
			std::cout << (timer.ticksToSeconds(t2 - t1)) << std::endl;
			RecvPacket(_socket);
			
		}

	} while (!wait);

	system("pause");

	WSACleanup();
	return 0;
}