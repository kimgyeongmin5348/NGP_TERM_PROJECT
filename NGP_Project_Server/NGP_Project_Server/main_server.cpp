#include "framework.h"
#include "PacketDefine.h"

using namespace std;

// 전역 변수 선언
PacketPlayerMove Player[2];
int nowID = 0;
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;
SOCKET clientSockets[2];
bool readyState[2] = { false, false };
bool gameStarted = false;

// 함수 선언
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
//    cout << "ID 저장 완료: " << NewID << endl;
//}
//
//bool CheckID(const char* playerid) {
//    // DataEvent 대기는 제거 (불필요한 동기화 제거)
//    for (int i = 0; i < nowID; ++i) {
//        if (strcmp(Player[i].playerid, playerid) == 0) {
//            cout << "중복된 ID: " << playerid << endl;
//            return false;
//        }
//    }
//    cout << "사용 가능한 ID: " << playerid << endl;
//    return true;
//}

// Lobby 관련 함수
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
                cout << "준비 상태 타입 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
            readyPacket.id = i;
            retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                cout << "준비 상태 패킷 전송 실패: " << WSAGetLastError() << endl;
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
//        cout << "\n모든 플레이어가 준비되었습니다!" << endl;
//        cout << "게임을 시작합니다..." << endl;
//
//        AllReady readyPacket;
//        readyPacket.size = sizeof(AllReady);
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
//}


// InGame 관련 함수
void MakeBuildings() 
{
    WaitForSingleObject(DataEvent, INFINITE);

    PacketBuildingMove buildingPacket;
    buildingPacket.size = sizeof(PacketBuildingMove);
    buildingPacket.type = PACKET_BUILDING_MOVE;

    vector<PacketBuildingMove> buildings;

    for (int i = 0; i < 100; ++i) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> random_x(-20, 20);
        uniform_real_distribution<float> random_height(1, 25);

        buildingPacket.pos.x = random_x(gen);
        buildingPacket.pos.y = 0;
        buildingPacket.pos.z = 20.f;
        buildingPacket.scale.x = 2.0f;
        buildingPacket.scale.y = random_height(gen);
        buildingPacket.scale.z = 4.0f;
        buildingPacket.num = i;
        buildingPacket.is_broken = false;
        buildings.push_back(buildingPacket);
    }

    for (int k = 0; k < 2; ++k) {     
        for (const auto& building : buildings) {
            char type = PACKET_BUILDING_MOVE;
            send(clientSockets[k], &type, sizeof(char), 0);
            send(clientSockets[k], (char*)&building, sizeof(PacketBuildingMove), 0);
        }
    }
    SetEvent(DataEvent);
}

void ProcessMove() 
{
    WaitForSingleObject(DataEvent, INFINITE);

    // Update Building 
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
                //send(clientSockets[j], &type, sizeof(char), 0);
                //send(clientSockets[j], (char*)&buildingPacket, sizeof(buildingPacket), 0);
            }
        }
    }

    // Update Player - ProcessClient() 에서 전역변수 Player에 저장하는 것으로 대체
    int retval;

    // Send PacketPlayerMove
    PacketPlayerMove ppm;
    ZeroMemory(&ppm, sizeof(PacketPlayerMove));
    ppm.size = sizeof(PacketPlayerMove);
    ppm.type = PACKET_PLAYER_MOVE;

    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            char type = PACKET_PLAYER_MOVE;
            retval = send(clientSockets[i], &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR) {
                cout << "플레이어 이동 타입 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
            retval = send(clientSockets[i], (char*)&Player[i], sizeof(PacketPlayerMove), 0);
            if (retval == SOCKET_ERROR) {
                cout << "플레이어 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
            cout << "ClientSocket [" << i << "] : " << Player[i].pos << endl;
        }
    }


    SetEvent(DataEvent);
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

// 아직 정확하게 미완성 
void GoToInGame()
{
    if (!gameStarted) {
        cout << "게임 시작!" << endl;
        gameStarted = true;

        // 게임 시작 패킷 전송
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

        // 업데이트 스레드 시작
        SetEvent(UpdateEvent);
    }
}


// 메인 스레드
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
                cout << "준비 패킷 수신 실패: " << WSAGetLastError() << endl;
                break;
            }

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = true;
            cout << "플레이어 " << ClientNum + 1 << " : 준비완료" << endl;

            // 모든 클라이언트가 준비되었는지 확인
            bool allReady = true;
            for (int i = 0; i < 2; i++) {
                if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
                    allReady = false;
                    break;
                }
            }

            // 모든 클라이언트가 준비되었다면 게임 시작
            if (allReady) {
                cout << "\n모든 플레이어가 준비되었습니다!" << endl;
                cout << "게임을 시작합니다..." << endl;

                PacketAllReady readyPacket;
                readyPacket.size = sizeof(PacketAllReady);
                readyPacket.type = CLIENT_ALL_READY;

                for (int i = 0; i < 2; i++) {
                    if (clientSockets[i] != INVALID_SOCKET) {
                        char type = CLIENT_ALL_READY;
                        send(clientSockets[i], &type, sizeof(char), 0);
                        send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);

                        cout << "ready packet 전송 성공 : Client " << i << endl;
                    }
                }
                gameStarted = true;
                SetEvent(UpdateEvent);
            }

            SetEvent(DataEvent);
            cout << "=== 준비 패킷 처리 완료 ===" << endl;
            break;
        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }

        if (ClientNum < 2) {
            SetEvent(ClientEvent[(ClientNum + 1) % 2]);
        }
    }

    // 연결 종료 처리
    WaitForSingleObject(DataEvent, INFINITE);
    readyState[ClientNum] = false;
    closesocket(client_sock);
    clientSockets[ClientNum] = INVALID_SOCKET;
    nowID--;
    SetEvent(DataEvent);

    return 0;
}


// 업데이트 스레드
DWORD WINAPI ProcessUpdate(LPVOID arg) 
{
    while (true) {
        cout << "ProcessUpdate" << endl;
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 1. 건물 생성
        MakeBuildings();

        // 2. 플레이어/총알/빌딩 이동 처리
        ProcessMove();

        // 3. 충돌 체크
        if (ColidePlayerToObjects()) DeleteObjects();
        if (ColideBulletToObjects()) DeleteObjects();

        SetEvent(ClientEvent[0]);
        SetEvent(UpdateEvent);
    }
    return 0;
}


int main() {
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    // 이벤트 생성 및 확인
    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DataEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    if (!ClientEvent[0] || !ClientEvent[1] || !UpdateEvent || !DataEvent) {
        WSACleanup();
        return 1;
    }


    // 서버 소켓 생성 및 설정
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);

    bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    listen(listen_sock, SOMAXCONN);

    // 업데이트 스레드 생성
    CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);

    // 클라이언트 접속 처리
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

    // 정리
    closesocket(listen_sock);
    WSACleanup();

    return 0;
}