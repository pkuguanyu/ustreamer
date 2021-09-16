#include "peer_management.h"

//initialization
void init(peer_management *p, int _device_id, int _devices)
{
	for (int i = 0; i < 6; ++i) {
		if (i == _device_id) continue;
		udp_client_init(&(p->clientskt[i]), "127.0.0.1", 8020 + i);
	}

	for (int i = 0; i < _devices; ++i)
		p->current_frame[i] = 0;
		
	p->device_id = _device_id;
	p->devices = _devices;
	udp_server_init(&p->serverskt, 8020 + p->device_id);
}
	
//update the current frame ID being captured by the device
void update_current_frame(peer_management *p, int frame)
{
	if (frame > p->current_frame[p->device_id]) p->current_frame[p->device_id] = frame;
}

//compute how much frames the curent device is ahead of the slowest device in the group.
int gap(peer_management *p)
{
	int ret = 0;
	for (int i = 0; i < p->devices; ++i)
		if (p->current_frame[p->device_id] - p->current_frame[i] > ret) ret = p->current_frame[p->device_id] - p->current_frame[i];
	return ret;
}

//receive the information about the current frame ID being captured by other devices.
void update_peers(peer_management *p)
{
	int size = 0, x, y;
	char* q;
	while (1) {
		if (_select(1, p->serverskt)) size = udp_receive(&(p->serverskt), p->buffer);//a non-block socket receive
		if (size < 0) puts("Error receiving peer information");
		if (size == 0) break;
		
		q = p->buffer;
		while (q < p->buffer + size) {
			x = *q;
			y = *(int*)(q + 1);
			if (p->current_frame[x] < y) p->current_frame[x] = y;
			q += 5;
		}
		size = 0;
	}
}

//tell all other players about the current frame ID being displayed by the player.
//the format of the message:
//+----------+----------+----------+----------+----------+
//|player_id |                     frame_id              |
//+----------+----------+----------+----------+----------+
void broadcast(peer_management *p)
{
	char* q;
	for (int i = 0; i < p->devices; ++i) {
		if (i == p->device_id) continue;
		q = p->buffer;
		*q = (char)p->device_id;
		q++;
		*((int*)q) = p->current_frame[p->device_id];
		udp_send(&(p->clientskt[i]), p->buffer, 5);
	}
}
