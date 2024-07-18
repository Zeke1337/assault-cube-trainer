#pragma once
#include <iostream>

/*
0 - Black
1 - Blue
2 - Green
3 - Aqua
4 - Red
5 - Purple
6 - Yellow
7 - White
8 - Gray
9 - Light Blue
10 - Light Green
11 - Light Aqua
12 - Light Red
13 - Light Purple
14 - Light Yellow
15 - Bright White
*/

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

struct MENU
{
	bool bOptionAimbot,
		bOptionAmmo,
		bOptionHealth,
		bOptionArmor,
		bOptionRecoil,
		bOptionTriggerbot,
		bMenuHasUpdated = false;
};

void printMenu(MENU* menu)
{
	system("cls");
	setColor(7);
	std::cout << "ASHURA AC HACK" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;


	std::cout << "AIM" << std::endl;

	if (menu->bOptionAimbot)
		setColor(2);
	else
		setColor(7);

	std::cout << "AIMBOT        [CTRL]" << std::endl;


	if (menu->bOptionRecoil)
		setColor(2);
	else
		setColor(7);
	std::cout << "NO RECOIL     [NUM1]" << std::endl;


	if (menu->bOptionTriggerbot)
		setColor(2);
	else
		setColor(7);
	std::cout << "TRIGGERBOT    [NUM2]" << std::endl;

	setColor(7);
	std::cout << "\nMISC" << std::endl;

	if (menu->bOptionHealth)
		setColor(2);
	else
		setColor(7);
	std::cout << "HEALTH    [NUM3]" << std::endl;

	if (menu->bOptionArmor)
		setColor(2);
	else
		setColor(7);
	std::cout << "ARMOR     [NUM4]" << std::endl;


	if (menu->bOptionAmmo)
		setColor(2);
	else
		setColor(7);
	std::cout << "AMMO      [NUM5]" << std::endl;

	setColor(7);
}

