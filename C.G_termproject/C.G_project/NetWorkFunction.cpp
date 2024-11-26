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
int myID;

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
    readyPacket.id = myID;

    // ���� Ÿ�� ����
    char type = CLIENT_READY;
    int retval = send(sock, &type, sizeof(char), 0);
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

void SendNotReadyClientToServer()
{
    NotReadyClientToServer notReadyPacket;
    notReadyPacket.size = sizeof(NotReadyClientToServer);
    notReadyPacket.type = CLIENT_NOTREADY;
    notReadyPacket.id = myID;

    // ���� Ÿ�� ����
    char type = CLIENT_NOTREADY;
    int retval = send(sock, &type, sizeof(char), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� ���� Ÿ�� ���� ����" << endl;
        return;
    }

    // �� ���� ��Ŷ ����
    retval = send(sock, (char*)&notReadyPacket, sizeof(notReadyPacket), 0);
    if (retval == SOCKET_ERROR) {
        cout << "�غ� ���� ���� ��Ŷ ���� ����" << endl;
        return;
    }

    cout << "�غ� ���� ���� ���� �Ϸ�" << endl;
    isReady = false;
}

void ReadyClient()
{
    if (gameStarted) {
        cout << "������ �����մϴ�..." << endl;
        // ���⿡ ���� ���� ���� �߰�
        // Scene::GetInstance()->Initialize(); ��
    }
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

        // ��ü ��Ŷ�� �ѹ��� �޵��� ����
        char packet_buffer[256];  // ����� ũ���� ����
        int retval = recv(sock, packet_buffer, sizeof(char), 0);
        if (retval == SOCKET_ERROR || retval == 0) {
            std::cout << "�������� ������ ���������ϴ�." << std::endl;
            break;
        }

        char type = packet_buffer[0];
        switch (type) {
        case CLIENT_READY: {     // 5
            ReadyClientToServer packet;
            retval = recvn(sock, ((char*)&packet) + 1, sizeof(ReadyClientToServer) - 1, 0);
            if (retval == SOCKET_ERROR) break;
            cout << "�÷��̾� " << (int)packet.id << " �غ�Ϸ�" << endl;
            break;
        }
        case CLIENT_NOTREADY: { // 5
            NotReadyClientToServer packet;
            retval = recvn(sock, ((char*)&packet) + 1, sizeof(NotReadyClientToServer) - 1, 0);
            if (retval == SOCKET_ERROR) break;
            cout << "�÷��̾� " << (int)packet.id << " �غ�����" << endl;
            break;
        }
        case CLIENT_ALL_READY: {    // 8
            AllReady packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            cout << "��� �÷��̾ �غ�Ǿ����ϴ�." << endl;
            gameStarted = true;
            break;
        }
        case PACKET_BUILDING_MOVE: {    // 10
            PacketBuildingMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �����κ��� ���� �ǹ� ��ġ ������ �� ������Ʈ �ؾ��ϴ°�

         
        }

        case PACKET_PLAYER_MOVE: {  // 9
            PacketPlayerMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            cout << "��� Player : " << packet.pos.x << " " << packet.pos.y << endl;
            if (retval == SOCKET_ERROR) break;

            // ��� �÷��̾� �̵� ó��
            break;
        }

        case PACKET_BULLET_MOVE: {  // 11
            PacketBulletMove packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �Ѿ� �̵� ó��
            break;
        }
        case PACKET_COLLIDE_BULLET_BUILDING: {  // 14
            PacketCollideBB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �Ѿ�-�ǹ� �浹 ó��
            break;
        }
        case PACKET_COLLIDE_PLAYER_BUILDING: {  // 15
            PacketCollidePB packet;
            retval = recvn(sock, (char*)&packet, sizeof(packet), 0);
            if (retval == SOCKET_ERROR) break;

            // �÷��̾�-�ǹ� �浹 ó��
            break;
        }

        default:
            cout << "�� �� ���� ��Ŷ Ÿ��: " << (int)type << endl;
            break;
        }
    }
    return 0;
}