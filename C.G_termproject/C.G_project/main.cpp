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

    // 3. 로그인 처리
    bool loginSuccess = false;
    while (!loginSuccess && isConnected) {
        char username[MAX_ID_SIZE];
        char password[MAX_ID_SIZE];

        do {
            cout << "\n=== 로그인 ===\n";
            cout << "사용자 이름: ";
            cin >> username;
            cout << "비밀번호: ";
            cin >> password;

            // 입력 길이 검증
            if (strlen(username) >= MAX_ID_SIZE || strlen(password) >= MAX_ID_SIZE) {
                cout << "사용자 이름 또는 비밀번호가 너무 깁니다. (최대 31자)\n";
                continue;
            }

            // 로그인 요청 전송
            if (!SendLoginRequest(username, password)) {
                cout << "로그인 요청 실패\n";
                break;
            }

            // 서버 응답 대기
            PacketLoginResponse loginResponse;
            ZeroMemory(&loginResponse, sizeof(loginResponse));

            char type;
            int retval = recv(sock, &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR || type != PACKET_LOGIN_RESPONSE) {
                cout << "서버 응답 오류\n";
                break;
            }

            retval = recv(sock, (char*)&loginResponse, sizeof(loginResponse), 0);
            if (retval == SOCKET_ERROR) {
                cout << "서버 연결 오류: " << WSAGetLastError() << "\n";
                break;
            }

            // 로그인 결과 처리
            if (loginResponse.success = true) {
                loginSuccess = true;
                myID = loginResponse.userID;
                cout << "\n로그인 성공!\n";
                break;  // 로그인 루프 종료
            }
            else {
                cout << "\n로그인 실패. 사용자 이름 또는 비밀번호가 잘못되었습니다.\n";
                cout << "다시 시도하시겠습니까? (y/n): ";
                char retry;
                cin >> retry;
                if (retry != 'y' && retry != 'Y') {
                    cout << "로그인을 취소합니다.\n";
                    return 1;
                }
            }
        } while (!loginSuccess);

        if (!isConnected) {
            break;
        }
    }

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