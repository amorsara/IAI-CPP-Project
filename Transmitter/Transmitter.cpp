#pragma once
using namespace std;
#include <iostream>
#include <thread>
#include "../shardFiles/UdpSocket.h"
#include "../shardFiles/IDD.h"
#include "../shardFiles/Componnent.h"
#include "../shardFiles/Antenna.h"
#include "../shardFiles/Manager.h"
#include "../shardFiles/Amplifier.h"
#include "../shardFiles/Modem.h"

int main()
{
	vector < typeComponent>Componnents = {
		AntennaComponnent,
		AmplifierComponnent,
		ModemComponnent
	};
	Antenna antenna(TRANSMITTER, AntennaComponnent);
	Amplifier amplifier(TRANSMITTER, AmplifierComponnent);
	Modem modem(TRANSMITTER, ModemComponnent);
	Manager manager(Componnents, TRANSMITTER);

	while (true) {
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
}



