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

//#define SERVERIP "127.0.0.1"  // localhost
#define SERVERIP "192.168.219.101"

bool InitializeNetwork();
void CleanupNetwork();
// �α��� ��û �Լ�
//bool SendLoginRequest(const char* playerid) {
//    if (sock == INVALID_SOCKET) {
//        cout << "������ ����Ǿ� ���� �ʽ��ϴ�." << endl;
//        return false;
//    }
//
//    PacketID loginPacket;
//    loginPacket.size = sizeof(PacketID);
//    loginPacket.type = PACKET_ID;
//    strncpy_s(loginPacket.id, playerid, MAX_ID_SIZE - 1);
//    loginPacket.id[MAX_ID_SIZE - 1] = '\0';
//
//    int retval = send(sock, (char*)&loginPacket, sizeof(loginPacket), 0);
//    if (retval == SOCKET_ERROR) {
//        cout << "�α��� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
//        return false;
//    }
//    return true;
//}
void SendReadyClientToServer();
void SendNotReadyClientToServer();
void ReadyClient();
void SendPlayerMove();

DWORD WINAPI ProcessServer(LPVOID arg);
