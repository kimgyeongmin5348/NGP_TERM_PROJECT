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

    //// 3. �α��� ó��
    //bool loginSuccess = false;
    //while (!loginSuccess && isConnected) {
    //    char playerid[MAX_ID_SIZE];
    //    cout << "ID �Է�: ";
    //    cin >> playerid;

    //    // ID ���� üũ
    //    if (strlen(playerid) >= MAX_ID_SIZE) {
    //        cout << "ID�� �ʹ� ��ϴ�. �ٽ� �Է����ּ���." << endl;
    //        continue;
    //    }

    //    // �α��� ��û ����
    //    if (!SendLoginRequest(playerid)) {
    //        cout << "�α��� ��û ����" << endl;
    //        break;
    //    }

    //    // ���� ���� ���
    //    ClientLoginUsePacket loginResult;
    //    ZeroMemory(&loginResult, sizeof(loginResult));
    //    int retval = recvn(sock, (char*)&loginResult, sizeof(loginResult), 0);
    //    if (retval == SOCKET_ERROR) {
    //        cout << "���� ���� ����: " << WSAGetLastError() << endl;
    //        break;
    //    }

    //    // �α��� ��� ó��
    //    if (loginResult.type == ID_USE) {
    //        loginSuccess = true;
    //        myID = nowID;
    //        cout << "�α��� ���� (ID: " << playerid << ")" << endl;
    //    }
    //    else {
    //        cout << "�̹� ��� ���� ID�Դϴ�. �ٸ� ID�� �Է����ּ���." << endl;
    //    }
    //}

    //// 4. �α��� ���� �� ����
    //if (!loginSuccess) {
    //    CleanupNetwork();
    //    game.Shutdown();
    //    return 1;
    //}

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