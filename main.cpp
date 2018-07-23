#include <iostream>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctime>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h> //From reference https://stackoverflow.com/questions/27096985/getting-error-code-when-creating-a-socket-in-linux when trying to debug
#include <thread>
#include <chrono>
#include <cstring> //From reference https://stackoverflow.com/questions/16354058/c-compile-issue when trying to debug
using namespace std;

#define INF 100000

int main(int argc, const char* argv[]) {
	//Some error Checking

	//Make sure there's only one argument
	if (argc > 2 || argc < 2) {
		cout << "Incorrect number of arguments" << endl;
		return 0;
	}
	//Make sure there is a valid router number
	else if (stoi(argv[1]) > 4 || stoi(argv[1]) < 0) {
		cout << "Please enter a valid router number (1, 2, 3, 4)" << endl;
		return 0;
	}

	//Some variables
	string my_ip; int port; int sock = socket(AF_INET, SOCK_DGRAM, 0);
	my_ip = "127.0.0.1";
	int dis_vec[4] = { INF, INF, INF, INF };
	int prev_dv[4];
	string locations[4];
	int no_path = -1;

	//Initalize port and distance vector
	if (stoi(argv[1]) == 1) {
		port = 10001;
		dis_vec[0] = 0; dis_vec[1] = 1; dis_vec[2] = 4; dis_vec[3] = 8;
		locations[0] = "Local"; locations[1] = "R2"; locations[2] = "R3"; locations[3] = "R4";
	}
	else if (stoi(argv[1]) == 2) {
		port = 10002;
		dis_vec[0] = 1; dis_vec[1] = 0; dis_vec[2] = 2;
		no_path = 4;
		locations[0] = "R1"; locations[1] = "Local"; locations[2] = "R3"; locations[3] = "-";
	}
	else if (stoi(argv[1]) == 3) {
		port = 10003;
		dis_vec[0] = 4; dis_vec[1] = 2; dis_vec[2] = 0; dis_vec[3] = 1;
		locations[0] = "R1"; locations[1] = "R2"; locations[2] = "Local"; locations[3] = "R4";
	}
	else if (stoi(argv[1]) == 4) {
		port = 10004;
		dis_vec[0] = 8; dis_vec[2] = 1; dis_vec[3] = 0;
		no_path = 2;
		locations[0] = "-l"; locations[1] = "R2"; locations[2] = "R3"; locations[3] = "Local";
	}
	//Setup basic information
	struct sockaddr_in dest, host;
	const char* this_ip = my_ip.c_str();

	//Other routers
	int dest_size = sizeof(dest);
	dest.sin_family = AF_INET;
	inet_pton(AF_INET, this_ip, &dest.sin_addr);

	//This router
	int host_size = sizeof(host);
	host.sin_family = AF_INET;
	host.sin_port = htons(port);
	inet_pton(AF_INET, this_ip, &host.sin_addr);

	//Bind socket
	bind(sock, (const sockaddr*)&host, sizeof(host));

	//Prepare the message
	struct message {
		int seq_num;
		int router_num;
		int dis_vect[4];
	};

	//Wait a little before we continue to make sure the socket has be successfully set up.
	cout << "Setting up some things. . ." << endl;
	this_thread::sleep_for(chrono::seconds(2)); //From reference: 4.	Cplusplus.com “<chrono>” - http://www.cplusplus.com/reference/chrono/ and http://www.cplusplus.com/reference/thread/this_thread/sleep_for/
	cout << "Almost done configuring. . .\n";
	this_thread::sleep_for(chrono::seconds(3)); //From reference: 4.	Cplusplus.com “<chrono>” - http://www.cplusplus.com/reference/chrono/ and http://www.cplusplus.com/reference/thread/this_thread/sleep_for/
	cout << "Done! \n";
	cout << "Router " << argv[1] << " is configured and on port " << port << endl;
	cout << "On port hton: " << htons(port) << endl;
	cout << "INITIAL FORWARDING TABLE" << endl;
	cout << "------------------------\n" << "R1: " << dis_vec[0] << endl << "R2: " << dis_vec[1] << endl << "R3: " << dis_vec[2] << endl << "R4: " << dis_vec[3] << endl << "------------------------\n";

	bool updated = true; //If the distance vector was updated, we need to send it out to other routers
	bool new_router = true; //We don't need to talk to those who don't have a path to us
	int j = 0;
	while (j < 3) {
		//Keep track of some things
		bool talk_to[4] = { false, false, false, false };
		bool social_party_monster = false; //Keep track if we talked to everyone or not (is everyone connected)
		talk_to[stoi(argv[1]) - 1] = true; //We don't have to talk to ourself, we are ourself
		if (no_path != -1) talk_to[no_path - 1] = true; //If we don't have a path, we don't have to talk to it
		while (!social_party_monster) {

			//Send out the distance vector to whomever is neighboring
			for (int i = 0; i < 4; i++) {
				if ((no_path != -1 && no_path == i + 1) || i + 1 == stoi(argv[1])) continue;
				else {
					for (int k = 0; k < 4; k++) dis_vec[k] = htonl(dis_vec[k]); //Before we send out something, make it Big Endian
					int s_num = htonl(i);
					int r_num = htonl(stoi(argv[1]));
					struct message dv = { s_num, r_num,{ dis_vec[0], dis_vec[1], dis_vec[2], dis_vec[3] } };
					dest.sin_port = htons(10000 + i + 1);

					//Wait a little before we send something
					this_thread::sleep_for(chrono::milliseconds(200)); //From reference: 4.	Cplusplus.com “<chrono>” - http://www.cplusplus.com/reference/chrono/ and http://www.cplusplus.com/reference/thread/this_thread/sleep_for/
					
					if ((new_router || updated)) {
						cout << "Sending Distance Vector to: R" << i + 1 << endl;
						sendto(sock, &dv, sizeof(dv), 0, (const sockaddr*)&dest, sizeof(dest)); //Reference http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html used to explain meaning of parameters for setsockopt
						for (int k = 0; k < 4; k++) dis_vec[k] = ntohl(dis_vec[k]);
					}
				}
			}
			while (true) {
				struct timeval timeout;
				timeout.tv_sec = 0;
				timeout.tv_usec = 1;
				struct message r_dv;
				//We want to receive from any port 10001 - 10004

				//Reference https://linux.die.net/man/2/recvfrom used to explain meaning of parameters
				if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == 0
					&& recvfrom(sock, &r_dv, sizeof(r_dv), 0, 0, 0) < 0) {
					continue;
				}
				//We recieved something!
				else {
					//Don't forget to convert back to little Endian so we can read it
					for (int k = 0; k < 4; k++) r_dv.dis_vect[k] = ntohl(r_dv.dis_vect[k]); //Before we send out something, make it Big Endian
					r_dv.seq_num = ntohl(r_dv.seq_num);
					r_dv.router_num = ntohl(r_dv.router_num);
					
					//Who have we received it from?
					cout << "Receiving Distance Vector from: " << r_dv.router_num << endl;

					//Received a distance vector, let's update our own
					//Store the previous distance vector
					for (int i = 0; i < 4; i++)
						prev_dv[i] = dis_vec[i];

					//Execute Bellman Ford Algorithm to Update Fowarding Table
					//Something came in, let's see if anything needs updating
					for (int i = 0; i < 4; i++) {
						
						//If we update our distance vector, we need to send out our new one
						if (dis_vec[i] > dis_vec[r_dv.router_num - 1] + r_dv.dis_vect[i]) {
							dis_vec[i] = dis_vec[r_dv.router_num - 1] + r_dv.dis_vect[i];
							locations[i] = "R" + to_string(r_dv.router_num);
							//Something HAS been updated
							updated = true;
							//We have now talked to this router
							talk_to[r_dv.router_num - 1] = true;
						}
					}
					//If we have updated, that is taken care of through updated = true

					//If we have seen this router before and there is an update, we need to recompute the distance vector
					//This is also taken care of by updated = true

					//If we haven't seen this router before, and we did not update, it is a new router
					//We need to send something
					//This works because if we have updated, we will already be marked to break and send something
					//If we didn't updated, the router being seen is not set to true, so we have not seen it and we have not updated
					if (talk_to[r_dv.router_num - 1] == false && !updated) {
						new_router = true; //It's a new router
						talk_to[r_dv.router_num - 1] = true; //We've seen it now
					}


					//Nothing has been updated and there's no new router, no need to send out our distance vector

					else {
						updated = false;
						new_router = false;
					}
					if (updated || !new_router) {
						int same = 0;
						for (int i = 0; i < 4; i++) {
							if (prev_dv[i] == dis_vec[i]) {
								same++; updated = false;
							}
						}
						if (same != 4) {
							cout << "UPDATE OCCURRED" << endl;
							cout << "     Cost    Next Hop\n";
							cout << "----------------------\n" << 
								"R1: " << dis_vec[0] << "        " << locations[0] << endl <<
								"R2: " << dis_vec[1] << "        " << locations[1] << endl <<
								"R3: " << dis_vec[2] << "        " << locations[2] << endl <<
								"R4: " << dis_vec[3] << "        " << locations[3] << endl << "----------------------\n";
							
						}
					}
					//Have we talked to everyone at the party?
					for (int i = 0; i < 4; i++) {
						if (talk_to[i] == false) { social_party_monster = false;  break; }
						else social_party_monster = true;
					}
					
					if (updated || new_router || social_party_monster) break;

				}

			}

		}
		j++;
	}
	cout << "All contacts have been made" << endl;
	cout << "----------------------\n\nFINAL FORWARDING TABLE\n" << endl << endl;
	cout << "     Cost    Next Hop\n";
	cout << "----------------------\n" << "R1: " << dis_vec[0] << "        " << locations[0] << endl <<
										  "R2: " << dis_vec[1] << "        " << locations[1] << endl <<
										  "R3: " << dis_vec[2] << "        " << locations[2] << endl << 
										  "R4: " << dis_vec[3] << "        " << locations[3] << endl << "----------------------\n";
	
	return 0;
}