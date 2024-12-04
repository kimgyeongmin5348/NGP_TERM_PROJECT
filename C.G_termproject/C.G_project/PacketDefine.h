#pragma once
#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄
#include "stdafx.h"
#include "Scene.h"

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

using namespace std;

#define MAX_ID_SIZE 32

#define PACKET_LOGIN_REQUEST 3  
#define PACKET_LOGIN_RESPONSE 4
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


// 로그인 요청 패킷 구조체 추가
struct PacketLoginRequest {
	char size;
	char type;
	char username[MAX_ID_SIZE];
	char password[MAX_ID_SIZE];
};

// 로그인 응답 패킷 구조체
struct PacketLoginResponse {
	char size;
	char type;
	bool success;
	int userID;
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
	//char id;  <<- 이거 중복 오류 뜬다... 수정 필요
};

struct PacketAllReady
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
	int num; // 빌딩 번호
};

struct PacketBulletMove
{
	char size;
	char type;
	glm::vec3 pos;
	int num; // 총알 번호
	bool active;
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
	int num;
	char id[MAX_ID_SIZE];
};

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg);
// 소켓 함수 오류 출력
void err_display(const char* msg);
// 소켓 함수 오류 출력
void err_display(int errcode);

int recvn(SOCKET s, char* buf, int len, int flags);