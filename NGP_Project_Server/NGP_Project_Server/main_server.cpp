#include "framework.h"
#include "PacketDefine.h"

using namespace std;
PacketPlayerMove Player[2]; //플레이어 정보?
int nowID = 0; // 클라당 id
HANDLE ClientEvent[2];
HANDLE UpdateEvent;

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

}

void SendReadyCompleteServerToClient()
{

}

void GoToInGame()
{

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

}

BOOL ColideBulletToObjects()
{

}

void DeleteObjects()
{

}

void SendPacketMadebuildings()
{

}

void SendPacketMadebuildings()
{

}

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

        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    closesocket(listen_sock);
    WSACleanup();
}