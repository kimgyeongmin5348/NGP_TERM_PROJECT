#include "framework.h"
#include "PacketDefine.h"

using namespace std;

// 전역 변수 선언
PacketPlayerMove Player[2];
PacketBulletMove Bullet[2][3]; // 클라마다 3개
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

struct UserInfo {
    string username;
    string password;
    bool isLoggedIn;
};

vector<UserInfo> userDatabase;

// 함수 선언
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
                std::cout << "준비 상태 타입 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
            readyPacket.id = i;
            retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "준비 상태 패킷 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
        }
    }
    SetEvent(DataEvent);
}


// InGame 관련 함수
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
            // 클라이언트에 전송
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
    // SendPacketMoveBuilding() 에서 처리   

    /**************
      Update Player
    ***************/
    //ProcessClient() 에서 전역변수 Player에 저장하는 것으로 대체


    /**************
      Update Bullet
    ***************/
    //ProcessClient() 에서 전역변수 Bullet에 저장하는 것으로 대체


    /***********************
      Send PacketPlayerMove
    ************************/
    int retval;
    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            char type = PACKET_PLAYER_MOVE;
            retval = send(clientSockets[i], &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR) {
                cout << "플레이어 이동 타입 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
            retval = send(clientSockets[i], (char*)&Player[(i + 1) % 2], sizeof(PacketPlayerMove), 0);
            if (retval == SOCKET_ERROR) {
                cout << "플레이어 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }
        }
    }

    /**********************
      Send PacketBulletMove
    ***********************/
    // 다른 클라이언트에게 전송
    for (int i = 0; i < 2; i++) {
        for (int bulletIndex = 0; bulletIndex < 3; bulletIndex++) {
            if (clientSockets[i] != INVALID_SOCKET) {
                if (Bullet[(i + 1) % 2][bulletIndex].active){
                    char type = PACKET_BULLET_MOVE;
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "총알 이동 타입 전송 실패: " << WSAGetLastError() << endl;
                        continue;
                    }

                    retval = send(clientSockets[i], (char*)&Bullet[(i + 1) % 2][bulletIndex], sizeof(PacketBulletMove), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "총알 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
                        continue;
                    }

                    //cout << i << "에게 송신 " << Bullet[(i + 1) % 2][bulletIndex].num << "-" << Bullet[(i + 1) % 2][bulletIndex].pos << endl;
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

void ColideBulletToObjects() {
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

                    //cout << "충돌 발생!" << endl;
                    //cout << "건물 번호: " << j << ", 총알 번호: " << i << endl;
                    //cout << "충돌 위치: (" << Bullet[k][i].pos.x << ", "
                    //    << Bullet[k][i].pos.y << ", "
                    //    << Bullet[k][i].pos.z << ")" << endl;

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


void SendPacketMoveBuildings() {
    WaitForSingleObject(DataEvent, INFINITE);

    for (auto& building : g_buildings) {
        // z축으로만 이동
        building.pos.z -= 0.05f;   // 건물 이동속도 조절

        // 위치 정보 출력 (디버깅용)
        //cout << "Building " << building.num << " Position: ("
        //    << building.pos.x << ", "
        //    << building.pos.y << ", "
        //    << building.pos.z << ")" << endl;

        // 클라이언트에 전송
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

// 메인 스레드
DWORD WINAPI ProcessClient(LPVOID arg) 
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    // TCP_NODELAY 설정 추가
    int flag = 1;
    setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));

    WaitForSingleObject(DataEvent, INFINITE);
    int ClientNum = nowID++;
    SetEvent(DataEvent);

    while (true) {
        //WaitForSingleObject(ClientEvent[ClientNum], INFINITE);
        retval = recv(client_sock, &type, sizeof(type), 0);
        if (retval <= 0) break;

        switch (type) {
        case PACKET_LOGIN_REQUEST: {
            PacketLoginRequest loginPacket;
            retval = recvn(client_sock, (char*)&loginPacket, sizeof(PacketLoginRequest), 0);
            cout << "로그인정보 받음" << '\n';
            if (retval == SOCKET_ERROR) break;

            PacketLoginResponse response;
            response.size = sizeof(PacketLoginResponse);
            response.type = PACKET_LOGIN_RESPONSE;
            response.success = true;  // 임시로 모든 로그인 허용
            response.userID = ClientNum;

            send(client_sock, (char*)&response, sizeof(response), 0);
            cout << "클라이언트 " << ClientNum << " 로그인 성공" << endl;
            break;
        }
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
                    }
                }
                start_time = chrono::high_resolution_clock::now();
                gameStarted = true;
                SetEvent(UpdateEvent);
            }

            SetEvent(DataEvent);
            cout << "=== 준비 패킷 처리 완료 ===" << endl;
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

            // 총알 인덱스 범위 체크 추가
            if (movebulletPacket.num < 0 || movebulletPacket.num >= 30) {
                cout << "잘못된 총알 인덱스 수신: " << movebulletPacket.num << endl;
                break;
            }

            // Update Bullet
            Bullet[ClientNum][movebulletPacket.num] = movebulletPacket;

            break;
        }
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

static DWORD lastBuildingTime = GetTickCount();


// 업데이트 스레드
DWORD WINAPI ProcessUpdate(LPVOID arg) 
{
    while (true) {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 1. 건물 생성 - 3초마다 실행
        DWORD currentTime = GetTickCount64();
        if (currentTime - lastBuildingTime >= 10000) {
            MakeBuildings();
            lastBuildingTime = currentTime;
        }

        // 2. 빌딩 이동 처리
        SendPacketMoveBuildings();

        // 3. 플레이어/총알/빌딩 이동 처리
        ProcessMove();

        // 3. 충돌 체크
        ColidePlayerToObjects();
        ColideBulletToObjects();

        //SetEvent(ClientEvent[0]);
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