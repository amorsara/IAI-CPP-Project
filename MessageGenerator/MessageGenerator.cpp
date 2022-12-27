#pragma once
using namespace std;
#include <iostream>
#include <thread>
#include "../shardFiles/IDD.h"
#include "../shardFiles/UdpSocket.h"

int sendDataMessages(int& cntSend) {//function to send data message
	int time = 0;
	UdpSocket tx;
	struct sockaddr_in to = {
		 to.sin_family = AF_INET,
		 to.sin_port = htons(ManagerPort(TRANSMITTER)),
		 to.sin_addr.S_un.S_addr = inet_addr(SERVER),
	};
	// the message
	DataMessage dataMsg; // create and fill message
	dataMsg.header.msgSync = MSG_SYNC;
	dataMsg.header.Opcode = DATA_MSG_OPCODE;
	while (true) { 
		dataMsg.header.msgCounter = ++cntSend;// count send message
		int i = tx.sendMessage((char*)&dataMsg, sizeof(dataMsg), &to);// send message
		this_thread::sleep_for(chrono::milliseconds(1000));// sleep for 1 s' after send message
	}
	return cntSend; //return count send message
}

int reciveDataMessages(int& cntRec) {// functuion recive data message
	DataMessage dataMsg;
	UdpSocket rx;
	struct sockaddr_in from;
	while (true) {
		int j = rx.reciveMessage((char*)&dataMsg, sizeof(Header), sizeof(dataMsg), &from);// recive message...
		DataMessage getDataMsg = (DataMessage)dataMsg;
		if (rx.serverBind(MessageGenPort) < 0) {
			return -1;
		}
		if (getDataMsg.header.msgSync != MSG_SYNC) {// if this message isnt vallid
			cout << "this message isnt valid !!" << endl;
		}
		else {
			if (getDataMsg.header.Opcode != DATA_MSG_OPCODE) {// check if opcode is dataOpcode
				cout << "this message isnt valid !!" << endl;
			}
			else {
				cntRec++;  // count message that recive
			}
		}
	}
	return cntRec; // return count message
}

int main()
{
	int second, cntMsgSend = 0, cntMsgRecive = 0;
	struct sockaddr_in from, to;
	UdpSocket rx, tx;
	DataMessage dataMsg, getMsg;
	cout << "enter num of seconds to run : " << endl;
	cin >> second;
	thread t1(sendDataMessages, ref(cntMsgSend)); // thread for send message
	t1.detach();
	thread t2(reciveDataMessages, ref(cntMsgRecive));// thread for recive message
	t2.detach();
	this_thread::sleep_for(chrono::seconds(second));// whait for x seconds to send & recive message
	cout << "send " << cntMsgSend << " message, get " << cntMsgRecive << "  message, during " << second << " secounds !!!" << endl;
}



