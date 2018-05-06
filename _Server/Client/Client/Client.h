#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <iostream>

enum Packet
{
	P_ChatMessage,
	P_Test
};

class Client
{
public:
	Client(std::string IP, int PORT);
	bool Connect();

	bool SendString(std::string & _string);
	bool CloseConnection();
private:
	bool ProcessPacket(Packet _packettype);
	static void ClientThread();
	bool sendall(char * data, int totalbytes);
	bool Sendint32_t(int32_t _int32_t);
	bool SendPacketType(Packet _packettype);

	bool recvall(char * data, int totalbytes);
	bool Getint32_t(int32_t & _int32_t);
	bool GetPacketType(Packet & _packettype);
	bool GetString(std::string & _string);

private:
	SOCKET Connection;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr); 
};

static Client * clientptr; 