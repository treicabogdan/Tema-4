#include "Client.h"


int main()
{
	Client myClient("127.0.0.1", 1111); 

	if (!myClient.Connect())
	{
		std::cout << "Failed to connect to server..." << std::endl;
		system("pause");
		return 1;
	}


	std::string userinput;
	while (true)
	{
		std::getline(std::cin, userinput);
		if (!myClient.SendString(userinput))
			break;
	}

	system("pause");
	return 0;
}

