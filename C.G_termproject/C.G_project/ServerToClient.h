#pragma once
// ���⿡ ���� ������� �߰��ؾ� �Ұ� ������ ������ �߰�����
#include "PacketDefine.h"
#include "Player.h"
#include "Object.h"


// ���⿡ Send�� Recv �Ұ���

Player player[2];


WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;
bool isConnection{ false };

#define SERVERIP "127.0.0.1"

