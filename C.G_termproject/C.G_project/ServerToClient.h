#pragma once
#define WIN32_LEAN_AND_MEAN  // Windows.h�� Winsock.h �浹 ����
#include "PacketDefine.h"

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Winsock ���̺귯�� ��ũ


// Player* player[2] = { nullptr, nullptr };  // ���� �Ҵ����� ����

extern WSADATA wsa;
extern SOCKET sock;
extern SOCKADDR_IN serveraddr;
extern bool isReady;  // ���� Ŭ���̾�Ʈ�� �غ� ����
extern bool isConnected;
extern bool gameStarted;
extern int myID;  // Ŭ���̾�Ʈ�� ID
// extern int nowID;  // �α��� �κж����� �߰���

//#define SERVERIP "192.168.45.121"  //  �̰� ��Ʈ��ip �ּ� ->192.168.40.29 ���
//#define SERVERIP "172.30.1.72"  //  �ƿ�
//#define SERVERIP "192.168.219.102"  //  �λ�
#define SERVERIP "192.168.41.255"


bool InitializeNetwork();
void CleanupNetwork();

// �α��� ��û �Լ�
bool SendLoginRequest(const char* playerid);
void SendReadyClientToServer();
void SendNotReadyClientToServer();
void ReadyClient();



DWORD WINAPI ProcessServer(LPVOID arg);
