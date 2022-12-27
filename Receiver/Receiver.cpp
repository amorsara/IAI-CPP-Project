#pragma once
using namespace std;
#include <iostream>
#include <thread>
#include "../shardFiles/UdpSocket.h"
#include "../shardFiles/IDD.h"
#include "../shardFiles/Componnent.h"
#include "../shardFiles/Antenna.h"
#include "../shardFiles/Modem.h"
#include "../shardFiles/Manager.h"


int main()
{
	vector < typeComponent>Componnents = {
		AntennaComponnent,
		ModemComponnent
	};
	Antenna antenna(RECEIVER, AntennaComponnent);
	Modem modem(RECEIVER, ModemComponnent);
	Manager manager(Componnents, RECEIVER);

	while (true) {
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
}

