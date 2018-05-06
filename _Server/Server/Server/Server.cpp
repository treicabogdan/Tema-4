#include "Server.h"

Server::Server(int PORT, bool BroadcastPublically)
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) 
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	if (BroadcastPublically) 
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else 
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	addr.sin_port = htons(PORT); 
	addr.sin_family = AF_INET; 

	sListen = socket(AF_INET, SOCK_STREAM, NULL); 
	if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) 
	{
		std::string ErrorMsg = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) 
	{
		std::string ErrorMsg = "Failed to listen on listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	serverptr = this;
}

bool Server::ListenForNewConnection()
{
	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); 
	if (newConnection == 0) 
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
		return false;
	}
	else //If client connection properly accepted
	{
		std::cout << "Client Connected! ID:" << TotalConnections << std::endl;
		Connections[TotalConnections] = newConnection; 
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(TotalConnections), NULL, NULL); 
		std::string MOTD = "MOTD: Welcome! This is the message of the day!.";
		SendString(TotalConnections, MOTD);
		TotalConnections += 1; 
		return true;
	}
}

bool Server::ProcessPacket(int ID, Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage: 
	{
		std::string Message; 
		if (!GetString(ID, Message)) 
			return false; 
						  
		for (int i = 0; i < TotalConnections; i++)
		{
			if (i == ID) 
				continue;
			if (!SendString(i, Message)) 
			{
				std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << i << std::endl;
			}
		}
		std::cout << "Processed chat message packet from user ID: " << ID << std::endl;
		break;
	}

	default:
	{
		std::cout << "Unrecognized packet: " << _packettype << std::endl; 
		break;
	}
	}
	return true;
}

void Server::ClientHandlerThread(int ID) 
{
	Packet PacketType;
	while (true)
	{
		if (!serverptr->GetPacketType(ID, PacketType))
			break;
		if (!serverptr->ProcessPacket(ID, PacketType)) 
			break;
	}
	std::cout << "Lost connection to client ID: " << ID << std::endl;
	closesocket(serverptr->Connections[ID]);
	return;
}