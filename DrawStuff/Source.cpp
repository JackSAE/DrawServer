//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS﻿
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <mswsock.h>
#include <iostream>
#include "Header.h"
#include "kf\kf_time.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "bitmap_class.h"
#include "MapInfo.h"

#pragma comment(lib, "ws2_32.lib")

MapInfo mi;

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
			
			mi.mapWidth = pi->width;
			mi.mapHeight = pi->height;
			std::cout << "Pack Server info: " << "Width: " << pi->width << " Height: " << pi->height << std::endl;
			break;
		}
	}

}

// functions to obtain COLORREF value - Red
BYTE Red(COLORREF color)
{
	return (color );
}
//& 63488) >> 11
//Blue
BYTE Blue(COLORREF color)
{
	return (color );
}
//& 31)
//Green
BYTE Green(COLORREF color)
{
	return (color );
}
//& 2016) >> 5


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

	//send pixels

	//Open bitmap - Only works with images that are 512x512 or smaller !!!!!!
	cBitmap bmp("image.bmp", 0, 0, 0);

	std::cout << "width: " << mi.mapWidth << " Height: " << mi.mapHeight << std::endl;
	std::cout << "bmp width: " << bmp.GetWidth() << " bmp Height: " << bmp.GetHeight() << std::endl;

	for (int x = 0; x < bmp.GetHeight(); ++x)
	{
		for (int y = 0; y < bmp.GetWidth(); ++y)
		{
			COLORREF colour = bmp.GetPixel(x, y);
			PacketPixel pp;
			pp.type = Packet::e_pixel;
			pp.x = mi.mapWidth / 2 - (bmp.GetWidth()/2) +x;
			pp.y = mi.mapHeight / 2 - (bmp.GetHeight() / 2) + y;

			//printf("%d",colour);
			//Bitmap holds -> A B G R 

			//Currently the colours are completetly wrong... 

			pp.b = int(Blue(colour));
			pp.r = int(Red(colour));
			pp.g = int(Green(colour));

			//Send Pixel to draw server
			//printf("Sending Packet");
			//std::cout << "\nx pos: " << mi.mapWidth / 2 - (bmp.GetWidth() / 2) + x << "\ny pos: " << mi.mapHeight / 2 - (bmp.GetHeight() / 2) + y << std::endl;
			sendto(_socket, (const char*)&pp, sizeof(pp), NULL, (SOCKADDR*)&send_address, sizeof(send_address));
		}
	}


	system("pause");

	WSACleanup();
	return 0;
}
