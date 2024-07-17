#pragma once
#include "entity.h"
#include <vector>
#include "mem.h"

#define M_PI 3.14159265358979323846

float euclidean_distance(float x, float y);

void getEntityInfo(Game* ac,Entity* plr);

void aimbot(Game* ac, std::vector<Entity>& enemy, Entity* plr);



