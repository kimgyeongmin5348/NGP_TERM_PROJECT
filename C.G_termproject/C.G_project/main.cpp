// main.cpp
#include "GameFramework.h"
#include "ServerToClient.h"

// int myID = -1;   // 로그인 때문에 있었음
// extern int nowID;    // 로그인 때문에 있었음

int main(int argc, char** argv) {
    // 1. 게임 프레임워크 객체 생성
    GameFramework game;

    // 게임 프레임워크 초기화
    game.Initialize(argc, argv);
    // 11_20 동기화 위해서 추가한 함수...(로비준비에서 게임 시작을 위해서)
    // Scene::GetInstance()->InitializePlayer();  // 플레이어만 초기화

    // 2. 네트워크 초기화
    if (!InitializeNetwork()) {
        cout << "네트워크 초기화 실패" << endl;
        game.Shutdown();
        return 1;
    }

    cout << "게임 시작..." << endl;

    //// 3. 로그인 처리
    //bool loginSuccess = false;
    //while (!loginSuccess && isConnected) {
    //    char playerid[MAX_ID_SIZE];
    //    cout << "ID 입력: ";
    //    cin >> playerid;

    //    // ID 길이 체크
    //    if (strlen(playerid) >= MAX_ID_SIZE) {
    //        cout << "ID가 너무 깁니다. 다시 입력해주세요." << endl;
    //        continue;
    //    }

    //    // 로그인 요청 전송
    //    if (!SendLoginRequest(playerid)) {
    //        cout << "로그인 요청 실패" << endl;
    //        break;
    //    }

    //    // 서버 응답 대기
    //    ClientLoginUsePacket loginResult;
    //    ZeroMemory(&loginResult, sizeof(loginResult));
    //    int retval = recvn(sock, (char*)&loginResult, sizeof(loginResult), 0);
    //    if (retval == SOCKET_ERROR) {
    //        cout << "서버 연결 오류: " << WSAGetLastError() << endl;
    //        break;
    //    }

    //    // 로그인 결과 처리
    //    if (loginResult.type == ID_USE) {
    //        loginSuccess = true;
    //        myID = nowID;
    //        cout << "로그인 성공 (ID: " << playerid << ")" << endl;
    //    }
    //    else {
    //        cout << "이미 사용 중인 ID입니다. 다른 ID를 입력해주세요." << endl;
    //    }
    //}

    //// 4. 로그인 실패 시 종료
    //if (!loginSuccess) {
    //    CleanupNetwork();
    //    game.Shutdown();
    //    return 1;
    //}

    // 5. 게임 실행
    while (isConnected) {
        game.Run();
        if (!isConnected) {
            cout << "서버와의 연결이 끊어졌습니다." << endl;
            break;
        }
    }


    // 6. 종료 처리
    game.Shutdown();
    CleanupNetwork();
    return 0;
}