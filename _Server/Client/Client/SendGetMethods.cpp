#include "Client.h"

bool Client::recvall(char * data, int totalbytes)
{
	int bytesreceived = 0;
	while (bytesreceived < totalbytes)
	{
		int RetnCheck = recv(Connection, data + bytesreceived, totalbytes - bytesreceived, NULL);
		if (RetnCheck == SOCKET_ERROR)
			return false;
		bytesreceived += RetnCheck;
	}
	return true; 
}

bool Client::sendall(char * data, int totalbytes)
{
	int bytessent = 0; 
	while (bytessent < totalbytes) 
	{
		int RetnCheck = send(Connection, data + bytessent, totalbytes - bytessent, NULL);
		if (RetnCheck == SOCKET_ERROR)
			return false; 
		bytessent += RetnCheck; 
	}
	return true; 
}
bool Client::Sendint32_t(int32_t _int32_t)
{
	_int32_t = htonl(_int32_t); 
	if (!sendall((char*)&_int32_t, sizeof(int32_t))) 
		return false; 
	return true; 
}

bool Client::Getint32_t(int32_t & _int32_t)
{
	if (!recvall((char*)&_int32_t, sizeof(int32_t))) 
		return false; 
	_int32_t = ntohl(_int32_t); 
	return true;
}

bool Client::SendPacketType(Packet _packettype)
{
	if (!Sendint32_t(_packettype)) 
		return false; 
	return true; 
}

bool Client::GetPacketType(Packet & _packettype)
{
	int packettype;
	if (!Getint32_t(packettype)) 
		return false; 
	_packettype = (Packet)packettype;
	return true;
}

bool Client::SendString(std::string & _string)
{
	if (!SendPacketType(P_ChatMessage)) 
		return false;
	int32_t bufferlength = _string.size(); 
	if (!Sendint32_t(bufferlength)) 
		return false; 
	if (!sendall((char*)_string.c_str(), bufferlength)) 
		return false; 
	return true; 
}

bool Client::GetString(std::string & _string)
{
	int32_t bufferlength; 
	if (!Getint32_t(bufferlength)) 
		return false;
	char * buffer = new char[bufferlength + 1]; 
	buffer[bufferlength] = '\0';
	if (!recvall(buffer, bufferlength))
	{
		delete[] buffer;
		return false;
	}
	_string = buffer;
	delete[] buffer;
	return true;
}