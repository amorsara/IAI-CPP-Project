#pragma once
#include <iostream>
#include <winsock2.h>
#include "../shardFiles/IDD.h"
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "192.168.34.1"  //  ip address of UDP server


class UdpSocket {
private:
	SOCKET msgSocket;
public:
	UdpSocket(){
        // initialise winsock
		WSADATA ws;
       // cout << "Initialising Winsock..." << endl;
        if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
        {
            cout << "WSAStartup Failed,Error Code : " << WSAGetLastError() << endl;
            return;
        }
        //cout << "Initialised." << endl;
        // create socket
        if ((msgSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) // <<< UDP socket // SOCKET_ERROR
        {
            cout << "socket() failed with error code : " << WSAGetLastError() << endl;
            return;
        }
       // cout << "socket done." << endl;
	}

	~UdpSocket() {
		closesocket(msgSocket);
		WSACleanup();
	}

	int serverBind(unsigned short port) {
        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port);

        if (bind(msgSocket,(struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
        {
            cout << "Bind failed with error code : " << WSAGetLastError() << endl;
            return -1;
        }
      //  cout << "Bind done." << endl;;
        return 0;
	}

	int sendMessage(char* buf,int len,struct sockaddr_in *to){
        int status = sendto(msgSocket, buf, len, 0, (struct sockaddr*)to, sizeof(*to));
        if (status == SOCKET_ERROR) {
            cout << "sendto() failed with error code : "<< WSAGetLastError() << endl;
            return -1;
        }
      //  cout << "send message."<< endl;
        return 0;
    }

    int reciveMessage(char* buf, int minlen, int maxlen, struct sockaddr_in* from) {
        int lenfrom = sizeof(*from);
        int len=0, temp;
        while (len< minlen)
        {
            temp = recvfrom(msgSocket, buf, maxlen, 0, (sockaddr*)from, &lenfrom);
            if (temp == SOCKET_ERROR) {
                cout << "recvfrom() failed with error code : " << WSAGetLastError() << endl;
                return -1;
            }
            len += temp;
        }
        Message* msg = (Message*)buf;
        ControlMessage* cMsg = (ControlMessage*)msg;
        // print message
        cout<<(short*)cMsg->header.msgCounter<<endl;
        cout << (short*)cMsg->header.Opcode << endl;
        cout << (short*)cMsg->header.msgSync<< endl;
        cout << (short*)cMsg->Ack << endl;
        return len;
    }
};
