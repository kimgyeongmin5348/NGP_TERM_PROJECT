#pragma once
// 여기에 보고 헤더파일 추가해야 할거 있을것 같으면 추가해줘
#include <WinSock2.h>
#include <Windows.h>
#include "PacketDefine.h"
#include <iostream>


// 여기에 Send랑 Recv 할거임


DWORD WINAPI ProcessServer(LPVOID arg);

Player* player[2] = { nullptr, nullptr };  // 동적 할당으로 변경


extern int myID;  // 클라이언트의 ID
// extern int nowID;  // 로그인 부분때문에 추가함

WSADATA wsa;
SOCKET sock = INVALID_SOCKET;  // 초기화 추가
SOCKADDR_IN serveraddr;
bool isReady = false;  // 현재 클라이언트의 준비 상태
bool isConnected = false;

#define SERVERIP "192.168.45.121"  //  이건 노트북ip 주소 ->192.168.40.29

bool InitializeNetwork() {

    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "윈속 초기화 실패" << endl;
        return false;
    }

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "소켓 생성 실패: " << WSAGetLastError() << endl;
        WSACleanup();
        return false;
    }

    // 서버 주소 설정
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(TCPPORT);

    // 서버 연결
    if (connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "서버 연결 실패: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    isConnected = true; // 연결 성공 시 상태 변경
    cout << "서버 연결 성공" << endl;

    // 서버와 통신할 스레드 생성
    HANDLE hThread = CreateThread(NULL, 0, ProcessServer, NULL, 0, NULL);
    if (hThread == NULL) {
        cout << "스레드 생성 실패" << endl;
        return false;
    }

    CloseHandle(hThread);
    return true;
}

void CleanupNetwork()
{
    // Player 객체 정리
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

// 로그인 요청 함수
bool SendLoginRequest(const char* playerid) {
    if (sock == INVALID_SOCKET) {
        cout << "서버에 연결되어 있지 않습니다." << endl;
        return false;
    }

    PacketID loginPacket;
    loginPacket.size = sizeof(PacketID);
    loginPacket.type = PACKET_ID;
    strncpy_s(loginPacket.id, playerid, MAX_ID_SIZE - 1);
    loginPacket.id[MAX_ID_SIZE - 1] = '\0';

    int retval = send(sock, (char*)&loginPacket, sizeof(loginPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "로그인 패킷 전송 실패: " << WSAGetLastError() << endl;
        return false;
    }
    return true;
}


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

    while (isConnected) {

        if (sock == INVALID_SOCKET) {
            std::cout << "소켓이 유효하지 않습니다." << std::endl;
            break;
        }

        retval = recv(sock, &type, sizeof(type), 0);
        if (retval == SOCKET_ERROR || retval == 0) {
            std::cout << "서버와의 연결이 끊어졌습니다." << std::endl;
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
    isConnected = false;
    return 0;
}