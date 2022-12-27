#pragma once
#include "../shardFiles/Componnent.h"
#include "../shardFiles/IDD.h"
#include "../shardFiles/UdpSocket.h"
#include<thread>
#include<iostream>
using namespace std;
class Modem:Componnent
{
public:
	Modem(Devices devices, typeComponent type) {
		devices = devices;
		thread t1(&Modem::recieveMessageComponnent, this, type);
		t1.detach();
	}
};

