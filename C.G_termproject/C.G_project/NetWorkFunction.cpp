#include "PacketDefine.h"
#include "ServerToClient.h"
#include "Scene.h"

// 전역 변수 정의
WSADATA wsa;
SOCKET sock = INVALID_SOCKET;   // 초기화 추가
SOCKADDR_IN serveraddr;
bool isReady = false;
bool isConnected = false;
bool gameStarted = false;
int myID{};

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
bool SendLoginRequest(const char* username, const char* password) {
    if (!isConnected) {
        cout << "서버에 연결되어 있지 않습니다." << endl;
        return false;
    }

    PacketLoginRequest loginPacket;
    loginPacket.size = sizeof(PacketLoginRequest);
    loginPacket.type = PACKET_LOGIN_REQUEST;
    strncpy(loginPacket.username, username, MAX_ID_SIZE - 1);
    strncpy(loginPacket.password, password, MAX_ID_SIZE - 1);
    loginPacket.username[MAX_ID_SIZE - 1] = '\0';
    loginPacket.password[MAX_ID_SIZE - 1] = '\0';

    // 패킷 타입 전송
    char type = PACKET_LOGIN_REQUEST;
    int retval = send(sock, &type, sizeof(char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "로그인 요청 타입 전송 실패" << endl;
        return false;
    }

    // 패킷 데이터 전송
    retval = send(sock, (char*)&loginPacket, sizeof(loginPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "로그인 패킷 전송 실패" << endl;
        return false;
    }

    return true;
}

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
    int received = 0;
    while (received < len) {
        int retval = recv(s, buf + received, len - received, flags);
        if (retval == SOCKET_ERROR || retval == 0) {
            cerr << "recv error or connection closed" << endl;
            return SOCKET_ERROR; // 에러 또는 연결 종료
        }
        received += retval;
    }
    return received;
}

void SendReadyClientToServer()
{
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;
    readyPacket.id = myID;

    // 먼저 타입 전송
    char type = CLIENT_READY;
    int retval = send(sock, (char*)&type, sizeof(unsigned char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "준비 상태 타입 전송 실패" << endl;
        return;
    }

    // 그 다음 패킷 전송
    retval = send(sock, (char*)&readyPacket, sizeof(readyPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "준비 상태 패킷 전송 실패" << endl;
        return;
    }

    cout << "준비 상태 전송 완료" << endl;
    isReady = true;
}

void SendPlayerMove(const glm::vec3& pos, int state) {
    if (!isConnected) return;

    PacketPlayerMove movePacket;
    movePacket.size = sizeof(PacketPlayerMove);
    movePacket.type = PACKET_PLAYER_MOVE;
    movePacket.pos = pos;
    movePacket.state = state;
    movePacket.id = myID;

    char type = PACKET_PLAYER_MOVE;
    send(sock, &type, sizeof(char), 0);
    send(sock, (char*)&movePacket, sizeof(movePacket), 0);
}

void SendBulletMove(const glm::vec3& bulletPos, int bulletIndex) {
    if (sock == INVALID_SOCKET) {
        cout << "서버에 연결되어 있지 않습니다." << endl;
        return;
    }

    // 총알 인덱스 범위 체크 추가
    if (bulletIndex < 0 || bulletIndex >= 30) {
        cout << "잘못된 총알 인덱스: " << bulletIndex << endl;
        return;
    }

    PacketBulletMove bulletPacket;
    bulletPacket.size = sizeof(PacketBulletMove);
    bulletPacket.type = PACKET_BULLET_MOVE;
    bulletPacket.pos = bulletPos;
    bulletPacket.num = bulletIndex;

    char type = PACKET_BULLET_MOVE;
    int retval = send(sock, &type, sizeof(char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "총알 이동 타입 전송 실패: " << WSAGetLastError() << endl;
        return;
    }

    retval = send(sock, (char*)&bulletPacket, sizeof(bulletPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "총알 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
        return;
    }
}

// 서버로부터 패킷을 받아 처리하는 스레드 함수
DWORD WINAPI ProcessServer(LPVOID arg) {
    while (isConnected) {
       /* if (sock == INVALID_SOCKET) {
            std::cout << "소켓이 유효하지 않습니다." << std::endl;
            break;
        }*/

        // 1. 먼저 패킷 타입만 수신
        unsigned char type;
        int retval = recv(sock, (char*)&type, sizeof(type), 0);
        if (retval <= 0) {
            cout << "서버와의 연결이 끊어졌습니다." << endl;
            break;
        }

        // 2. 패킷 타입에 따른 처리
        switch (type) {
        case PACKET_LOGIN_RESPONSE: {
            PacketLoginResponse loginResponse;
            retval = recvn(sock, (char*)&loginResponse, sizeof(loginResponse), 0);
            if (retval == SOCKET_ERROR) break;
            break;
        }
        case CLIENT_READY: {
            ReadyClientToServer packet;
            retval = recvn(sock, (char*)&packet, sizeof(ReadyClientToServer), 0);
            if (retval == SOCKET_ERROR) break;
            cout << "플레이어 " << (int)packet.id << " 준비완료" << endl;
            break;
        }    
        case PACKET_BUILDING_MOVE: {
            PacketBuildingMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(PacketBuildingMove), 0);
            if (retval == SOCKET_ERROR) break;
            // 서버로부터 받은 건물 위치 정보로 씬 업데이트
            Scene::GetInstance()->UpdateBuilding(packet.num, packet.scale, packet.pos);
            break;
        }
        case PACKET_PLAYER_MOVE: {
            PacketPlayerMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recvn() - PACKET_PLAYER_MOVE");
                break;
            }
            // 상대 플레이어 Update
            Scene::GetInstance()->UpdatePlayerPosition(packet.id, packet.pos);
            break;
        }
        case PACKET_BULLET_MOVE: {
            PacketBulletMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            //cout << "수신" << packet.num << " " << packet.pos << endl;

            // 총알 이동 처리
            Scene::GetInstance()->UpdateOtherBulletPosition(packet.num, packet.pos);
            break;
        }
        case PACKET_COLLIDE_BULLET_BUILDING: {
            PacketCollideBB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            // 총알-건물 충돌 처리
            Scene::GetInstance()->ProcessBulletBuildingCollision(packet.bullet_num, packet.building_num);
            break;
        }
        case PACKET_COLLIDE_PLAYER_BUILDING: {
            PacketCollidePB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            // 플레이어-건물 충돌 처리
            Scene::GetInstance()->ProcessPlayerBuildingCollision(packet.num);
            break;
        }
        case CLIENT_ALL_READY: {
            PacketAllReady packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
        }
        default:
            //cout << "알 수 없는 패킷 타입: " << (int)type << endl;
            break;
        }
    }
    return 0;
}