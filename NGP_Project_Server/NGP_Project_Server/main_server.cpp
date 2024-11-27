#include "framework.h"
#include "PacketDefine.h"

using namespace std;

// ���� ���� ����
PacketPlayerMove Player[2];
PacketBulletMove Bullet[2][30]; // Ŭ�󸶴� 30��
PacketBuildingMove Building[100]; 
PacketCollideBB  bb;

int nowID = 0;
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;
SOCKET clientSockets[2];
bool readyState[2] = { false, false };
bool gameStarted = false;
vector<PacketBuildingMove> g_buildings(100);

// �Լ� ����
//void SaveID();
void GoToInGame();
void MakeBuildings();
void ProcessMove();
BOOL ColidePlayerToObjects();
void ColideBulletToObjects();
void DeleteObjects();
//void SendPacketMadebuildings();
//void SendPacketMoveBuildings();

//void SaveID(const char* NewID) {
//    strncpy(Player[nowID].playerid, NewID, MAX_ID_SIZE - 1);
//    Player[nowID].playerid[MAX_ID_SIZE - 1] = '\0';
//    nowID++;
//    SetEvent(DataEvent);
//    cout << "ID ���� �Ϸ�: " << NewID << endl;
//}
//
//bool CheckID(const char* playerid) {
//    // DataEvent ���� ���� (���ʿ��� ����ȭ ����)
//    for (int i = 0; i < nowID; ++i) {
//        if (strcmp(Player[i].playerid, playerid) == 0) {
//            cout << "�ߺ��� ID: " << playerid << endl;
//            return false;
//        }
//    }
//    cout << "��� ������ ID: " << playerid << endl;
//    return true;
//}

// Lobby ���� �Լ�
void SendReadyServerToClient() 
{
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;

    WaitForSingleObject(DataEvent, INFINITE);
    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            char type = CLIENT_READY;
            int retval = send(clientSockets[i], &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�غ� ���� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
            readyPacket.id = i;
            retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�غ� ���� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
        }
    }
    SetEvent(DataEvent);
}

//void SendReadyCompleteServerToClient() 
//{
//    WaitForSingleObject(DataEvent, INFINITE);
//
//    bool allReady = true;
//    for (int i = 0; i < 2; i++) {
//        if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
//            allReady = false;
//            break;
//        }
//    }
//
//    if (allReady) {
//        cout << "\n��� �÷��̾ �غ�Ǿ����ϴ�!" << endl;
//        cout << "������ �����մϴ�..." << endl;
//
//        PacketAllReady readyPacket;
//        readyPacket.size = sizeof(PacketAllReady);
//        readyPacket.type = CLIENT_ALL_READY;
//
//        for (int i = 0; i < 2; i++) {
//            if (clientSockets[i] != INVALID_SOCKET) {
//                char type = CLIENT_ALL_READY;
//                send(clientSockets[i], &type, sizeof(char), 0);
//                send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
//            }
//        }
//        gameStarted = true;
//        SetEvent(UpdateEvent);
//    }
//    SetEvent(DataEvent);
//    cout << "=== �غ� ��Ŷ ó�� �Ϸ� ===" << endl;
//}


// InGame ���� �Լ�
void MakeBuildings() 
{
    WaitForSingleObject(DataEvent, INFINITE);

    //PacketBuildingMove buildingPacket;
    //buildingPacket.size = sizeof(PacketBuildingMove);
    //buildingPacket.type = PACKET_BUILDING_MOVE;

    vector<PacketBuildingMove> buildings;

    for (int i = 0; i < 100; ++i) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> random_x(-20, 20);
        uniform_real_distribution<float> random_height(1, 25);

        Building[i].pos.x = random_x(gen);
        Building[i].pos.y = 0;
        Building[i].pos.z = 20.f;
        Building[i].scale.x = 2.0f;
        Building[i].scale.y = random_height(gen);
        Building[i].scale.z = 4.0f;
        Building[i].num = i;
        Building[i].is_broken = false;
        buildings.push_back(Building[i]);
    }

        // Ŭ���̾�Ʈ�� ����
        for (int k = 0; k < 2; ++k) {
            if (clientSockets[k] != INVALID_SOCKET) {
                char type = PACKET_BUILDING_MOVE;
                send(clientSockets[k], &type, sizeof(char), 0);
                send(clientSockets[k], (char*)&g_buildings[i], sizeof(PacketBuildingMove), 0);
            }
        }
    }

    SetEvent(DataEvent);
}

void ProcessMove() 
{
    WaitForSingleObject(DataEvent, INFINITE);

    /*****************
      Update Building 
    ******************/
    PacketBuildingMove buildingPacket;
    ZeroMemory(&buildingPacket, sizeof(PacketBuildingMove));
    buildingPacket.size = sizeof(PacketBuildingMove);
    buildingPacket.type = PACKET_BUILDING_MOVE;

    // ���� ��ġ ������ �����ϱ� ���� static ���� �߰�
    static vector<glm::vec3> buildingPositions(100);  // ���� ��ġ �����

    for (int i = 0; i < 100; ++i) {
        buildingPacket.num = i;
        buildingPacket.pos = buildingPositions[i];  // ����� ��ġ ���
        buildingPacket.pos.z -= 0.1f;  // z�� �������� �̵�


        // Ŭ���̾�Ʈ�� ����
        for (int j = 0; j < 2; ++j) {
            if (clientSockets[j] != INVALID_SOCKET) {
                char type = PACKET_BUILDING_MOVE;
               // send(clientSockets[j], &type, sizeof(char), 0);
               // send(clientSockets[j], (char*)&buildingPacket, sizeof(buildingPacket), 0);
            }
        }
    }

    /**************
      Update Player
    ***************/
    //ProcessClient() ���� �������� Player�� �����ϴ� ������ ��ü


    /**************
      Update Bullet
    ***************/
    //ProcessClient() ���� �������� Bullet�� �����ϴ� ������ ��ü


    /*****************
      Collide Check BB
    ******************/


    /*****************
      Collide Check PB
    ******************/


    /***********************
      Send PacketPlayerMove
    ************************/
    int retval;
    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            char type = PACKET_PLAYER_MOVE;
            retval = send(clientSockets[i], &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�÷��̾� �̵� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
            retval = send(clientSockets[i], (char*)&Player[(i + 1) % 2], sizeof(PacketPlayerMove), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�÷��̾� �̵� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
        }
    }

    /**********************
      Send PacketBulletMove
    ***********************/


    SetEvent(DataEvent);
}

BOOL ColidePlayerToObjects()
{

    return FALSE;
}

void ColideBulletToObjects()
{
    bb.size = sizeof(PacketCollideBB);
    bb.type = PACKET_COLLIDE_BULLET_BUILDING;

    bool collision = false;

    for (int k = 0; k < 2; ++k) {
        if (collision) break;
        for (int i = 0; i < 30; ++i) {
            if (collision) break;
            for (int j = 0; j < 100; ++j) {
                if (Building[i].pos.x - 0.6f < Bullet[k][j].pos.x &&
                    Bullet[k][j].pos.x < Building[i].pos.x + 0.6f &&
                    Bullet[k][j].pos.y < Building[i].scale.y / 5 - 0.2f &&
                    Bullet[k][j].pos.z < Building[i].pos.z + 1 &&
                    Bullet[k][j].pos.z > Building[i].pos.z - 1) {
                    bb.building_num = i;
                    bb.bullet_num = j;
                    send(clientSockets[k], &bb.type, sizeof(char), 0);
                    send(clientSockets[k], (char*)&bb, sizeof(PacketCollideBB), 0);
                    cout << "�浹 ��Ŷ ����" << endl;                 
                    DeleteObjects();
                    collision = true;
                    break;
                }
            }
        }
    }
}

void DeleteObjects()
{
    for (int k = 0; k < 2; ++k) {
        Building[bb.building_num].scale.y = 0;    
        char type = PACKET_BUILDING_MOVE;
        send(clientSockets[k], &type, sizeof(char), 0);
        send(clientSockets[k], (char*)&Building, sizeof(PacketBuildingMove), 0);
        cout << "���� ��Ŷ ����" << endl;
    }
}

//void SendPacketMadebuildings()
//{
//    PacketBuildingMove building;
//    building.size = sizeof(PacketBuildingMove);
//    building.type = PACKET_BUILDING_MOVE;
//
//
//    for (int i = 0; i < 2; ++i) {
//        recv(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
//        if (building.pos.z < 0.f) {
//            MakeBuildings();
//            send(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
//        }
//    }
//
//
//}

void SendPacketMoveBuildings() {
    WaitForSingleObject(DataEvent, INFINITE);

    for (auto& building : g_buildings) {
        // z�����θ� �̵�
        building.pos.z -= 0.3f;   // �ǹ� �̵��ӵ� ����

        // ��ġ ���� ��� (������)
        cout << "Building " << building.num << " Position: ("
            << building.pos.x << ", "
            << building.pos.y << ", "
            << building.pos.z << ")" << endl;

        // Ŭ���̾�Ʈ�� ����
        for (int i = 0; i < 2; ++i) {
            if (clientSockets[i] != INVALID_SOCKET) {
                char type = PACKET_BUILDING_MOVE;
                send(clientSockets[i], &type, sizeof(char), 0);
                send(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
            }
        }
    }

    SetEvent(DataEvent);
}

// ���� ��Ȯ�ϰ� �̿ϼ� 
void GoToInGame()
{
    if (!gameStarted) {
        cout << "���� ����!" << endl;
        gameStarted = true;

        // ���� ���� ��Ŷ ����
        for (int i = 0; i < 2; i++) {
            if (clientSockets[i] != INVALID_SOCKET) {
                char type = CLIENT_ALL_READY;
                send(clientSockets[i], &type, sizeof(char), 0);

                PacketAllReady packet;
                packet.size = sizeof(PacketAllReady);
                packet.type = CLIENT_ALL_READY;
                send(clientSockets[i], (char*)&packet, sizeof(packet), 0);
            }
        }

        // ������Ʈ ������ ����
        SetEvent(UpdateEvent);
    }
}


// ���� ������
DWORD WINAPI ProcessClient(LPVOID arg) 
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    WaitForSingleObject(DataEvent, INFINITE);
    int ClientNum = nowID++;
    SetEvent(DataEvent);

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);
        retval = recv(client_sock, &type, sizeof(type), 0);
        if (retval <= 0) break;

        switch (type) {
        case CLIENT_READY: {
            ReadyClientToServer readyPacket;
            retval = recv(client_sock, (char*)&readyPacket, sizeof(ReadyClientToServer), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�غ� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                break;
            }

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = true;
            cout << "�÷��̾� " << ClientNum + 1 << " : �غ�Ϸ�" << endl;

            // ��� Ŭ���̾�Ʈ�� �غ�Ǿ����� Ȯ��
            bool allReady = true;
            for (int i = 0; i < 2; i++) {
                if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
                    allReady = false;
                    break;
                }
            }

            // ��� Ŭ���̾�Ʈ�� �غ�Ǿ��ٸ� ���� ����
            if (allReady) {
                cout << "\n��� �÷��̾ �غ�Ǿ����ϴ�!" << endl;
                cout << "������ �����մϴ�..." << endl;

                PacketAllReady readyPacket;
                readyPacket.size = sizeof(PacketAllReady);
                readyPacket.type = CLIENT_ALL_READY;

                for (int i = 0; i < 2; i++) {
                    if (clientSockets[i] != INVALID_SOCKET) {
                        char type = CLIENT_ALL_READY;
                        send(clientSockets[i], &type, sizeof(char), 0);
                        send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
                    }
                }
                gameStarted = true;
                SetEvent(UpdateEvent);
            }

            SetEvent(DataEvent);
            cout << "=== �غ� ��Ŷ ó�� �Ϸ� ===" << endl;
            break;
        }
        case PACKET_PLAYER_MOVE: {
            PacketPlayerMove movePacket;
            retval = recvn(client_sock, (char*)&movePacket, sizeof(movePacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }

            // Update Player
            Player[ClientNum] = movePacket;

            break;
        }
        case PACKET_BULLET_MOVE: {
            PacketBulletMove movePacket;
            retval = recvn(client_sock, (char*)&movePacket, sizeof(movePacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }

            // Update Bullet
            Bullet[ClientNum][movePacket.num] = movePacket;

            break;
        }
        }

        if (ClientNum < 2) {
            SetEvent(ClientEvent[(ClientNum + 1) % 2]);
        }
    }

    // ���� ���� ó��
    WaitForSingleObject(DataEvent, INFINITE);
    readyState[ClientNum] = false;
    closesocket(client_sock);
    clientSockets[ClientNum] = INVALID_SOCKET;
    nowID--;
    SetEvent(DataEvent);

    return 0;
}

static DWORD lastBuildingTime = GetTickCount();


// ������Ʈ ������
DWORD WINAPI ProcessUpdate(LPVOID arg) 
{
    while (true) {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 1. �ǹ� ���� - 3�ʸ��� ����
        DWORD currentTime = GetTickCount64();
        if (currentTime - lastBuildingTime >= 10000) {
            MakeBuildings();
            lastBuildingTime = currentTime;
        }

        // 2. ���� �̵� ó��
        SendPacketMoveBuildings();

        // 3. �÷��̾�/�Ѿ�/���� �̵� ó��
        ProcessMove();

        // 3. �浹 üũ
        //if (ColidePlayerToObjects()) DeleteObjects();
        ColideBulletToObjects();

        SetEvent(ClientEvent[0]);
        SetEvent(UpdateEvent);
    }
    return 0;
}


int main() {
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    // �̺�Ʈ ���� �� Ȯ��
    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DataEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    if (!ClientEvent[0] || !ClientEvent[1] || !UpdateEvent || !DataEvent) {
        WSACleanup();
        return 1;
    }


    // ���� ���� ���� �� ����
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // ���� �ּ� ����
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);

    bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    listen(listen_sock, SOMAXCONN);

    // ������Ʈ ������ ����
    CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);

    // Ŭ���̾�Ʈ ���� ó��
    while (true) {
        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) break;

        WaitForSingleObject(DataEvent, INFINITE);
        if (nowID >= 2) {
            SetEvent(DataEvent);
            closesocket(client_sock);
            continue;
        }

        clientSockets[nowID] = client_sock;
        SetEvent(DataEvent);

        CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
    }

    // ����
    closesocket(listen_sock);
    WSACleanup();

    return 0;
}