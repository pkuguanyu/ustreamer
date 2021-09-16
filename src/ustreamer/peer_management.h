#include "udp.h"

//a struct recording the current frame ID being captured by each device in the group.
typedef struct
{
	_udp_t clientskt[6];
	_udp_t serverskt;
	int current_frame[6], device_id, devices;
	char buffer[20000];
} peer_management;
	
void init(peer_management *p, int _device_id, int _devices);
void update_current_frame(peer_management *p, int frame);
int gap(peer_management *p);
void update_peers(peer_management *p);
void broadcast(peer_management *p);
