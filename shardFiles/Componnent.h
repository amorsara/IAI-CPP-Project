#pragma once
#include "../shardFiles/IDD.h"
#include "../shardFiles/UdpSocket.h"
#include <map>
#include <thread>
#include<iostream>
using namespace std;

// change opcod - Manager to Controllers
static map<typeComponent, unsigned short> OpcodeOfManager{
	{AntennaComponnent , MANAGER_TO_ANTENNA_CONTROL_MSG_OPCODE },
	{AmplifierComponnent , MANAGER_TO_AMPLIFIER_CONTROL_MSG_OPCODE },
	{ModemComponnent , MANAGER_TO_MODEM_CONTROL_MSG_OPCODE},
};


// change opcod - Controllers to Manager
static map<unsigned short, typeComponent> ControllerOpcode{
	{ANTENNA_TO_MANAGER_CONTROL_MSG_OPCODE, AntennaComponnent},
	{AMPLIFIER_TO_MANAGER_CONTROL_MSG_OPCODE, AmplifierComponnent},
	{MODEM_TO_MANAGER_CONTROL_MSG_OPCODE, ModemComponnent},
};

class Componnent {
protected:
	int counter;
	int minlen = sizeof(Header);
	Message message;
	UdpSocket Serversocket;
	struct sockaddr_in to;
	Devices device;
public:
	Componnent() {}

	Componnent(Devices device) {
		counter = 0;
		this->device = device;
	}

	~Componnent() {}

	// function check the message type and send back
	void check(char* message) {
		if (*(short*)&message[0] != MSG_SYNC) { // if MSG_SYNC of message isnt valid 
			return;
		}
		else {
			if (*(short*)&message[2] != DATA_MSG_OPCODE) { // if this message is control type
				ControlMessage* ctrlMsg = (ControlMessage*)&message;
				if (ctrlMsg->header.msgCounter == ++counter) { 
					// change the opcode for manager
					typeComponent type;
					for (map<typeComponent, unsigned short>::iterator it1 = OpcodeOfManager.begin(); it1 != OpcodeOfManager.end(); ++it1) {
						if (it1->first == ctrlMsg->header.Opcode) {
							type = it1->first;
							break;
						}
					}
					for (map<unsigned short, typeComponent>::iterator it1 = ControllerOpcode.begin(); it1 != ControllerOpcode.end(); ++it1) {
						if (it1->second == type) {
							ctrlMsg->header.Opcode = it1->first;
							break;
						}
					}
					ctrlMsg->Ack = 1; 
				}
				else
					ctrlMsg->Ack = 0; // this message is not valid 
				Serversocket.sendMessage((char*)&ctrlMsg, sizeof(ControlMessage), &to); // send message back
			}
			//this for data message
			else {
				DataMessage* dataMsg = (DataMessage*)&message;
				//only in Modem
				if (device == TRANSMITTER) {
					device = RECEIVER;
					to.sin_port = htons(ComponnentPort(ModemComponnent, device)); // change the port
					Serversocket.sendMessage((char*)&dataMsg, sizeof(ControlMessage), &to);
				}
				else {
					to.sin_port = htons(MessageGenPort); // change the port
					Serversocket.sendMessage((char*)&dataMsg, sizeof(ControlMessage), &to);
				}
			}
		}
	}
	// function recive message from manager
	void recieveMessageComponnent(typeComponent type) {
		Message message;
		UdpSocket Serversocket;
		struct sockaddr_in from;
		from.sin_family = AF_INET;
		from.sin_addr.s_addr = INADDR_ANY;
		unsigned short port = ComponnentPort(ManagerPort(device), type);
		from.sin_port = htons(port);
		if (Serversocket.serverBind(port) < 0) {
			return;
		}
		while (true) { // recive message from manager
			int status = Serversocket.reciveMessage((char*)&message, minlen, sizeof(message), &from);
			if (status == -1) {
				return;
			}
			check((char*)&message); // function check if this message is valid
		}
	}
};






