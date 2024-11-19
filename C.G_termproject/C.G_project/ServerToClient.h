#pragma once
// 여기에 보고 헤더파일 추가해야 할거 있을것 같으면 추가해줘
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

