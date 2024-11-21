#include "framework.h"
#include "PacketDefine.h"

using namespace std;
PacketPlayerMove Player[2]; //�÷��̾� ����?
int nowID = 0; // Ŭ��� id
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;  // ������ ������ ���� �̺�Ʈ
SOCKET clientSockets[2];  // Ŭ���̾�Ʈ ���� ���� �迭

bool readyState[2] = { false, false }; // �� �÷��̾��� �غ� ����
bool gameStarted = false;
void GoToInGame();

void SaveID(const char* NewID) {
    strncpy(Player[nowID].playerid, NewID, MAX_ID_SIZE - 1);
    Player[nowID].playerid[MAX_ID_SIZE - 1] = '\0';
    nowID++;
    SetEvent(DataEvent);
    cout << "ID ���� �Ϸ�: " << NewID << endl;
}

bool CheckID(const char* playerid) {
    // DataEvent ���� ���� (���ʿ��� ����ȭ ����)
    for (int i = 0; i < nowID; ++i) {
        if (strcmp(Player[i].playerid, playerid) == 0) {
            cout << "�ߺ��� ID: " << playerid << endl;
            return false;
        }
    }
    cout << "��� ������ ID: " << playerid << endl;
    return true;
}

// Lobby
void SendReadyServerToClient()
{
    // ��� Ŭ���̾�Ʈ���� ���� �غ� ���� ����
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;

    WaitForSingleObject(DataEvent, INFINITE);
    for (int i = 0; i < 2; i++) {
        readyPacket.id = i;
        if (readyState[i]) {
            send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
        }
    }
    SetEvent(DataEvent);
}

void SendReadyCompleteServerToClient()
{
    // ��� �÷��̾ �غ�Ǿ����� Ȯ��
    bool allReady = true;
    for (int i = 0; i < 2; i++) {
        if (!readyState[i]) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        // ��� Ŭ���̾�Ʈ���� ���� ���� ���� ���� ����
        AllReady readyPacket;
        readyPacket.size = sizeof(AllReady);
        readyPacket.type = CLIENT_ALL_READY;

        for (int i = 0; i < 2; i++) {
            send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
        }

        // ���� ���� ���·� ��ȯ
        GoToInGame();
    }
}

// ���� ��Ȯ�ϰ� �̿ϼ� 
void GoToInGame()
{
    if (!gameStarted) {
        gameStarted = true;

        // �÷��̾� �ʱ� ��ġ ����
        for (int i = 0; i < 2; i++) {
            // Player[i].pos = { 0.0f, 0.0f, 0.0f };                // �ʱ� ��ġ ����  �̰� Ŭ��� ���� �ʿ�.
            Player[i].state = 0; // �ʱ� ���� ����
            readyState[i] = false;  // ���� ���� �� �غ� ���� �ʱ�ȭ
        }

        // ���� ���� �̺�Ʈ ����
        SetEvent(UpdateEvent);
    }
}

// inGame
void MakeBuildings()
{
    PacketBuildingMove buildingPacket;
    buildingPacket.size = sizeof(PacketBuildingMove);
    buildingPacket.type = PACKET_BUILDING_MOVE;

    for (int k = 0; k < 2; ++k) {
        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 10; ++j) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> random_building_x_pos(-20, 20);
                std::uniform_real_distribution<float> random_building_hight(1, 25);

                buildingPacket.pos.x = random_building_x_pos(gen);
                buildingPacket.pos.y = 0;
                buildingPacket.pos.z = 20.f * (j + 1);
                //scale.y = random_building_hight(gen);
                buildingPacket.num = i;
                buildingPacket.is_broken = false;
            }
        }
        send(clientSockets[k], (char*)&buildingPacket, sizeof(buildingPacket), 0);
    }

}

void ProcessMove()
{
    for (int i = 0; i < 2; ++i) {
        if (Player[i].state != 0) {
            //�Ѿ�, �ǹ�, �÷��̾��� �����ӿ� ���� ��ǥ�� ����, ó���ϴ� �Լ�
            PacketBulletMove bullet;
            bullet.size = sizeof(PacketBulletMove);
            bullet.type = PACKET_BULLET_MOVE;

            PacketBuildingMove building;
            building.size = sizeof(PacketBuildingMove);
            building.type = PACKET_BUILDING_MOVE;

            PacketPlayerMove player;
            player.size = sizeof(PacketPlayerMove);
            player.type = PACKET_PLAYER_MOVE;

            recvn(clientSockets[i], (char*)&bullet, sizeof(PacketBulletMove), 0);
            recvn(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
            recvn(clientSockets[i], (char*)&player, sizeof(PacketPlayerMove), 0);

            Player[i].pos.x += player.pos.x;
            Player[i].pos.y += player.pos.y;
            Player[i].pos.z += player.pos.z;
        }
    }
}

BOOL ColidePlayerToObjects()
{

    return FALSE;
}

BOOL ColideBulletToObjects()
{

    return FALSE;
}

void DeleteObjects()
{

}

void SendPacketMadebuildings()
{
    PacketBuildingMove building;
    building.size = sizeof(PacketBuildingMove);
    building.type = PACKET_BUILDING_MOVE;


    for (int i = 0; i < 2; ++i) {
        recv(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
        if (building.pos.z < 0.f) {
            MakeBuildings();
            send(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
        }
    }


}

void SendPacketMoveBuildings()
{
    PacketBuildingMove building;
    building.size = sizeof(PacketBuildingMove);
    building.type = PACKET_BUILDING_MOVE;

    for (int i = 0; i < 2; ++i) {
        send(clientSockets[i], (char*)&building.pos, sizeof(building.pos), 0);
    }
}

// ���� ������
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    int ClientNum = nowID - 1;
    char type;

    // ������ ����ŷ ���� ����
    u_long mode = 1;
    ioctlsocket(client_sock, FIONBIO, &mode);

    // Ÿ�Ӿƿ� ����
    DWORD timeout = 3000;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));


    // Ŭ���̾�Ʈ ���� ���� �� ó��
    auto cleanup = [&]() {
        WaitForSingleObject(DataEvent, INFINITE);
        readyState[ClientNum] = false;
        if (clientSockets[ClientNum] != INVALID_SOCKET) {
            closesocket(clientSockets[ClientNum]);
            clientSockets[ClientNum] = INVALID_SOCKET;
        }
        nowID--;
        SetEvent(DataEvent);
        SendReadyServerToClient();
        };

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

        retval = recvn(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                err_display("Client Thread Type recv()");
                break;
            }
        }
        switch (type) {
        case PACKET_ID: {
            PacketID loginPacket;
            retval = recvn(client_sock, (char*)&loginPacket, sizeof(loginPacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }

            ClientLoginUsePacket response;
            response.size = sizeof(ClientLoginUsePacket);
            response.type = CheckID(loginPacket.id) ? ID_USE : ID_NOT_USE;

            if (response.type == ID_USE) {
                SaveID(loginPacket.id);
                strncpy(response.playerid, loginPacket.id, MAX_ID_SIZE - 1);
                response.playerid[MAX_ID_SIZE - 1] = '\0';
            }

            send(client_sock, (char*)&response, sizeof(response), 0);
            break;
        }
        break;
        case CLIENT_READY: // Lobby �غ� ���� ó��
        {
            ReadyClientToServer readyPacket;
            retval = recvn(client_sock, (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }

            readyState[ClientNum] = true;
            SendReadyServerToClient();


            SendReadyCompleteServerToClient();
        }
        break;
        case CLIENT_NOTREADY: // Lobby �غ� ���� ó��
        {
            NotReadyClientToServer notReadyPacket;
            retval = recvn(client_sock, (char*)&notReadyPacket, sizeof(notReadyPacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            readyState[ClientNum] = false;
            SendReadyServerToClient();
        }
        break;
        case PACKET_PLAYER_MOVE:  // ���� �� �÷��̾� �̵� ó�� 
        {
            PacketPlayerMove movePacket;
            retval = recvn(client_sock, (char*)&movePacket, sizeof(movePacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            Player[ClientNum] = movePacket;
        }
        break;
        }

        if (ClientNum < 2) {
            SetEvent(ClientEvent[(ClientNum + 1) % 2]);
        }
    }

    cleanup();
    return 0;
}

// ������Ʈ ������
DWORD WINAPI ProcessUpdate(LPVOID arg)
{

    while (true)
    {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // �ǹ� ����
        MakeBuildings();

        // ������Ʈ �̵� ó��
        ProcessMove();

        // �浹 üũ
        if (ColidePlayerToObjects()) {
            DeleteObjects();
        }

        if (ColideBulletToObjects()) {
            DeleteObjects();
        }

        // Ŭ���̾�Ʈ�鿡�� ������Ʈ�� ���� ����
        SendPacketMadebuildings();

        SetEvent(ClientEvent[0]);
    }
    return 0;

}


int main()
{

    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    // �̺�Ʈ ����
    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DataEvent = CreateEvent(NULL, FALSE, TRUE, NULL);  // ������ ���ٿ� �̺�Ʈ

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("listen socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);

    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

    HANDLE hThread;
    hThread = CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);
    if (hThread != NULL) { CloseHandle(hThread); cout << "������Ʈ ������ ����" << endl; }

    while (true) {
        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        WaitForSingleObject(DataEvent, INFINITE);
        if (nowID >= 2) {
            SetEvent(DataEvent);
            closesocket(client_sock);
            continue;
        }
        clientSockets[nowID] = client_sock;
        SetEvent(DataEvent);



        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    closesocket(listen_sock);
    WSACleanup();
}