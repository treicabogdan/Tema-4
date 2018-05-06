#include "Client.h"

bool Client::ProcessPacket(Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(Message))
			return false;
		std::cout << Message << std::endl;
		break;
	}
	default: 
		std::cout << "Unrecognized packet: " << _packettype << std::endl; 
		break;
	}
	return true;
}

void Client::ClientThread()
{
	Packet PacketType;
	while (true)
	{
		if (!clientptr->GetPacketType(PacketType)) 
			break; 
		if (!clientptr->ProcessPacket(PacketType))
			break; 
	}
	std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->CloseConnection()) 
	{
		std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

Client::Client(std::string IP, int PORT)
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str());
	addr.sin_port = htons(PORT); 
	addr.sin_family = AF_INET;
	clientptr = this;
}

bool Client::Connect()
{
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); 
	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(Connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(NULL, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}