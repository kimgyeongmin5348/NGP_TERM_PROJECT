#pragma once
// ���⿡ ���� ������� �߰��ؾ� �Ұ� ������ ������ �߰�����
#include <WinSock2.h>
#include <Windows.h>
#include "PacketDefine.h"
#include <iostream>


// ���⿡ Send�� Recv �Ұ���


DWORD WINAPI ProcessServer(LPVOID arg);

Player* player[2] = { nullptr, nullptr };  // ���� �Ҵ����� ����


extern int myID;  // Ŭ���̾�Ʈ�� ID
// extern int nowID;  // �α��� �κж����� �߰���

WSADATA wsa;
SOCKET sock = INVALID_SOCKET;  // �ʱ�ȭ �߰�
SOCKADDR_IN serveraddr;
bool isReady = false;  // ���� Ŭ���̾�Ʈ�� �غ� ����
bool isConnected = false;

#define SERVERIP "192.168.45.121"  //  �̰� ��Ʈ��ip �ּ� ->192.168.40.29

bool InitializeNetwork() {

    // ���� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "���� �ʱ�ȭ ����" << endl;
        return false;
    }

    // ���� ����
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "���� ���� ����: " << WSAGetLastError() << endl;
        WSACleanup();
        return false;
    }

    // ���� �ּ� ����
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(TCPPORT);

    // ���� ����
    if (connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "���� ���� ����: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    isConnected = true; // ���� ���� �� ���� ����
    cout << "���� ���� ����" << endl;

    // ������ ����� ������ ����
    HANDLE hThread = CreateThread(NULL, 0, ProcessServer, NULL, 0, NULL);
    if (hThread == NULL) {
        cout << "������ ���� ����" << endl;
        return false;
    }

    CloseHandle(hThread);
    return true;
}

void CleanupNetwork()
{
    // Player ��ü ����
    for (int i = 0; i < 2; i++) {
        if (player[i] != nullptr) {
            delete player[i];
            player[i] = nullptr;
        }
    }

    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    WSACleanup();
}

// �α��� ��û �Լ�
bool SendLoginRequest(const char* playerid) {
    if (sock == INVALID_SOCKET) {
        cout << "������ ����Ǿ� ���� �ʽ��ϴ�." << endl;
        return false;
    }

    PacketID loginPacket;
    loginPacket.size = sizeof(PacketID);
    loginPacket.type = PACKET_ID;
    strncpy_s(loginPacket.id, playerid, MAX_ID_SIZE - 1);
    loginPacket.id[MAX_ID_SIZE - 1] = '\0';

    int retval = send(sock, (char*)&loginPacket, sizeof(loginPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�α��� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
        return false;
    }
    return true;
}


void SendReadyClientToServer()
{
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;
    readyPacket.id = myID;  // Ŭ���̾�Ʈ�� ID

    int retval = send(sock, (char*)&readyPacket, sizeof(readyPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� ���� ����" << endl;
        return;
    }
    isReady = true;
}

void SendNotReadyClientToServer()
{
    NotReadyClientToServer notReadyPacket;
    notReadyPacket.size = sizeof(NotReadyClientToServer);
    notReadyPacket.type = CLIENT_NOTREADY;
    notReadyPacket.id = myID;

    int retval = send(sock, (char*)&notReadyPacket, sizeof(notReadyPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� ���� ���� ����" << endl;
        return;
    }
    isReady = false;
}

void ReadyClient()
{


}


// �����κ��� ��Ŷ�� �޾� ó���ϴ� ������ �Լ�
DWORD WINAPI ProcessServer(LPVOID arg)
{
    char type;
    int retval;

    while (isConnected) {

        if (sock == INVALID_SOCKET) {
            std::cout << "������ ��ȿ���� �ʽ��ϴ�." << std::endl;
            break;
        }

        retval = recv(sock, &type, sizeof(type), 0);
        if (retval == SOCKET_ERROR || retval == 0) {
            std::cout << "�������� ������ ���������ϴ�." << std::endl;
            break;
        }

        switch (type) {
        case CLIENT_READY: {
            ReadyClientToServer packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "�÷��̾� " << (int)packet.id << " �غ�Ϸ�" << endl;
            break;
        }
        case CLIENT_NOTREADY: {
            NotReadyClientToServer packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "�÷��̾� " << (int)packet.id << " �غ�����" << endl;
            break;
        }
        case CLIENT_ALL_READY: {
            AllReady packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "��� �÷��̾ �غ�Ǿ����ϴ�." << endl;
            break;
        }
        }
    }
    isConnected = false;
    return 0;
}