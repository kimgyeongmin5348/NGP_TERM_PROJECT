#include "framework.h"
#include "PacketDefine.h"

using namespace std;
PacketPlayerMove Player[2]; //플레이어 정보?
int nowID = 0; // 클라당 id
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
HANDLE DataEvent;  // 데이터 접근을 위한 이벤트
SOCKET clientSockets[2];  // 클라이언트 소켓 저장 배열

bool readyState[2] = { false, false }; // 각 플레이어의 준비 상태
bool gameStarted = false;

void GoToInGame();

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

// Lobby
void SendReadyServerToClient() {
    // 모든 클라이언트에게 현재 준비 상태 전송
    ReadyClientToServer readyPacket;
    readyPacket.size = sizeof(ReadyClientToServer);
    readyPacket.type = CLIENT_READY;

    WaitForSingleObject(DataEvent, INFINITE);
    for (int i = 0; i < 2; i++) {
        if (clientSockets[i] != INVALID_SOCKET) {
            // 먼저 타입 전송
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
            cout << "클라이언트 " << i << "에게 준비 상태 전송 완료" << endl;
        }
    }
    SetEvent(DataEvent);
}

void SendReadyCompleteServerToClient() {
    WaitForSingleObject(DataEvent, INFINITE);

    cout << "\n=== 게임 시작 신호 전송 시작 ===" << endl;

    // 모든 플레이어가 준비되었는지 확인
    bool allReady = true;
    for (int i = 0; i < 2; i++) {
        if (!readyState[i] || clientSockets[i] == INVALID_SOCKET) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        cout << "모든 플레이어 준비 완료 확인" << endl;

        // 모든 클라이언트에게 게임 시작 가능 상태 전송
        AllReady readyPacket;
        readyPacket.size = sizeof(AllReady);
        readyPacket.type = CLIENT_ALL_READY;

        for (int i = 0; i < 2; i++) {
            if (clientSockets[i] != INVALID_SOCKET) {
                char type = CLIENT_ALL_READY;
                int retval = send(clientSockets[i], &type, sizeof(char), 0);
                if (retval == SOCKET_ERROR) {
                    cout << "준비 완료 타입 전송 실패 (클라이언트 " << i << "): " << WSAGetLastError() << endl;
                    continue;
                }

                retval = send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
                if (retval == SOCKET_ERROR) {
                    cout << "준비 완료 패킷 전송 실패 (클라이언트 " << i << "): " << WSAGetLastError() << endl;
                    continue;
                }
                cout << "클라이언트 " << i << "에게 게임 시작 신호 전송 완료" << endl;
            }
        }

        gameStarted = true;
        cout << "\n=== 게임 시작! ===" << endl;
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
//            //총알, 건물, 플레이어의 움직임에 따라 좌표를 수정, 처리하는 함수
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
//            cout << "\n플레이어 " << i << " 이동 처리 시작" << endl;
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
//            // 패킷 수신
//            int retval;
//
//            retval = recvn(clientSockets[i], (char*)&bullet, sizeof(PacketBulletMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "총알 패킷 수신 실패 (플레이어 " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "총알 패킷 수신 성공 (플레이어 " << i << ")" << endl;
//
//            retval = recvn(clientSockets[i], (char*)&building, sizeof(PacketBuildingMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "건물 패킷 수신 실패 (플레이어 " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "건물 패킷 수신 성공 (플레이어 " << i << ")" << endl;
//
//            retval = recvn(clientSockets[i], (char*)&player, sizeof(PacketPlayerMove), 0);
//            if (retval == SOCKET_ERROR) {
//                cout << "플레이어 패킷 수신 실패 (플레이어 " << i << "): " << WSAGetLastError() << endl;
//                continue;
//            }
//            cout << "플레이어 패킷 수신 성공 (플레이어 " << i << ")" << endl;
//
//            // 위치 업데이트 전 위치 출력
//            cout << "플레이어 " << i << " 이동 전 위치: ("
//                << Player[i].pos.x << ", " << Player[i].pos.y << ", " << Player[i].pos.z << ")" << endl;
//
//            // 위치 업데이트
//            Player[i].pos.x += player.pos.x;
//            Player[i].pos.y += player.pos.y;
//            Player[i].pos.z += player.pos.z;
//
//            // 업데이트 후 위치 출력
//            cout << "플레이어 " << i << " 이동 후 위치: ("
//                << Player[i].pos.x << ", " << Player[i].pos.y << ", " << Player[i].pos.z << ")" << endl;
//
//            // 업데이트된 위치 정보를 모든 클라이언트에게 전송
//            for (int j = 0; j < 2; ++j) {
//                if (clientSockets[j] != INVALID_SOCKET) {
//                    cout << "\n클라이언트 " << j << "에게 데이터 전송 시작" << endl;
//
//                    // 총알 정보 전송
//                    char type = PACKET_BULLET_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "총알 타입 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    retval = send(clientSockets[j], (char*)&bullet, sizeof(bullet), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "총알 패킷 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "총알 정보 전송 완료 (클라이언트 " << j << ")" << endl;
//
//                    // 건물 정보 전송
//                    type = PACKET_BUILDING_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "건물 타입 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    retval = send(clientSockets[j], (char*)&building, sizeof(building), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "건물 패킷 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "건물 정보 전송 완료 (클라이언트 " << j << ")" << endl;
//
//                    // 플레이어 정보 전송
//                    type = PACKET_PLAYER_MOVE;
//                    retval = send(clientSockets[j], &type, sizeof(char), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "플레이어 타입 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    Player[i].id = i;  // 플레이어 ID 설정
//                    retval = send(clientSockets[j], (char*)&Player[i], sizeof(PacketPlayerMove), 0);
//                    if (retval == SOCKET_ERROR) {
//                        cout << "플레이어 패킷 전송 실패 (클라이언트 " << j << "): " << WSAGetLastError() << endl;
//                        continue;
//                    }
//                    cout << "플레이어 정보 전송 완료 (클라이언트 " << j << ")" << endl;
//                }
//            }
//            cout << "플레이어 " << i << " 이동 처리 완료\n" << endl;
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


// 아직 정확하게 미완성 
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


// 클라이언트 스레드
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    char type;

    // ClientNum 설정 부분 수정
    WaitForSingleObject(DataEvent, INFINITE);
    int ClientNum = nowID;
    nowID++;  // 클라이언트 수 증가
    SetEvent(DataEvent);

    cout << "클라이언트 " << ClientNum << " 처리 시작" << endl;

    // 소켓을 블로킹 모드로 변경
    u_long mode = 0;  // 0은 블로킹 모드 , 1은 논 블로킹 모드
    ioctlsocket(client_sock, FIONBIO, &mode);


    // 클라이언트 연결 해제 시 처리
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

        // 타입 수신 시도
        retval = recv(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                Sleep(100);  // 잠시 대기
                continue;
            }
            else {
                cout << "타입 수신 실패: " << err << endl;
                break;
            }
        }
        else if (retval == 0) {
            cout << "클라이언트 연결 종료" << endl;
            break;
        }

        //cout << "\n=== 클라이언트 " << ClientNum << " 패킷 처리 시작 ===" << endl;
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
                cout << "준비 패킷 수신 실패: " << WSAGetLastError() << endl;
                break;
            }

            cout << "준비 패킷 수신 성공 (크기: " << retval << " bytes)" << endl;

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = true;

            // 모든 클라이언트에게 전송
            for (int i = 0; i < 2; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    char type = CLIENT_READY;
                    send(clientSockets[i], &type, sizeof(char), 0);
                    readyPacket.id = ClientNum;
                    send(clientSockets[i], (char*)&readyPacket, sizeof(ReadyClientToServer), 0);
                    cout << "클라이언트 " << i << "에게 준비 상태 전송 완료" << endl;
                }
            }

            SetEvent(DataEvent);
            cout << "=== 준비 패킷 처리 완료 ===" << endl;
            break;
        }
        case CLIENT_NOTREADY: {
            NotReadyClientToServer notReadyPacket;
            retval = recv(client_sock, (char*)&notReadyPacket, sizeof(NotReadyClientToServer), 0);
            if (retval == SOCKET_ERROR) {
                cout << "준비 해제 패킷 수신 실패: " << WSAGetLastError() << endl;
                break;
            }

            cout << "클라이언트 " << ClientNum << "로부터 준비 해제 패킷 수신 성공" << endl;

            WaitForSingleObject(DataEvent, INFINITE);
            readyState[ClientNum] = false;
            cout << "클라이언트 " << ClientNum << "의 준비 상태를 false로 설정" << endl;

            // 현재 모든 클라이언트의 준비 상태 출력
            cout << "현재 준비 상태: ";
            for (int i = 0; i < 2; i++) {
                cout << "플레이어" << i << ": " << (readyState[i] ? "준비됨" : "준비안됨") << " ";
            }
            cout << endl;

            // 모든 클라이언트에게 준비 해제 상태 전송
            char type = CLIENT_NOTREADY;
            for (int i = 0; i < 2; i++) {
                if (clientSockets[i] != INVALID_SOCKET) {
                    // 타입 전송
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "준비 해제 타입 전송 실패 (클라이언트 " << i << "): " << WSAGetLastError() << endl;
                        continue;
                    }
                    cout << "클라이언트 " << i << "에게 준비 해제 타입 전송 성공" << endl;

                    // 패킷 전송
                    notReadyPacket.id = ClientNum;  // 어떤 플레이어가 준비 해제했는지 표시
                    retval = send(clientSockets[i], (char*)&notReadyPacket, sizeof(notReadyPacket), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "준비 해제 패킷 전송 실패 (클라이언트 " << i << "): " << WSAGetLastError() << endl;
                        continue;
                    }
                    cout << "클라이언트 " << i << "에게 준비 해제 패킷 전송 성공 (준비 해제한 플레이어: " << ClientNum << ")" << endl;
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
            // 현재 플레이어의 위치 업데이트
            Player[ClientNum] = movePacket;

            // 모든 클라이언트에게 위치 정보 전송
            for (int i = 0; i < 2; i++) {
                if (i != ClientNum && clientSockets[i] != INVALID_SOCKET) {
                    // 패킷 타입 전송
                    char type = PACKET_PLAYER_MOVE;
                    retval = send(clientSockets[i], &type, sizeof(char), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "플레이어 이동 타입 전송 실패: " << WSAGetLastError() << endl;
                        continue;
                    }

                    // 위치 정보 패킷 전송
                    retval = send(clientSockets[i], (char*)&movePacket, sizeof(movePacket), 0);
                    if (retval == SOCKET_ERROR) {
                        cout << "플레이어 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
                       continue;
                    }

                    //cout << "클라이언트 " << i << "에게 플레이어 " << ClientNum << "의 위치 전송 완료" << endl;
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


// 업데이트 스레드
DWORD WINAPI ProcessUpdate(LPVOID arg)
{

    while (true)
    {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 건물 생성
        MakeBuildings();

        // 오브젝트 이동 처리
        //ProcessMove();

        // 충돌 체크
        if (ColidePlayerToObjects()) {
            DeleteObjects();
        }

        if (ColideBulletToObjects()) {
            DeleteObjects();
        }

        // 클라이언트들에게 업데이트된 정보 전송
        SendPacketMadebuildings();

        SetEvent(ClientEvent[0]);
    }
    return 0;

}


int main() {
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "윈속 초기화 실패" << endl;
        return 1;
    }
    cout << "윈속 초기화 성공" << endl;

    // 이벤트 생성 및 확인
    ClientEvent[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
    ClientEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    UpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DataEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

    if (!ClientEvent[0] || !ClientEvent[1] || !UpdateEvent || !DataEvent) {
        cout << "이벤트 생성 실패" << endl;
        return 1;
    }
    cout << "이벤트 생성 성공" << endl;

    // 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        cout << "리스닝 소켓 생성 실패: " << WSAGetLastError() << endl;
        err_quit("listen socket()");
    }
    cout << "리스닝 소켓 생성 성공" << endl;

    // 서버 주소 설정
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(TCPPORT);

    // 바인딩
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        cout << "바인드 실패: " << WSAGetLastError() << endl;
        err_quit("bind()");
    }
    cout << "바인드 성공 (포트: " << TCPPORT << ")" << endl;

    // 리스닝
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        cout << "리스닝 실패: " << WSAGetLastError() << endl;
        err_quit("listen()");
    }
    cout << "리스닝 시작" << endl;

    // 업데이트 스레드 생성
    HANDLE hThread = CreateThread(NULL, 0, ProcessUpdate, NULL, 0, NULL);
    if (hThread != NULL) {
        CloseHandle(hThread);
        cout << "업데이트 쓰레드 생성 성공" << endl;
    }
    else {
        cout << "업데이트 쓰레드 생성 실패" << endl;
    }

    // 클라이언트 접속 처리
    while (true) {
        cout << "\n현재 접속된 클라이언트 수: " << nowID << endl;
        cout << "준비 상태: ";
        for (int i = 0; i < 2; i++) {
            cout << "플레이어" << i << ": " << (readyState[i] ? "준비됨" : "준비안됨") << " ";
        }
        cout << endl;

        SOCKADDR_IN clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) {
            cout << "클라이언트 접속 실패: " << WSAGetLastError() << endl;
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
        cout << "클라이언트 " << nowID << " 소켓 저장 완료" << endl;
        SetEvent(DataEvent);

        HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
            cout << "스레드 생성 실패" << endl;
        }
        else {
            CloseHandle(hThread);
            cout << "클라이언트 " << nowID - 1 << " 스레드 생성 성공" << endl;
        }
    }

    // 정리
    cout << "서버 종료 중..." << endl;
    closesocket(listen_sock);
    WSACleanup();
    cout << "서버 종료 완료" << endl;

    return 0;
}