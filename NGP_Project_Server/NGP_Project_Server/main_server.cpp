#include "framework.h"
#include "PacketDefine.h"

using namespace std;

// ���� ���� ����
PacketPlayerMove Player[2];
PacketBulletMove Bullet[2][3]; // Ŭ�󸶴� 3��
PacketCollideBB  bb;
PacketCollidePB  pb;

int nowID = 0;
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;
SOCKET clientSockets[2];
bool readyState[2] = { false, false };
bool gameStarted = false;
vector<PacketBuildingMove> g_buildings(100);
auto start_time = chrono::high_resolution_clock::now();

// �Լ� ����
//void SaveID();
void MakeBuildings();
void ProcessMove();
void ColidePlayerToObjects();
void ColideBulletToObjects();
void DeleteObjects();
//void SendPacketMadebuildings();
void SendPacketMoveBuildings();

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
                std::cout << "�غ� ���� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
            readyPacket.id = i;
            retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "�غ� ���� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                continue;
            }
        }
    }
    SetEvent(DataEvent);
}


// InGame ���� �Լ�
void MakeBuildings()
{
    WaitForSingleObject(DataEvent, INFINITE);

    for (int i = 0; i < 100; ++i) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> random_x(-20, 20);
        uniform_real_distribution<float> random_height(1, 25);

        g_buildings[i].pos.x = random_x(gen);
        g_buildings[i].pos.y = 0;
        g_buildings[i].pos.z = 20.f;
        g_buildings[i].scale.x = 2.0f;
        g_buildings[i].scale.y = random_height(gen);
        g_buildings[i].scale.z = 4.0f;
        g_buildings[i].num = i;
        g_buildings[i].is_broken = false;

        for (int k = 0; k < 2; ++k) {
            // Ŭ���̾�Ʈ�� ����
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
    // SendPacketMoveBuilding() ���� ó��   

    /**************
      Update Player
    ***************/
    //ProcessClient() ���� �������� Player�� �����ϴ� ������ ��ü


    /**************
      Update Bullet
    ***************/
    //ProcessClient() ���� �������� Bullet�� �����ϴ� ������ ��ü


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
    for (int i = 0; i < 2; i++) {
        for (int bulletIndex = 0; bulletIndex < 3; bulletIndex++) {
            if (clientSockets[i] != INVALID_SOCKET) {
                if (Bullet[(i + 1) % 2][bulletIndex].active){
                    char type = PACKET_BULLET_MOVE;
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�Ѿ� �̵� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                        continue;
                    }

                    retval = send(clientSockets[i], (char*)&Bullet[(i + 1) % 2][bulletIndex], sizeof(PacketBulletMove), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�Ѿ� �̵� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                        continue;
                    }
                    //cout << i << "���� �۽� " << Bullet[(i + 1) % 2][bulletIndex].num << "-" << Bullet[(i + 1) % 2][bulletIndex].pos << endl;
                }
            }
        }
    }


    SetEvent(DataEvent);
}

void ColidePlayerToObjects()
{
    pb.size = sizeof(PacketCollidePB);
    pb.type = PACKET_COLLIDE_PLAYER_BUILDING;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < g_buildings.size(); ++j) {
            if (g_buildings[j].pos.z < 0.4f && g_buildings[j].pos.z > -0.1f){
                if ((g_buildings[j].pos.x - 0.6f) < Player[i].pos.x &&
                    Player[i].pos.x < (g_buildings[j].pos.x + 0.6f) &&
                    Player[i].pos.y < g_buildings[j].scale.y / 5 - 0.2f) {

                    pb.num = i;
                        
                    for (int c = 0; c < 2; ++c) {
                        if (clientSockets[c] != INVALID_SOCKET) {
                            send(clientSockets[c], &pb.type, sizeof(char), 0);
                            send(clientSockets[c], (char*)&pb, sizeof(PacketCollidePB), 0);
                        }
                    }
                }
            }
        }
    }
}

void ColideBulletToObjects() 
{
    bb.size = sizeof(PacketCollideBB);
    bb.type = PACKET_COLLIDE_BULLET_BUILDING;
    bool collision = false;

    for (int k = 0; k < 2; ++k) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < g_buildings.size(); ++j) {
                if (g_buildings[j].pos.x - 0.6f < Bullet[k][i].pos.x &&
                    Bullet[k][i].pos.x < g_buildings[j].pos.x + 0.6f &&
                    Bullet[k][i].pos.y < g_buildings[j].scale.y / 5 - 0.2f &&
                    Bullet[k][i].pos.z < g_buildings[j].pos.z + 1 &&
                    Bullet[k][i].pos.z > g_buildings[j].pos.z - 1) {

                    //cout << "�浹 �߻�!" << endl;
                    //cout << "�ǹ� ��ȣ: " << j << ", �Ѿ� ��ȣ: " << i << endl;
                    //cout << "�浹 ��ġ: (" << Bullet[k][i].pos.x << ", "
                    //    << Bullet[k][i].pos.y << ", "
                    //    << Bullet[k][i].pos.z << ")" << endl;

                    cout << j  << " - " << k << "(" << i << ")" << Bullet[k][i].pos << endl;

                    bb.building_num = j;
                    bb.bullet_num = i;

                    for (int c = 0; c < 2; ++c) {
                        if (clientSockets[c] != INVALID_SOCKET) {
                            send(clientSockets[c], &bb.type, sizeof(char), 0);
                            send(clientSockets[c], (char*)&bb, sizeof(PacketCollideBB), 0);
                        }
                    }

                    g_buildings[j].is_broken = true;
                    g_buildings[j].scale.y = 0;
                    collision = true;
                    break;
                }
            }
            if (collision) break;
        }
        if (collision) break;
    }
}

void DeleteObjects()
{
    for (int k = 0; k < 2; ++k) {
        // g_buildings vector ���
        g_buildings[bb.building_num].scale.y = 0;
        g_buildings[bb.building_num].is_broken = true;

        char type = PACKET_BUILDING_MOVE;
        send(clientSockets[k], &type, sizeof(char), 0);
        // �ش� �ǹ��� ������ ����
        send(clientSockets[k], (char*)&g_buildings[bb.building_num], sizeof(PacketBuildingMove), 0);
    }
}

void SendPacketMoveBuildings() 
{
    WaitForSingleObject(DataEvent, INFINITE);

    for (auto& building : g_buildings) {
        // z�����θ� �̵�
        building.pos.z -= 0.05f;   // �ǹ� �̵��ӵ� ����

        // ��ġ ���� ��� (������)
        //cout << "Building " << building.num << " Position: ("
        //    << building.pos.x << ", "
        //    << building.pos.y << ", "
        //    << building.pos.z << ")" << endl;

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


DWORD WINAPI ProcessClient(LPVOID arg) 
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    // TCP_NODELAY ���� �߰�
    int flag = 1;
    setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));

    WaitForSingleObject(DataEvent, INFINITE);
    int ClientNum = nowID++;
    SetEvent(DataEvent);

    while (true) {
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
                start_time = chrono::high_resolution_clock::now();
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
            PacketBulletMove movebulletPacket;
            retval = recvn(client_sock, (char*)&movebulletPacket, sizeof(movebulletPacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            //cout << "����" << ClientNum << " - " << movebulletPacket.num << " - " << movebulletPacket.pos << endl;

            // Update Bullet
            Bullet[ClientNum][movebulletPacket.num] = movebulletPacket;

            break;
        }
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

        // 1. �ǹ� ���� - 5�ʸ��� ����
        DWORD currentTime = GetTickCount64();
        if (currentTime - lastBuildingTime >= 5000) {
            MakeBuildings();
            lastBuildingTime = currentTime;
        }

        // 2. ���� �̵� ó��
        SendPacketMoveBuildings();

        // 3. �÷��̾�/�Ѿ�/���� �̵� ó��
        ProcessMove();

        // 3. �浹 üũ
        ColideBulletToObjects();

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

    // �Ѿ� �ʱ� ��ġ �ʱ�ȭ
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            Bullet[i][j].pos = glm::vec3(1000.0f, 1000.0f, 1000.0f);
        }
    }

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