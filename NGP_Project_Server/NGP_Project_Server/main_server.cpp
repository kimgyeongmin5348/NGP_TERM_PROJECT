#include "framework.h"
#include "PacketDefine.h"

using namespace std;

// ���� ���� ����
PacketPlayerMove Player[2];
int nowID = 0;
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;
SOCKET clientSockets[2];
bool readyState[2] = { false, false };
bool gameStarted = false;

// �Լ� ����
//void SaveID();
void GoToInGame();
void MakeBuildings();
void ProcessMove();
BOOL ColidePlayerToObjects();
BOOL ColideBulletToObjects();
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
void SendReadyServerToClient() {
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

void SendReadyCompleteServerToClient() {
    WaitForSingleObject(DataEvent, INFINITE);

    bool allReady = true;
    for (int i = 0; i < 2; i++) {
        if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        cout << "\n��� �÷��̾ �غ�Ǿ����ϴ�!" << endl;
        cout << "������ �����մϴ�..." << endl;

        AllReady readyPacket;
        readyPacket.size = sizeof(AllReady);
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
}


// InGame ���� �Լ�
void MakeBuildings() {
    PacketBuildingMove buildingPacket;
    buildingPacket.size = sizeof(PacketBuildingMove);
    buildingPacket.type = PACKET_BUILDING_MOVE;

    WaitForSingleObject(DataEvent, INFINITE);
    for (int k = 0; k < 2; ++k) {
        vector<PacketBuildingMove> buildings;
        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 10; ++j) {
                random_device rd;
                mt19937 gen(rd());
                uniform_real_distribution<float> random_x(-20, 20);
                uniform_real_distribution<float> random_height(1, 25);

                buildingPacket.pos.x = random_x(gen);
                buildingPacket.pos.y = 0;
                buildingPacket.pos.z = 20.f * (j + 1);
                buildingPacket.num = i;
                buildingPacket.is_broken = false;
                buildings.push_back(buildingPacket);
            }
        }

        for (const auto& building : buildings) {
            send(clientSockets[k], (char*)&building, sizeof(PacketBuildingMove), 0);
        }
    }
    SetEvent(DataEvent);
}

void ProcessMove() {
    WaitForSingleObject(DataEvent, INFINITE);

    // buildingPacket�� �� ���� ����
    PacketBuildingMove buildingPacket;
    ZeroMemory(&buildingPacket, sizeof(PacketBuildingMove));
    buildingPacket.size = sizeof(PacketBuildingMove);
    buildingPacket.type = PACKET_BUILDING_MOVE;

    for (int i = 0; i < 10; ++i) {
        buildingPacket.num = i;
        buildingPacket.pos.z -= 0.1f;

        if (buildingPacket.pos.z < 0.0f) {
            buildingPacket.pos.x = (float)(rand() % 41 - 20);
            buildingPacket.pos.z = 200.0f;
        }

        for (int j = 0; j < 2; ++j) {
            if (clientSockets[j] != INVALID_SOCKET) {
                char type = PACKET_BUILDING_MOVE;
                send(clientSockets[j], &type, sizeof(char), 0);
                send(clientSockets[j], (char*)&buildingPacket, sizeof(buildingPacket), 0);
            }
        }
    }

    SetEvent(DataEvent);
}

//void ProcessMove() {
//    for (int i = 0; i < 2; ++i) {
//        if (Player[i].state != 0) {
//            cout << "\n�÷��̾� " << i << " �̵� ó�� ����" << endl;
//
//            PacketBulletMove bullet;
//            bullet.size = sizeof(PacketBulletMove);
//            bullet.type = PACKET_BULLET_MOVE;
//
//            PacketBuildingMove building;
//            building.size = sizeof(PacketBuildingMove);
//            building.type = PACKET_BUILDING_MOVE;
//
//            PacketPlayerMove player;
//            player.size = sizeof(PacketPlayerMove);
//            player.type = PACKET_PLAYER_MOVE;
//
//            // ��Ŷ ����
//            int retval;
//
//            retval = recvn(clientSockets[i], (char*)&bullet, sizeof(PacketBulletMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "�Ѿ� ��Ŷ ���� ���� (�÷��̾� " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "�Ѿ� ��Ŷ ���� ���� (�÷��̾� " << i << ")" << endl;
//
//            retval = recvn(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "�ǹ� ��Ŷ ���� ���� (�÷��̾� " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "�ǹ� ��Ŷ ���� ���� (�÷��̾� " << i << ")" << endl;
//
//            retval = recvn(clientSockets[i], (char*)&player, sizeof(PacketPlayerMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "�÷��̾� ��Ŷ ���� ���� (�÷��̾� " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "�÷��̾� ��Ŷ ���� ���� (�÷��̾� " << i << ")" << endl;
//
//            // ��ġ ������Ʈ �� ��ġ ���
//            cout << "�÷��̾� " << i << " �̵� �� ��ġ: ("
//                << Player[i].pos.x << ", " << Player[i].pos.y << ", " << Player[i].pos.z << ")" << endl;
//
//            // ��ġ ������Ʈ
//            Player[i].pos.x += player.pos.x;
//            Player[i].pos.y += player.pos.y;
//            Player[i].pos.z += player.pos.z;
//
//            // ������Ʈ �� ��ġ ���
//            cout << "�÷��̾� " << i << " �̵� �� ��ġ: ("
//                << Player[i].pos.x << ", " << Player[i].pos.y << ", " << Player[i].pos.z << ")" << endl;
//
//            // ������Ʈ�� ��ġ ������ ��� Ŭ���̾�Ʈ���� ����
//            for (int j = 0; j < 2; ++j) {
//                if (clientSockets[j] != INVALID_SOCKET) {
//                    cout << "\nŬ���̾�Ʈ " << j << "���� ������ ���� ����" << endl;
//
//                    // �Ѿ� ���� ����
//                    char type = PACKET_BULLET_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�Ѿ� Ÿ�� ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    retval = send(clientSockets[j], (char*)&bullet, sizeof(bullet), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�Ѿ� ��Ŷ ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "�Ѿ� ���� ���� �Ϸ� (Ŭ���̾�Ʈ " << j << ")" << endl;
//
//                    // �ǹ� ���� ����
//                    type = PACKET_BUILDING_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�ǹ� Ÿ�� ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    retval = send(clientSockets[j], (char*)&building, sizeof(building), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�ǹ� ��Ŷ ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "�ǹ� ���� ���� �Ϸ� (Ŭ���̾�Ʈ " << j << ")" << endl;
//
//                    // �÷��̾� ���� ����
//                    type = PACKET_PLAYER_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�÷��̾� Ÿ�� ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    Player[i].id = i;  // �÷��̾� ID ����
//                    retval = send(clientSockets[j], (char*)&Player[i], sizeof(PacketPlayerMove), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "�÷��̾� ��Ŷ ���� ���� (Ŭ���̾�Ʈ " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "�÷��̾� ���� ���� �Ϸ� (Ŭ���̾�Ʈ " << j << ")" << endl;
//                }
//            }
//            cout << "�÷��̾� " << i << " �̵� ó�� �Ϸ�\n" << endl;
//        }
//    }
//}

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

//void SendPacketMoveBuildings()
//{
//    PacketBuildingMove building;
//    building.size = sizeof(PacketBuildingMove);
//    building.type = PACKET_BUILDING_MOVE;
//
//    for (int i = 0; i < 2; ++i) {
//        send(clientSockets[i], (char*)&building.pos, sizeof(building.pos), 0);
//    }
//}

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

                AllReady packet;
                packet.size = sizeof(AllReady);
                packet.type = CLIENT_ALL_READY;
                send(clientSockets[i], (char*)&packet, sizeof(packet), 0);
            }
        }

        // �ʱ� ���� ����
        MakeBuildings();

        // ������Ʈ ������ ����
        SetEvent(UpdateEvent);
    }
}


// ���� ������
DWORD WINAPI ProcessClient(LPVOID arg) {
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

                AllReady readyPacket;
                readyPacket.size = sizeof(AllReady);
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

            // ���� �÷��̾��� ��ġ ������Ʈ
            Player[ClientNum] = movePacket;
            Player[ClientNum].id = ClientNum;

            // ��� Ŭ���̾�Ʈ���� ��ġ ���� ����
            for (int i = 0; i < 2; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    char type = PACKET_PLAYER_MOVE;
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�÷��̾� �̵� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                        continue;
                    }
                    retval = send(clientSockets[i], (char*)&Player[ClientNum], sizeof(PacketPlayerMove), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�÷��̾� �̵� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                        continue;
                    }
                }
            }
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


// ������Ʈ ������
DWORD WINAPI ProcessUpdate(LPVOID arg) {
    while (true) {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 1. �ǹ� ���� �� �̵�
        MakeBuildings();

        // 2. �÷��̾�/�Ѿ�/���� �̵� ó��
        ProcessMove();

        // 3. �浹 üũ
        if (ColidePlayerToObjects()) DeleteObjects();
        if (ColideBulletToObjects()) DeleteObjects();

        SetEvent(ClientEvent[0]);
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