#pragma once
//#define WIN32_LEAN_AND_MEAN  // Windows.h�� Winsock.h �浹 ����
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // Winsock ���̺귯�� ��ũ
#include "PacketDefine.h"



// Player* player[2] = { nullptr, nullptr };  // ���� �Ҵ����� ����

extern WSADATA wsa;
extern SOCKET sock;
extern SOCKADDR_IN serveraddr;
extern bool isReady;  // ���� Ŭ���̾�Ʈ�� �غ� ����
extern bool isConnected;
extern bool gameStarted;
extern int myID;  // Ŭ���̾�Ʈ�� ID
// extern int nowID;  // �α��� �κж����� �߰���

#define SERVERIP "127.0.0.1"  // localhost
//#define SERVERIP "192.168.219.101"

bool InitializeNetwork();
void CleanupNetwork();
// �α��� ��û �Լ�
bool SendLoginRequest(const char* username, const char* password);
void SendReadyClientToServer();
void SendNotReadyClientToServer();
void ReadyClient();
void SendPlayerMove();
void SendBulletMove();

DWORD WINAPI ProcessServer(LPVOID arg);
