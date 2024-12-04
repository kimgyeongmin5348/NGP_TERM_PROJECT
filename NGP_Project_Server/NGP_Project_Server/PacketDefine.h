#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ��

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

//#include <gl/glm/glm.hpp>
//#include <gl/glm/ext.hpp>
//#include <gl/glm/gtc/matrix_transform.hpp>
//#include <gl/glm/fwd.hpp>
//#include <gl/glm/gtx/io.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/io.hpp>


#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

using namespace std;

#define MAX_ID_SIZE 32

#define PACKET_LOGIN_REQUEST 3
#define PACKET_LOGIN_RESPONSE 4
#define CLIENT_READY 5
#define CLIENT_NOTREADY 6
#define CLIENT_STATE_READY 7
#define CLIENT_ALL_READY 8
#define PACKET_PLAYER_MOVE 9
#define PACKET_BUILDING_MOVE 10
#define PACKET_BULLET_MOVE 11
#define PACKET_ID 12
#define PACKET_COLLIDE_BULLET_BUILDING 14
#define PACKET_COLLIDE_PLAYER_BUILDING 15

#define TCPPORT			4000


// �α��� ��û ��Ŷ ����ü �߰�
struct PacketLoginRequest {
	char size;
	char type;
	char username[MAX_ID_SIZE];
	char password[MAX_ID_SIZE];
};

// �α��� ���� ��Ŷ ����ü �߰� 
struct PacketLoginResponse {
	char size;
	char type;
	bool success;
	int userID;
};

struct ReadyClientToServer
{
	char size;
	char type;
	char id;
};

struct NotReadyClientToServer
{
	char size;
	char type;
	char id;
};

struct StateReady
{
	char size;
	char type;
	char id;
	//char id;  <<- �̰� �ߺ� ���� ���... ���� �ʿ�
};

struct PacketAllReady
{
	char size;
	char type;
	char id;
};

struct PacketPlayerMove
{
	char size;
	char type;
	glm::vec3 pos;
	char state;
	char playerid[MAX_ID_SIZE];
	char id;
};

struct PacketID
{
	char size;
	char type;
	char id[MAX_ID_SIZE];

};

struct PacketBuildingMove
{
	char size;
	char type;
	glm::vec3 pos;
	glm::vec3 scale;
	bool is_broken;
	int num; // ���� ��ȣ
};

struct PacketBulletMove
{
	char size;
	char type;
	glm::vec3 pos;
	int num; // �Ѿ� ��ȣ
	bool active;
};

struct PacketCollideBB
{
	char size;
	char type;
	int building_num;
	int bullet_num;
};

struct PacketCollidePB
{
	char size;
	char type;
	int num;	// �÷��̾� ��ȣ
	char id[MAX_ID_SIZE];
};

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ���� �Լ� ���� ���
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[����] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}