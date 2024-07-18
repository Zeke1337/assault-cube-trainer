#pragma once

#include "Windows.h"

#define ORIGIN 0x34
#define VIEW_ANGLES 0x40
#define TEAM_NUMBER 0x32c
#define PITCH 0x40
#define YAW 0x44
#define NUMBER_OF_PLAYERS 0x10F500
#define IS_DEAD 0x338
#define ENTITY_STRUCT_SIZE 0x4
#define LOCAL_PLAYER 0x10f4f4
#define ENTITY_LIST 0x10f4f8
#define HEALTH 0xf8
#define ARMOR 0xfc

//Physiks
struct Vec3
{
	float x, y, z;
};

struct Vec2
{
	float pitch, yaw;
};

struct Game
{
	DWORD procId;
	uintptr_t moduleBase;
	HANDLE hProcess;
	uintptr_t entitylist;

	
};

//entity
struct Entity
{
	uintptr_t baseAddress{};
	Vec3 origin{};
	Vec2 viewAngles{};
	int health{}, teamNum{}, armor{};
	bool isDead{};
};