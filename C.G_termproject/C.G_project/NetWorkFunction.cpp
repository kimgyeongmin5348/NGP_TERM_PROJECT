#include "PacketDefine.h"
#include "ServerToClient.h"

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
            gameStarted = true;
            break;
        }
        }
    }
    return 0;
}