#pragma once
#include "../shardFiles/Componnent.h"
#include "../shardFiles/IDD.h"
#include "../shardFiles/UdpSocket.h"
#include<thread>
#include<iostream>
using namespace std;
class Antenna:Componnent
{
public:
	Antenna(Devices devices, typeComponent type) {
		Devices device = devices;
		thread t1(&Antenna::recieveMessageComponnent, this, type);
		t1.detach();
	}
};

