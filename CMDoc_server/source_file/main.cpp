#include "../header_file/ChatServer.h"
#include "../header_file/Chatroom.h"
#include "../header_file/User.h"
#include "../header_file/MatchEngine.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	ChatServer server(8080);
	server.start();

	return 0;
}