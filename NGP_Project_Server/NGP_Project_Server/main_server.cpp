#include "framework.h"
#include "PacketDefine.h"

using namespace std;
PacketPlayerMove Player[2]; //플레이어 정보?
int nowID = 0; // 클라당 id
HANDLE ClientEvent[2];
HANDLE UpdateEvent;
SOCKET clientSockets[2];  // 클라이언트 소켓 저장 배열

bool readyState[2] = { false, false }; // 각 플레이어의 준비 상태
bool gameStarted = false;

void SaveID(const char* NewID)
{
    strncpy(Player[nowID].playerid, NewID, strlen(NewID));
    nowID++;

    cout << "저장완료" << endl;
}

bool CheckID(const char* playerid)
{
    for (int i = 0; i < nowID; ++i)
    {
        if (strcmp(Player[i].playerid, playerid) == false) {
            cout << "중복있음" << endl;
            return false;
        }
    }
    cout << "중복 없음" << endl;
    return true;
}

// Lobby
void SendReadyServerToClient()
{
    // 모든 클라이언트에게 현재 준비 상태 전송
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
    // 모든 플레이어가 준비되었는지 확인
    bool allReady = true;
    for (int i = 0; i < 2; i++) {
        if (!readyState[i]) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        // 모든 클라이언트에게 게임 시작 가능 상태 전송
        AllReady readyPacket;
        readyPacket.size = sizeof(AllReady);
        readyPacket.type = CLIENT_ALL_READY;

        for (int i = 0; i < 2; i++) {
            send(clientSockets[i], (char*)&readyPacket, sizeof(readyPacket), 0);
        }

        // 게임 시작 상태로 전환
        GoToInGame();
    }
}

// 아직 정확하게 미완성 
void GoToInGame()
{
    if (!gameStarted) {
        gameStarted = true;

        // 플레이어 초기 위치 설정
        for (int i = 0; i < 2; i++) {
            // Player[i].pos = { 0.0f, 0.0f, 0.0f };                // 초기 위치 설정  이건 클라랑 합의 필요.
            Player[i].state = 0; // 초기 상태 설정
        }

        // 게임 시작 이벤트 설정
        SetEvent(UpdateEvent);
    }
}

// inGame
void MakeBuildings()
{

}

void ProcessMove()
{

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

}

// 메인 스레드
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    int ClientNum = nowID;
    char type;

    while (true) {
        WaitForSingleObject(ClientEvent[ClientNum], INFINITE);

        retval = recvn(client_sock, (char*)&type, sizeof(type), 0);
        if (retval == SOCKET_ERROR) {
            err_display("Client Thread Type recv()");
            break;
        }

        switch (type) {
        case PACKET_ID: // Login 처리
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
        case CLIENT_READY: // Lobby 준비 상태 처리
        {
            ReadyClientToServer readyPacket;
            retval = recvn(client_sock, (char*)&readyPacket, sizeof(readyPacket), 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            SendReadyServerToClient();
        }
        break;
        case PACKET_PLAYER_MOVE:  // 게임 중 플레이어 이동 처리 
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

// 업데이트 스레드
DWORD WINAPI ProcessUpdate(LPVOID arg)
{
  
    while (true)
    {
        WaitForSingleObject(UpdateEvent, INFINITE);

        // 건물 생성
        MakeBuildings();

        // 오브젝트 이동 처리
        ProcessMove();

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
    if (hThread != NULL) { CloseHandle(hThread); cout << "업데이트 쓰레드 생성" << endl; }

    while (1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }
        // 클라이언트 소켓 저장
        clientSockets[nowID] = client_sock;


        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    closesocket(listen_sock);
    WSACleanup();
}