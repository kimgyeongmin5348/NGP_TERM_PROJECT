// main.cpp
#include "GameFramework.h"
#include "ServerToClient.h"

// int myID = -1;   // �α��� ������ �־���
// extern int nowID;    // �α��� ������ �־���

int main(int argc, char** argv) {
    // 1. ���� �����ӿ�ũ ��ü ����
    GameFramework game;

    // ���� �����ӿ�ũ �ʱ�ȭ
    game.Initialize(argc, argv);
    // 11_20 ����ȭ ���ؼ� �߰��� �Լ�...(�κ��غ񿡼� ���� ������ ���ؼ�)
    // Scene::GetInstance()->InitializePlayer();  // �÷��̾ �ʱ�ȭ

    // 2. ��Ʈ��ũ �ʱ�ȭ
    if (!InitializeNetwork()) {
        cout << "��Ʈ��ũ �ʱ�ȭ ����" << endl;
        game.Shutdown();
        return 1;
    }

    cout << "���� ����..." << endl;

    // 3. �α��� ó��
    bool loginSuccess = false;
    while (!loginSuccess && isConnected) {
        char username[MAX_ID_SIZE];
        char password[MAX_ID_SIZE];

        do {
            cout << "\n=== �α��� ===\n";
            cout << "����� �̸�: ";
            cin >> username;
            cout << "��й�ȣ: ";
            cin >> password;

            // �Է� ���� ����
            if (strlen(username) >= MAX_ID_SIZE || strlen(password) >= MAX_ID_SIZE) {
                cout << "����� �̸� �Ǵ� ��й�ȣ�� �ʹ� ��ϴ�. (�ִ� 31��)\n";
                continue;
            }

            // �α��� ��û ����
            if (!SendLoginRequest(username, password)) {
                cout << "�α��� ��û ����\n";
                break;
            }

            // ���� ���� ���
            PacketLoginResponse loginResponse;
            ZeroMemory(&loginResponse, sizeof(loginResponse));

            char type;
            int retval = recv(sock, &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR || type != PACKET_LOGIN_RESPONSE) {
                cout << "���� ���� ����\n";
                break;
            }

            retval = recv(sock, (char*)&loginResponse, sizeof(loginResponse), 0);
            if (retval == SOCKET_ERROR) {
                cout << "���� ���� ����: " << WSAGetLastError() << "\n";
                break;
            }

            // �α��� ��� ó��
            if (loginResponse.success = true) {
                loginSuccess = true;
                myID = loginResponse.userID;
                cout << "\n�α��� ����!\n";
                break;  // �α��� ���� ����
            }
            else {
                cout << "\n�α��� ����. ����� �̸� �Ǵ� ��й�ȣ�� �߸��Ǿ����ϴ�.\n";
                cout << "�ٽ� �õ��Ͻðڽ��ϱ�? (y/n): ";
                char retry;
                cin >> retry;
                if (retry != 'y' && retry != 'Y') {
                    cout << "�α����� ����մϴ�.\n";
                    return 1;
                }
            }
        } while (!loginSuccess);

        if (!isConnected) {
            break;
        }
    }

    // 5. ���� ����
    while (isConnected) {
        game.Run();
        if (!isConnected) {
            cout << "�������� ������ ���������ϴ�." << endl;
            break;
        }
    }


    // 6. ���� ó��
    game.Shutdown();
    CleanupNetwork();
    return 0;
}