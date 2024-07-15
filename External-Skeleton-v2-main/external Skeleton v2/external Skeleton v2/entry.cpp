// Credits:
// https://guidedhacking.com/
// https://learn.microsoft.com/en-us/search/

#include <iostream>

#include "proc.h"
#include "mem.h"
#include "entity.h"

#define M_PI 3.14159265358979323846


float euclidean_distance(float x, float y) {
    return sqrtf((x * x) + (y * y));
}


int main()
{
    using namespace mem;
    // Variables:
    bool bOptionOne = false;
    bool bOptionTwo = false;
    bool foundEnemy = false;
    DWORD procId = 0;
    HANDLE hProcess = nullptr;
    uintptr_t moduleBase = 0;
    uintptr_t localplayer = 0;

    // Attempt to get process ID and module base address
    procId = GetProcId(L"ac_client.exe");
    if (procId == 0)
    {
        std::cerr << "Failed to retrieve process ID." << std::endl;
        return 1;
    }

    moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
    if (moduleBase == 0)
    {
        std::cerr << "Failed to retrieve module base address." << std::endl;
        return 1;
    }

    // Open the process with appropriate access rights
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    if (hProcess == nullptr)
    {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }
    
    Entity* enemy = new Entity[16];

    localplayer = RPM<uintptr_t>(hProcess, moduleBase + 0x10f4f4);
    
    uintptr_t entitylist = RPM<uintptr_t>(hProcess, moduleBase + 0x10f4f8);


    // Main loop to monitor process and handle options
    DWORD dwExit = 0;
    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        Entity player{};
        Entity* plr = &player;
        
        plr->origin = RPM<Vec3>(hProcess, localplayer + 0x34);
        plr->viewAngles = RPM<Vec2>(hProcess, localplayer + 0x40);

        
        
        // Toggle option one with VK_NUMPAD1
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bOptionOne = !bOptionOne;
            
            // Execute option one logic here if needed
        }

        // Toggle option two with VK_NUMPAD2
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bOptionTwo = !bOptionTwo;
            // Execute option two logic here if needed
        }

        // Execute option two logic every tick if bOptionTwo is true
        if (bOptionTwo)
        {
            

            //std::cout << plr->origin.x << std::endl;

            float closest_player = 20.0f;
            float closest_yaw = 0.0f;
            float closest_pitch = 0.0f;

            foundEnemy = false;

            for (int i = 0; i < 16; ++i)
            {
                
                uintptr_t entity = RPM<uintptr_t>(hProcess, entitylist + (i * 0x4));
                enemy[i].origin = RPM<Vec3>(hProcess, entity + 0x34);
                enemy[i].isDead = RPM<int>(hProcess, entity + 0x338);

                if (!enemy[i].isDead)
                {
                    float abspos_x = enemy[i].origin.x - plr->origin.x;
                    float abspos_y = enemy[i].origin.y - plr->origin.y;
                    float abspos_z = enemy[i].origin.z - plr->origin.z;
                    float temp_distance = euclidean_distance(abspos_x, abspos_y);
                    std::cout << temp_distance << std::endl;

                    if (temp_distance < closest_player)
                    {
                        foundEnemy = true;
                        //closest_player = temp_distance;

                        float azimuth_xy = atan2f(abspos_y, abspos_x);
                        float yaw = (float)(azimuth_xy * (180.0 / M_PI));

                        closest_yaw = yaw + 90;

                        if (abspos_y < 0) {
                            abspos_y *= -1;
                        }
                        if (abspos_y < 5) {
                            if (abspos_x < 0) {
                                abspos_x *= -1;
                            }
                            abspos_y = abspos_x;
                        }


                        float azimuth_z = atan2f(abspos_z, abspos_y);

                        closest_pitch = (float)(azimuth_z * (180.0 / M_PI));
                    }
                }
            }

            if (foundEnemy)
            {
                plr->viewAngles.yaw = closest_yaw;
                plr->viewAngles.pitch = closest_pitch;

                WPM<float>(hProcess, localplayer + 0x40, plr->viewAngles.yaw);
                WPM<float>(hProcess, localplayer + 0x44, plr->viewAngles.pitch);
            }
           
                
                
            


            // Execute logic for option two every tick
        }

    }

    

    delete[] enemy;

    // Close the process handle before exiting
    CloseHandle(hProcess);
    return 0;
}





