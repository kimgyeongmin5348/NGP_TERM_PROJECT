#include "PacketDefine.h"
#include "Player.h"
#include "Object.h"


// 여기에 Send랑 Recv 할거임

Player player[2];


WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;
bool isConnection{ false };

#define SERVERIP "127.0.0.1"

