// Credits:
// https://guidedhgameking.com/
// https://learn.microsoft.com/en-us/search/
// https://gamehacking.academy/

#include <iostream>

#include "proc.h"
#include "mem.h"
#include "data.h"
#include "functions.h"
#include "menu.h"

using namespace mem;

int main()
{

    MENU menu{};

    Game assaultcube
    {
        GetProcId(L"ac_client.exe"),
        GetModuleBaseAddress(assaultcube.procId, L"ac_client.exe"),
        OpenProcess(PROCESS_ALL_ACCESS, NULL, assaultcube.procId),
        RPM<uintptr_t>(assaultcube.hProcess, assaultcube.moduleBase + ENTITY_LIST),
    };

    Game* game = &assaultcube;

    Entity player
    {
        RPM<uintptr_t>(game->hProcess, game->moduleBase + LOCAL_PLAYER),
    };

    Entity* plr = &player;

    std::vector<Entity> enemy(0);


    if (game->hProcess == nullptr)
    {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    printMenu(&menu);
    // Main loop to monitor process and handle options
    DWORD dwExit = 0;
    while (GetExitCodeProcess(game->hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        
        
        
        
        

        
        

        

        if (GetAsyncKeyState(VK_CONTROL) & 1)
        {
            menu.bOptionAimbot = !menu.bOptionAimbot;
            menu.bMenuHasUpdated = true;
            // Execute option two logic here if needed
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            menu.bOptionRecoil = !menu.bOptionRecoil;
            menu.bMenuHasUpdated = true;

            // Execute option one logic here if needed
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            menu.bOptionTriggerbot = !menu.bOptionTriggerbot;
            menu.bMenuHasUpdated = true;

            // Execute option one logic here if needed
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            menu.bOptionHealth = !menu.bOptionHealth;
            menu.bMenuHasUpdated = true;

            // Execute option one logic here if needed
        }

        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {

            menu.bOptionArmor = !menu.bOptionArmor;
            menu.bMenuHasUpdated = true;

            // Execute option one logic here if needed
        }

        if (GetAsyncKeyState(VK_NUMPAD5) & 1)
        {
            menu.bOptionAmmo = !menu.bOptionAmmo;
            menu.bMenuHasUpdated = true;

            // Execute option one logic here if needed
        }





        // Execute option two logic every tick if bOptionTwo is true
        if (menu.bOptionAimbot)
        {
            getEntityInfo(game, plr);
            aimbot(game, enemy, plr);
        }

        if (menu.bMenuHasUpdated)
        {
            printMenu(&menu);
            menu.bMenuHasUpdated = false;
        }
    }

    CloseHandle(game->hProcess);
    return 0;
}