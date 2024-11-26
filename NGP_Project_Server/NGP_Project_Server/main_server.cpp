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

// Lobby
void SendReadyServerToClient() {
    // ��� Ŭ���̾�Ʈ���� ���� �غ� ���� ����
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;

    WaitForSingleObject(DataEvent, INFINITE);
    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            // ���� Ÿ�� ����
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
            cout << "Ŭ���̾�Ʈ " << i << "���� �غ� ���� ���� �Ϸ�" << endl;
        }
    }
    SetEvent(DataEvent);
}

void SendReadyCompleteServerToClient() {
    WaitForSingleObject(DataEvent, INFINITE);

    cout << "\n=== ���� ���� ��ȣ ���� ���� ===" << endl;

    // ��� �÷��̾ �غ�Ǿ����� Ȯ��
    bool allReady = true;
    for (int i = 0; i < 2; i++) {
        if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        cout << "��� �÷��̾� �غ� �Ϸ� Ȯ��" << endl;

        // ��� Ŭ���̾�Ʈ���� ���� ���� ���� ���� ����
        AllReady readyPacket;
        readyPacket.size = sizeof(AllReady);
        readyPacket.type = CLIENT_ALL_READY;

        for (int i = 0; i < 2; i++) {
            if (clientSockets[i] != INVALID_SOCKET) {
                char type = CLIENT_ALL_READY;
                int retval = send(clientSockets[i], &type, sizeof(char), 0);
                if (retval == SOCKET_ERROR) {
                    cout << "�غ� �Ϸ� Ÿ�� ���� ���� (Ŭ���̾�Ʈ " << i << "): " << WSAGetLastError() << endl;
                    continue;
                }

                retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
                if (retval == SOCKET_ERROR) {
                    cout << "�غ� �Ϸ� ��Ŷ ���� ���� (Ŭ���̾�Ʈ " << i << "): " << WSAGetLastError() << endl;
                    continue;
                }
                cout << "Ŭ���̾�Ʈ " << i << "���� ���� ���� ��ȣ ���� �Ϸ�" << endl;
            }
        }

        gameStarted = true;
        cout << "\n=== ���� ����! ===" << endl;
        GoToInGame();
    }

    SetEvent(DataEvent);
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

//void ProcessMove()
//{
//    for (int i = 0; i < 2; ++i) {
//        if (Player[i].state != 0) {
//            //�Ѿ�, �ǹ�, �÷��̾��� �����ӿ� ���� ��ǥ�� ����, ó���ϴ� �Լ�
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
//            recvn(clientSockets[i], (char*)&bullet, sizeof(PacketBulletMove), 0);
//            recvn(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
//            recvn(clientSockets[i], (char*)&player, sizeof(PacketPlayerMove), 0);
//
//            Player[i].pos.x += player.pos.x;
//            Player[i].pos.y += player.pos.y;
//            Player[i].pos.z += player.pos.z;
//        }
//    }
//}

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


// ���� ��Ȯ�ϰ� �̿ϼ� 
void GoToInGame()
{
    if (!gameStarted) {
        gameStarted = true;
        for (int i = 0; i < 2; i++) {
            Player[i].state = 0;
            readyState[i] = false;
        }
        MakeBuildings();
        SetEvent(UpdateEvent);
    }
}


// Ŭ���̾�Ʈ ������
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    // ClientNum ���� �κ� ����
    WaitForSingleObject(DataEvent, INFINITE);
    int ClientNum = nowID;
    nowID++;  // Ŭ���̾�Ʈ �� ����
    SetEvent(DataEvent);

    cout << "Ŭ���̾�Ʈ " << ClientNum << " ó�� ����" << endl;

    // ������ ���ŷ ���� ����
    u_long mode = 0;  // 0�� ���ŷ ��� , 1�� �� ���ŷ ���
    ioctlsocket(client_sock, FIONBIO, &mode);


    // Ŭ���̾�Ʈ ���� ���� �� ó��
    auto cleanup = [&]() {
        WaitForSingleObject(DataEvent, INFINITE);
        readyState[ClientNum] = false;
        if (clientSockets[ClientNum] != INVALID_SOCKET) {
            shutdown(clientSockets[ClientNum], SD_BOTH);
            closesocket(clientSockets[ClientNum]);
            clientSockets[ClientNum] = INVALID_SOCKET;
        }
        nowID--;
        SetEvent(DataEvent);
        SendReadyServerToClient();
        };

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

        // Ÿ�� ���� �õ�
        retval = recv(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                Sleep(100);  // ��� ���
                continue;
            }
            else {
                cout << "Ÿ�� ���� ����: " << err << endl;
                break;
            }
        }
        else if (retval == 0) {
            cout << "Ŭ���̾�Ʈ ���� ����" << endl;
            break;
        }

        //cout << "\n=== Ŭ���̾�Ʈ " << ClientNum << " ��Ŷ ó�� ���� ===" << endl;
        switch (type) {
            /* case PACKET_ID: {
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
             break;*/
        case CLIENT_READY: {
            ReadyClientToServer readyPacket;
            retval = recv(client_sock, (char*)&readyPacket, sizeof(ReadyClientToServer), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�غ� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                break;
            }

            cout << "�غ� ��Ŷ ���� ���� (ũ��: " << retval << " bytes)" << endl;

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = true;

            // ��� Ŭ���̾�Ʈ���� ����
            for (int i = 0; i < 2; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    char type = CLIENT_READY;
                    send(clientSockets[i], &type, sizeof(char), 0);
                    readyPacket.id = ClientNum;
                    send(clientSockets[i], (char*)&readyPacket, sizeof(ReadyClientToServer), 0);
                    cout << "Ŭ���̾�Ʈ " << i << "���� �غ� ���� ���� �Ϸ�" << endl;
                }
            }

            SetEvent(DataEvent);
            cout << "=== �غ� ��Ŷ ó�� �Ϸ� ===" << endl;
            break;
        }
        case CLIENT_NOTREADY: {
            NotReadyClientToServer notReadyPacket;
            retval = recv(client_sock, (char*)&notReadyPacket, sizeof(NotReadyClientToServer), 0);
            if (retval == SOCKET_ERROR) {
                cout << "�غ� ���� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                break;
            }

            cout << "Ŭ���̾�Ʈ " << ClientNum << "�κ��� �غ� ���� ��Ŷ ���� ����" << endl;

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = false;
            cout << "Ŭ���̾�Ʈ " << ClientNum << "�� �غ� ���¸� false�� ����" << endl;

            // ���� ��� Ŭ���̾�Ʈ�� �غ� ���� ���
            cout << "���� �غ� ����: ";
            for (int i = 0; i < 2; i++) {
                cout << "�÷��̾�" << i << ": " << (readyState[i] ? "�غ��" : "�غ�ȵ�") << " ";
            }
            cout << endl;

            // ��� Ŭ���̾�Ʈ���� �غ� ���� ���� ����
            char type = CLIENT_NOTREADY;
            for (int i = 0; i < 2; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    // Ÿ�� ����
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�غ� ���� Ÿ�� ���� ���� (Ŭ���̾�Ʈ " << i << "): " << WSAGetLastError() << endl;
                        continue;
                    }
                    cout << "Ŭ���̾�Ʈ " << i << "���� �غ� ���� Ÿ�� ���� ����" << endl;

                    // ��Ŷ ����
                    notReadyPacket.id = ClientNum;  // � �÷��̾ �غ� �����ߴ��� ǥ��
                    retval = send(clientSockets[i], (char*)&notReadyPacket, sizeof(notReadyPacket), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�غ� ���� ��Ŷ ���� ���� (Ŭ���̾�Ʈ " << i << "): " << WSAGetLastError() << endl;
                        continue;
                    }
                    cout << "Ŭ���̾�Ʈ " << i << "���� �غ� ���� ��Ŷ ���� ���� (�غ� ������ �÷��̾�: " << ClientNum << ")" << endl;
                }
            }
            SetEvent(DataEvent);
            break;
        }
        case PACKET_PLAYER_MOVE: {
            PacketPlayerMove movePacket;
            retval = recvn(client_sock, (char*)&movePacket, sizeof(movePacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            cout << "[" << ClientNum << "] " << movePacket.pos.x << ", " << movePacket.pos.y << ", " << movePacket.pos.z <<"(" << (int)movePacket.state << ")" << endl;
            // ���� �÷��̾��� ��ġ ������Ʈ
            Player[ClientNum] = movePacket;

            // ��� Ŭ���̾�Ʈ���� ��ġ ���� ����
            for (int i = 0; i < 2; i++) {
                if (i != ClientNum && clientSockets[i] != INVALID_SOCKET) {
                    // ��Ŷ Ÿ�� ����
                    char type = PACKET_PLAYER_MOVE;
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�÷��̾� �̵� Ÿ�� ���� ����: " << WSAGetLastError() << endl;
                        continue;
                    }

                    // ��ġ ���� ��Ŷ ����
                    retval = send(clientSockets[i], (char*)&movePacket, sizeof(movePacket), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "�÷��̾� �̵� ��Ŷ ���� ����: " << WSAGetLastError() << endl;
                       continue;
                    }

                    //cout << "Ŭ���̾�Ʈ " << i << "���� �÷��̾� " << ClientNum << "�� ��ġ ���� �Ϸ�" << endl;
                }
            }
            break;
        }
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
        //ProcessMove();

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


int main() {
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "���� �ʱ�ȭ ����" << endl;
        return 1;
    }
    cout << "���� �ʱ�ȭ ����" << endl;

    // �̺�Ʈ ���� �� Ȯ��
    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DataEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    if (!ClientEvent[0] || !ClientEvent[1] || !UpdateEvent || !DataEvent) {
        cout << "�̺�Ʈ ���� ����" << endl;
        return 1;
    }
    cout << "�̺�Ʈ ���� ����" << endl;

    // ���� ����
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        cout << "������ ���� ���� ����: " << WSAGetLastError() << endl;
        err_quit("listen socket()");
    }
    cout << "������ ���� ���� ����" << endl;

    // ���� �ּ� ����
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);

    // ���ε�
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "���ε� ����: " << WSAGetLastError() << endl;
        err_quit("bind()");
    }
    cout << "���ε� ���� (��Ʈ: " << TCPPORT << ")" << endl;

    // ������
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        cout << "������ ����: " << WSAGetLastError() << endl;
        err_quit("listen()");
    }
    cout << "������ ����" << endl;

    // ������Ʈ ������ ����
    HANDLE hThread = CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);
    if (hThread != NULL) {
        CloseHandle(hThread);
        cout << "������Ʈ ������ ���� ����" << endl;
    }
    else {
        cout << "������Ʈ ������ ���� ����" << endl;
    }

    // Ŭ���̾�Ʈ ���� ó��
    while (true) {
        cout << "\n���� ���ӵ� Ŭ���̾�Ʈ ��: " << nowID << endl;
        cout << "�غ� ����: ";
        for (int i = 0; i < 2; i++) {
            cout << "�÷��̾�" << i << ": " << (readyState[i] ? "�غ��" : "�غ�ȵ�") << " ";
        }
        cout << endl;

        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) {
            cout << "Ŭ���̾�Ʈ ���� ����: " << WSAGetLastError() << endl;
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
        cout << "Ŭ���̾�Ʈ " << nowID << " ���� ���� �Ϸ�" << endl;
        SetEvent(DataEvent);

        HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
            cout << "������ ���� ����" << endl;
        }
        else {
            CloseHandle(hThread);
            cout << "Ŭ���̾�Ʈ " << nowID - 1 << " ������ ���� ����" << endl;
        }
    }

    // ����
    cout << "���� ���� ��..." << endl;
    closesocket(listen_sock);
    WSACleanup();
    cout << "���� ���� �Ϸ�" << endl;

    return 0;
}