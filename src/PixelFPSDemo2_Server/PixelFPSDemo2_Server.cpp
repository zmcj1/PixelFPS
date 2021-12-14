#include "FPS_Server.hpp"

using namespace std;

int main()
{
	FPSServer server(17971);
	server.Start();

	while (true)
	{
		server.Update(-1, true);
	}

	return 0;
}