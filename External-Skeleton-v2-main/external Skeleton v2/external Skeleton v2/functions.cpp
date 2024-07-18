#include "functions.h"

using namespace mem;

float euclidean_distance(float x, float y)
{
    return sqrtf((x * x) + (y * y));
}

void getEntityInfo(Game* game, Entity* ent)
{
    ent->origin = RPM<Vec3>(game->hProcess, ent->baseAddress + ORIGIN);
    ent->viewAngles = RPM<Vec2>(game->hProcess, ent->baseAddress + VIEW_ANGLES);
    ent->teamNum = RPM<int>(game->hProcess, ent->baseAddress + TEAM_NUMBER);
    ent->isDead = RPM<int>(game->hProcess, ent->baseAddress + IS_DEAD);
}

void aimbot(Game* game, std::vector<Entity>& enemy, Entity* plr)
{
    float closest_player = 20.0f; // Radius
    float closest_yaw = 0.0f;
    float closest_pitch = 0.0f;

    int numPlayer = RPM<int>(game->hProcess, game->moduleBase + NUMBER_OF_PLAYERS);
    enemy.resize(numPlayer);

    bool foundEnemy = false;

    for (int i = 0; i < numPlayer; ++i)
    {
        enemy[i].baseAddress = RPM<uintptr_t>(game->hProcess, game->entitylist + (i * ENTITY_STRUCT_SIZE));
        getEntityInfo(game, &enemy[i]);

        if (!enemy[i].isDead && enemy[i].teamNum != plr->teamNum)
        {
            float abspos_x = enemy[i].origin.x - plr->origin.x;
            float abspos_y = enemy[i].origin.y - plr->origin.y;
            float abspos_z = enemy[i].origin.z - plr->origin.z;

            float temp_distance = euclidean_distance(abspos_x, abspos_y);

            if (temp_distance <= closest_player)
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

        WPM<float>(game->hProcess, plr->baseAddress + 0x40, plr->viewAngles.yaw);
        WPM<float>(game->hProcess, plr->baseAddress + 0x44, plr->viewAngles.pitch);
    }

}
