#pragma once
#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ��
#include "stdafx.h"
#include "Scene.h"

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

using namespace std;

#define MAX_ID_SIZE 32


#define ID_USE 3
#define ID_NOT_USE 4
#define CLIENT_READY 5
#define CLIENT_NOTREADY 6
#define CLIENT_STATE_READY 7
#define CLIENT_ALL_READY 8
#define PACKET_PLAYER_MOVE 9
#define PACKET_BUILDING_MOVE 10
#define PACKET_BULLET_MOVE 11
#define PACKET_ID 12
#define PACKET_COLLIDE_BULLET_BUILDING 14
#define PACKET_COLLIDE_PLAYER_BUILDING 15

#define TCPPORT			4000


struct ClientLoginUsePacket
{
	char size;
	char type;
	char playerid[MAX_ID_SIZE];
	char id;
};

struct ReadyClientToServer
{
	char size;
	char type;
	char id;
};

struct NotReadyClientToServer
{
	char size;
	char type;
	char id;
};

struct StateReady
{
	char size;
	char type;
	char id;
	//char id;  <<- �̰� �ߺ� ���� ���... ���� �ʿ�
};

struct AllReady
{
	char size;
	char type;
	char id;
};

struct PacketPlayerMove
{
	char size;
	char type;
	glm::vec3 pos;
	char state;
	char playerid[MAX_ID_SIZE];
	char id;
};

struct PacketID
{
	char size;
	char type;
	char id[MAX_ID_SIZE];

};

struct PacketBuildingMove
{
	char size;
	char type;
	glm::vec3 pos;
	glm::vec3 scale;
	bool is_broken;
	int num; // ���� ��ȣ
};

struct PacketBulletMove
{
	char size;
	char type;
	glm::vec3 pos;
	int num; // �Ѿ� ��ȣ
};

struct PacketCollideBB
{
	char size;
	char type;
	int building_num;
	int bullet_num;
};

struct PacketCollidePB
{
	char size;
	char type;
	int building_num;
	char id[MAX_ID_SIZE];
};

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);
// ���� �Լ� ���� ���
void err_display(const char* msg);
// ���� �Լ� ���� ���
void err_display(int errcode);

int recvn(SOCKET s, char* buf, int len, int flags);