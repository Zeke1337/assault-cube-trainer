// Credits:
// https://guidedhgameking.com/
// https://learn.microsoft.com/en-us/search/
// https://gamehacking.academy/

#include <iostream>

#include "proc.h"
#include "mem.h"
#include "entity.h"
#include "functions.h"

using namespace mem;

int main()
{

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

    bool bOptionOne = false;
    bool bOptionTwo = false;

    if (game->hProcess == nullptr)
    {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    
    // Main loop to monitor process and handle options
    DWORD dwExit = 0;
    while (GetExitCodeProcess(game->hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bOptionOne = !bOptionOne;
            
            // Execute option one logic here if needed
        }


        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bOptionTwo = !bOptionTwo;
            // Execute option two logic here if needed
        }

        // Execute option two logic every tick if bOptionTwo is true
        if (bOptionTwo)
        {
            getEntityInfo(game, plr);
            aimbot(game, enemy, plr);
        }
    }

    CloseHandle(game->hProcess);
    return 0;
}