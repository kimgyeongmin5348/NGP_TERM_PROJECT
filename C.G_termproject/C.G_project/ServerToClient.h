#pragma once
//#define WIN32_LEAN_AND_MEAN  // Windows.h와 Winsock.h 충돌 방지
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // Winsock 라이브러리 링크
#include "PacketDefine.h"



// Player* player[2] = { nullptr, nullptr };  // 동적 할당으로 변경

extern WSADATA wsa;
extern SOCKET sock;
extern SOCKADDR_IN serveraddr;
extern bool isReady;  // 현재 클라이언트의 준비 상태
extern bool isConnected;
extern bool gameStarted;
extern int myID;  // 클라이언트의 ID
// extern int nowID;  // 로그인 부분때문에 추가함

#define SERVERIP "127.0.0.1"  // localhost
//#define SERVERIP "192.168.219.101"

bool InitializeNetwork();
void CleanupNetwork();
// 로그인 요청 함수
bool SendLoginRequest(const char* username, const char* password);
void SendReadyClientToServer();
void SendNotReadyClientToServer();
void ReadyClient();
void SendPlayerMove();
void SendBulletMove();

DWORD WINAPI ProcessServer(LPVOID arg);
