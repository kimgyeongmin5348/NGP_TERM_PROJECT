#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

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


#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

using namespace std;

#define MAX_ID_SIZE 32

#define PACKET_LOGIN_REQUEST 3
#define PACKET_LOGIN_RESPONSE 4
#define CLIENT_READY 5
#define CLIENT_ALL_READY 8
#define PACKET_PLAYER_MOVE 9
#define PACKET_BUILDING_MOVE 10
#define PACKET_BULLET_MOVE 11
#define PACKET_COLLIDE_BULLET_BUILDING 14
#define PACKET_COLLIDE_PLAYER_BUILDING 15
#define PACKET_GAME_OVER 16

#define TCPPORT			4000


// 로그인 요청 패킷 구조체 추가
struct PacketLoginRequest {
	char size;
	char type;
	char username[MAX_ID_SIZE];
	char password[MAX_ID_SIZE];
};

// 로그인 응답 패킷 구조체 추가 
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

struct PacketBuildingMove
{
	char size;
	char type;
	glm::vec3 pos;
	glm::vec3 scale;
	bool is_broken;
	int num; // 빌딩 번호
};

struct PacketBulletMove
{
	char size;
	char type;
	glm::vec3 pos;
	int num; // 총알 번호
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
	int num;	// 플레이어 번호
	char id[MAX_ID_SIZE];
};

struct PacketGameOver
{
	char size;
	char type;
	float score;
};

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[오류] %s\n", (char*)lpMsgBuf);
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