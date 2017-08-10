#pragma once

#pragma pack(push,1)
class CursorInfo
{
public:
	CursorInfo() :m_posX(0), m_posY(0), m_data(0)
	{
	}
	unsigned short m_posX;
	unsigned short m_posY;
	unsigned char m_data;
};

class Packet
{
public:
	enum
	{
		e_pixel = 1,		// Client to server. Draw a pixel.
		e_line,				// Client to server. Draw a line.
		e_box,				// Client to server. Draw a box.
		e_circle,			// Client to server. Draw a circle.
		e_clientAnnounce,	// Client to server. Client announces that it exists. Server responds with Server Info packet containing window size.
		e_clientCursor,		// Client to server. Current cursor position sent to the server. Server responds with Server Cursors
		e_serverInfo,		// Server to client. Contains server window resolution.
		e_serverCursors		// Server to client. Contains an array of every client cursor value.
	};
	int type;
};

class PacketPixel : public Packet
{
public:
	int x;
	int y;
	float r;
	float g;
	float b;
};

class PacketBox : public Packet
{
public:
	int x;
	int y;
	int w;
	int h;
	float r;
	float g;
	float b;
};

class PacketLine : public Packet
{
public:
	int x1;
	int y1;
	int x2;
	int y2;
	float r;
	float g;
	float b;
};

class PacketCircle : public Packet
{
public:
	int x;
	int y;
	int radius;
	float r;
	float g;
	float b;
};

class PacketClientCursor : public Packet
{
public:
	CursorInfo cursor;
};

class PacketClientAnnounce : public Packet
{
public:

};

class PacketServerInfo : public Packet
{
public:
	unsigned short width;
	unsigned short height;
};

class PacketServerCursors : public Packet
{
public:
	unsigned short count;
	CursorInfo cursor[1];
};

#pragma pack(pop)
