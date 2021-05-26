#include <webserv.hpp>

#include "ServerManager.hpp"
#include "Config.hpp"

int main()
{
	ServerManager manager;

	try
	{
		manager.config("sample.conf");
		manager.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	manager.serverClose();
}