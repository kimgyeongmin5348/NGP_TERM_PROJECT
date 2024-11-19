#pragma once
// 여기에 보고 헤더파일 추가해야 할거 있을것 같으면 추가해줘
#include "PacketDefine.h"
#include "Player.h"
#include "Object.h"


// 여기에 Send랑 Recv 할거임

Player player[2];


extern int myID;  // 클라이언트의 ID
WSADATA wsa;
SOCKET sock;
SOCKADDR_IN serveraddr;
bool isReady = false;  // 현재 클라이언트의 준비 상태

// bool isConnection{ false };

#define SERVERIP "127.0.0.1"

void SendReadyClientToServer()
{
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;
    readyPacket.id = myID;  // 클라이언트의 ID

    int retval = send(sock, (char*)&readyPacket, sizeof(readyPacket), 0);
    if (retval == SOCKET_ERROR) {
       cout << "준비 상태 전송 실패" << endl;
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
        cout << "준비 해제 상태 전송 실패" << endl;
        return;
    }
    isReady = false;
}

void ReadyClient()
{
   
   
}


// 서버로부터 패킷을 받아 처리하는 스레드 함수
DWORD WINAPI ProcessServer(LPVOID arg)
{
    char type;
    int retval;

    while (1) {
        retval = recvn(sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            cout << "서버와의 연결이 끊어졌습니다." << endl;
            break;
        }

        switch (type) {
        case CLIENT_READY: {
            ReadyClientToServer packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "플레이어 " << (int)packet.id << " 준비완료" << endl;
            break;
        }
        case CLIENT_NOTREADY: {
            NotReadyClientToServer packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "플레이어 " << (int)packet.id << " 준비해제" << endl;
            break;
        }
        case CLIENT_ALL_READY: {
            AllReady packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "모든 플레이어가 준비되었습니다." << endl;
            break;
        }
        }
    }
    return 0;
}