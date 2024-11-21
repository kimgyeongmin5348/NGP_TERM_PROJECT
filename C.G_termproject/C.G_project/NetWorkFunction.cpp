#include "PacketDefine.h"
#include "ServerToClient.h"

// 전역 변수 정의
WSADATA wsa;
SOCKET sock = INVALID_SOCKET;   // 초기화 추가
SOCKADDR_IN serveraddr;
bool isReady = false;
bool isConnected = false;
bool gameStarted = false;
int myID;

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
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
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

// 네트워크 정리
void CleanupNetwork() {
    closesocket(sock);
    WSACleanup();
    isConnected = false;
}

// 로그인 요청 함수
//bool SendLoginRequest(const char* playerid) {
//    if (sock == INVALID_SOCKET) {
//        cout << "서버에 연결되어 있지 않습니다." << endl;
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
//        cout << "로그인 패킷 전송 실패: " << WSAGetLastError() << endl;
//        return false;
//    }
//    return true;
//}

void err_quit(const char* msg) {
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void err_display(const char* msg) {
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    printf("[%s] %s\n", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

void err_display(int errcode) {
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    printf("[오류] %s\n", (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags) {
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }
    return (len - left);
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
            gameStarted = true;
            break;
        }
        }
    }
    return 0;
}