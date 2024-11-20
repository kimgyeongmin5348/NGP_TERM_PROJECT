#include "framework.h"
#include "PacketDefine.h"

using namespace std;
PacketPlayerMove Player[2]; //�÷��̾� ����?
int nowID = 0; // Ŭ��� id
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
SOCKET clientSockets[2];  // Ŭ���̾�Ʈ ���� ���� �迭

bool readyState[2] = { false, false }; // �� �÷��̾��� �غ� ����
bool gameStarted = false;

void SaveID(const char* NewID)
{
    strncpy(Player[nowID].playerid, NewID, strlen(NewID));
    nowID++;

    cout << "����Ϸ�" << endl;
}

bool CheckID(const char* playerid)
{
    for (int i = 0; i < nowID; ++i)
    {
        if (strcmp(Player[i].playerid, playerid) == false) {
            cout << "�ߺ�����" << endl;
            return false;
        }
    }
    cout << "�ߺ� ����" << endl;
    return true;
}

// Lobby
void SendReadyServerToClient()
{
    // ��� Ŭ���̾�Ʈ���� ���� �غ� ���� ����
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;

    for (int i = 0; i < 2; i++) {
        readyPacket.id = i;
        if (readyState[i]) {
             send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
        }
    }
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
    for(int k = 0; k< 2; ++k){}
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
            send(clientSockets[k], (char*)&buildingPacket, sizeof(buildingPacket), 0);
        }
    }
    }

}

void ProcessMove()
{
    for (int i = 0; i < 2; ++i) {
        if (Player[i].state != 0) { // If player is active
            Player[i].pos.x += Player[i].direction.x * Player[i].speed;
            Player[i].pos.y += Player[i].direction.y * Player[i].speed;
            Player[i].pos.z += Player[i].direction.z * Player[i].speed;
        }
    }
}

BOOL ColidePlayerToObjects()
{
    for (int i = 0; i < 2; ++i) {
        for (auto& obj : gameObjects) {
            if (Player[i].BoundingBox.Intersects(obj.BoundingBox)) {
                cout << "Collision detected between Player " << i << " and Object!" << endl;
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL ColideBulletToObjects()
{
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;

        for (auto& obj : gameObjects) {
            if (bullet.BoundingBox.Intersects(obj.BoundingBox)) {
                cout << "Bullet hit an object!" << endl;
                bullet.active = false; // Disable bullet
                return TRUE;
            }
        }
    }
    return FALSE;
}

void DeleteObjects()
{
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
        if (it->isDestroyed) {
            cout << "Deleting destroyed object" << endl;
            it = gameObjects.erase(it); // Remove object
        }
        else {
            ++it;
        }
    }
}

void SendPacketMadebuildings()
{
    BuildingPacket packet;
    packet.size = sizeof(BuildingPacket);
    packet.type = BUILDING_UPDATE;

    for (auto& obj : gameObjects) {
        if (obj.type == ObjectType::Building) {
            packet.position = obj.GetPosition();
            send(clientSockets[0], (char*)&packet, sizeof(packet), 0); // Broadcast to clients
        }
    }
}

// ���� ������
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    int ClientNum = nowID;
    char type;

    // Ŭ���̾�Ʈ ���� ���� �� ó��
    auto cleanup = [&]() {
        readyState[ClientNum] = false;
        closesocket(client_sock);
        clientSockets[ClientNum] = INVALID_SOCKET;
        
        SendReadyServerToClient();
        };

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

        retval = recvn(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            err_display("Client Thread Type recv()");
            break;
        }

        switch (type) {
        case PACKET_ID: // Login ó��
        {
            char playerid[MAX_ID_SIZE];
            retval = recvn(client_sock, playerid, MAX_ID_SIZE, 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            ClientLoginUsePacket loginPacket;
            loginPacket.size = sizeof(ClientLoginUsePacket);

            if (CheckID(playerid)) {
                loginPacket.type = ID_USE;
                SaveID(playerid);
            }
            else {
                loginPacket.type = ID_NOT_USE;
            }

            send(client_sock, (char*)&loginPacket, sizeof(loginPacket), 0);

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

        if (ClientNum < 2) SetEvent(ClientEvent[ClientNum + 1]);
        if (ClientNum == 2) SetEvent(UpdateEvent);

        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

        SetEvent(ClientEvent[(ClientNum + 1) % 3]);
    }

    closesocket(client_sock);
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

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("listen socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("listen bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    ClientEvent[2] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    HANDLE hThread;
    hThread = CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);
    if (hThread != NULL) { CloseHandle(hThread); cout << "������Ʈ ������ ����" << endl; }

    while (1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }
        // Ŭ���̾�Ʈ ���� ����
        clientSockets[nowID] = client_sock;


        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    closesocket(listen_sock);
    WSACleanup();
}