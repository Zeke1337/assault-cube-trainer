#pragma once


//Physiks
struct Vec3
{
	float x, y, z;
};

struct Vec2
{
	float pitch, yaw;
};

//entity
struct Entity
{
	Vec3 origin{};
	Vec2 viewAngles{};
	int health{};
	bool isDead{};

	struct Ammo
	{
		int currentAmmo;
		int grenadeAmmo;
	};

};