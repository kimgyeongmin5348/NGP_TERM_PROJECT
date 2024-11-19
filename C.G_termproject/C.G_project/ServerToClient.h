#pragma once
// ���⿡ ���� ������� �߰��ؾ� �Ұ� ������ ������ �߰�����
#include "PacketDefine.h"
#include "Player.h"
#include "Object.h"


// ���⿡ Send�� Recv �Ұ���

Player player[2];


extern int myID;  // Ŭ���̾�Ʈ�� ID
WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;
bool isReady = false;  // ���� Ŭ���̾�Ʈ�� �غ� ����

// bool isConnection{ false };

#define SERVERIP "127.0.0.1"

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

    while (1) {
        retval = recvn(sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            cout << "�������� ������ ���������ϴ�." << endl;
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
    return 0;
}