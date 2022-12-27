#pragma once
#include "../shardFiles/Componnent.h"
#include "../shardFiles/IDD.h"
#include "../shardFiles/UdpSocket.h"
#include <vector>
#include <list>
#include<mutex>
#include<thread>
#include<iostream>
#include <map>
using namespace std;

typedef struct {
	unsigned short Opcode;//
	time_t time;
	unsigned int msgNumber;
}MsgTime;

mutex mtx;
class Manager :Componnent
{
protected:
	Message msg1;
	struct sockaddr_in to,from;
	list <MsgTime>* listMessages;
	vector <typeComponent> AllComponnents;
	int minlen = sizeof(Header);
	UdpSocket udpSocket;
	Devices device;
public:
	Manager() {}

	Manager(vector <typeComponent>& Componnents, Devices devices) {
		Message* msg = (Message*)&msg1;
		AllComponnents = Componnents; // vector of all component
		listMessages = new list<MsgTime>[Componnents.size()]();
		this->device = devices;
		thread t1(&Manager::sendToDevices, this);
		t1.detach();
		thread t2(&Manager::reciveFromDevices, this, msg);
		t2.detach();
	}

	~Manager() {
		delete(this->listMessages);
	}

	void reciveFromDevices(Message* msg) {
		while (true) {
			unsigned short port = ManagerPort(device); // macro fanction port
			udpSocket.serverBind(port);
			int len = udpSocket.reciveMessage((char*)msg, minlen, sizeof(msg), &from);
			if (msg->header.msgSync != MSG_SYNC) { 
				cout << "this message isnt valid !!" << endl;
			}
			else {
				if (msg->header.Opcode != DATA_MSG_OPCODE) { //this message is control message type
					if (msg->ctrlMsg.Ack == 0) {
						cout << "this message is not valid !! " << endl;
					}
					else {
						if (counter > msg->header.msgCounter + 5) { // this message arrive after 5 s`
							cout << "this message arrive after 5 seconds !!" << endl;
						}
						else { // need to pop message from list
							int t = OpcodeOfManager[ControllerOpcode[msg->header.Opcode]];
							list<MsgTime> msgList = listMessages[t];
							mtx.lock();
							for (list<MsgTime>::iterator it = msgList.begin(); it != msgList.end(); ++it)
							{
								if (it->msgNumber == msg->header.msgCounter) {// pop the message from list
									listMessages[t].erase(it);
								}
							}
							mtx.unlock();
						}
					}
				}
				else { // data message
					if (device == TRANSMITTER) {// send to modem 
						unsigned short port = ComponnentPort(ModemComponnent, device);
						to.sin_port = htons(port);
						udpSocket.sendMessage((char*)msg, sizeof(msg), &to);
					}
					else {
						unsigned short port = MessageGenPort;
						to.sin_port = htons(port);
						udpSocket.sendMessage((char*)msg, sizeof(msg), &to);
					}
				}
			}
		}
	}

		void sendToDevices() {
			Message message; // createa new message
			message.header.msgSync = MSG_SYNC;
			message.ctrlMsg.Ack = 0;
			MsgTime msgTime{};
			to.sin_family = AF_INET;
			to.sin_addr.s_addr = INADDR_ANY;
			while (true) {
				for (int i = 0; i < AllComponnents.size(); i++) // loop send for all components
				{
					typeComponent type = AllComponnents[i]; // which component and fill the message
					message.header.Opcode = OpcodeOfManager[type];
					msgTime.Opcode = OpcodeOfManager[type];
					msgTime.msgNumber = message.header.msgCounter;
					msgTime.time = time(0);
					// push to list for message
					mtx.lock();
					listMessages[i].push_front(msgTime);
					mtx.unlock();
					unsigned short port = ComponnentPort(type, device); // change port
					to.sin_port = htons(port);
					udpSocket.sendMessage((char*)&message, sizeof(message), &to); // send message for the component
				}
				message.header.msgCounter++;
				counter = message.header.msgCounter;
				// thread sleep 1 second after sent message
				this_thread::sleep_for(chrono::milliseconds(1000));
				time_t now = time(0);
				for (int i = 0; i < AllComponnents.size(); i++)
				{
					mtx.lock();
					while (!listMessages[i].empty()) {
						MsgTime msgTime = listMessages[i].back();// return first msg
						if (now - msgTime.time > 5) { // if past 5 s` and didnt get message back
							listMessages[i].pop_back();
						}
						else {
							break;
						}
					}
					mtx.unlock();
				}
			}
		}

	};

