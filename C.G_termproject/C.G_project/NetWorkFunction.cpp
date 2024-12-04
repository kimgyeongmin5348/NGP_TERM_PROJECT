#include "PacketDefine.h"
#include "ServerToClient.h"
#include "Scene.h"

// ���� ���� ����
WSADATA wsa;
SOCKET sock = INVALID_SOCKET;   // �ʱ�ȭ �߰�
SOCKADDR_IN serveraddr;
bool isReady = false;
bool isConnected = false;
bool gameStarted = false;
int myID{};

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
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
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

// ��Ʈ��ũ ����
void CleanupNetwork() {
    closesocket(sock);
    WSACleanup();
    isConnected = false;
}

// �α��� ��û �Լ�
bool SendLoginRequest(const char* username, const char* password) {
    if (!isConnected) {
        cout << "������ ����Ǿ� ���� �ʽ��ϴ�." << endl;
        return false;
    }

    PacketLoginRequest loginPacket;
    loginPacket.size = sizeof(PacketLoginRequest);
    loginPacket.type = PACKET_LOGIN_REQUEST;
    strncpy(loginPacket.username, username, MAX_ID_SIZE - 1);
    strncpy(loginPacket.password, password, MAX_ID_SIZE - 1);
    loginPacket.username[MAX_ID_SIZE - 1] = '\0';
    loginPacket.password[MAX_ID_SIZE - 1] = '\0';

    // ��Ŷ Ÿ�� ����
    char type = PACKET_LOGIN_REQUEST;
    int retval = send(sock, &type, sizeof(char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�α��� ��û Ÿ�� ���� ����" << endl;
        return false;
    }

    // ��Ŷ ������ ����
    retval = send(sock, (char*)&loginPacket, sizeof(loginPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�α��� ��Ŷ ���� ����" << endl;
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
    printf("[����] %s\n", (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags) {
    int received = 0;
    while (received < len) {
        int retval = recv(s, buf + received, len - received, flags);
        if (retval == SOCKET_ERROR || retval == 0) {
            cerr << "recv error or connection closed" << endl;
            return SOCKET_ERROR; // ���� �Ǵ� ���� ����
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

    // ���� Ÿ�� ����
    char type = CLIENT_READY;
    int retval = send(sock, (char*)&type, sizeof(unsigned char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� Ÿ�� ���� ����" << endl;
        return;
    }

    // �� ���� ��Ŷ ����
    retval = send(sock, (char*)&readyPacket, sizeof(readyPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� ��Ŷ ���� ����" << endl;
        return;
    }

    cout << "�غ� ���� ���� �Ϸ�" << endl;
    isReady = true;
}

void SendPlayerMove(const glm::vec3& pos, int state) 
{
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

void SendBulletMove(const glm::vec3& bulletPos, int bulletIndex) 
{
    if (sock == INVALID_SOCKET) {
        cout << "������ ����Ǿ� ���� �ʽ��ϴ�." << endl;
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
        cout << "�Ѿ� �̵� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
        return;
    }

    retval = send(sock, (char*)&bulletPacket, sizeof(bulletPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�Ѿ� �̵� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
        return;
    }
}

// �����κ��� ��Ŷ�� �޾� ó���ϴ� ������ �Լ�
DWORD WINAPI ProcessServer(LPVOID arg) 
{
    while (isConnected) {
       /* if (sock == INVALID_SOCKET) {
            std::cout << "������ ��ȿ���� �ʽ��ϴ�." << std::endl;
            break;
        }*/

        // 1. ���� ��Ŷ Ÿ�Ը� ����
        unsigned char type;
        int retval = recv(sock, (char*)&type, sizeof(type), 0);
        if (retval <= 0) {
            cout << "�������� ������ ���������ϴ�." << endl;
            break;
        }

        // 2. ��Ŷ Ÿ�Կ� ���� ó��
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
            cout << "�÷��̾� " << (int)packet.id << " �غ�Ϸ�" << endl;
            break;
        }    
        case PACKET_BUILDING_MOVE: {
            PacketBuildingMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(PacketBuildingMove), 0);
            if (retval == SOCKET_ERROR) break;
            // �����κ��� ���� �ǹ� ��ġ ������ �� ������Ʈ
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
            // ��� �÷��̾� Update
            Scene::GetInstance()->UpdatePlayerPosition(packet.id, packet.pos);
            break;
        }
        case PACKET_BULLET_MOVE: {
            PacketBulletMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �Ѿ� �̵� ó��
            Scene::GetInstance()->UpdateOtherBulletPosition(packet.num, packet.pos);
            break;
        }
        case PACKET_COLLIDE_BULLET_BUILDING: {
            PacketCollideBB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �Ѿ�-�ǹ� �浹 ó��
            Scene::GetInstance()->ProcessBulletBuildingCollision(packet.bullet_num, packet.building_num);
            break;
        }
        case PACKET_COLLIDE_PLAYER_BUILDING: {
            PacketCollidePB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            // �÷��̾�-�ǹ� �浹 ó��
            Scene::GetInstance()->ProcessPlayerBuildingCollision(packet.num);
            break;
        }
        case CLIENT_ALL_READY: {
            PacketAllReady packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;
            Scene::GetInstance()->StartScoreCount();
        }
        default:
            //cout << "�� �� ���� ��Ŷ Ÿ��: " << (int)type << endl;
            break;
        }
    }
    return 0;
}