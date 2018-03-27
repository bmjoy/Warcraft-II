#include "EnemyGuardTower.h"

EnemyGuardTower::EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyGuardTowerInfo(enemyGuardTowerInfo)
{
	texArea = &enemyGuardTowerInfo.completeTexArea;
}

void EnemyGuardTower::Move(float dt)
{

}

// Animations
void EnemyGuardTower::LoadAnimationsSpeed()
{

}
void EnemyGuardTower::UpdateAnimations(float dt)
{

}