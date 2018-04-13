#include "Defs.h"
#include "p2Log.h"

#include "EnemyGuardTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"


EnemyGuardTower::EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyGuardTowerInfo(enemyGuardTowerInfo)
{
	texArea = &enemyGuardTowerInfo.completeTexArea;

	//Colliders
	CreateEntityCollider(EntitySide_Enemy);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, enemyGuardTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	sightRadiusCollider->isTrigger = true;
}

void EnemyGuardTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	//Check if building is destroyed
	if (currLife <= 0)
		towerState = TowerState_Die;

	//Check if tower has to attack or not
	if (isBuilt) {
		if (attackingTarget != nullptr && !enemyAttackList.empty())
			towerState = TowerState_Attack;
		else
			towerState = TowerState_Idle;
	}

	TowerStateMachine(dt);

	//Update animations for the construction cycle
	if (!isBuilt)
		UpdateAnimations(dt);

	//Check is building is built already
	if (!isBuilt && constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;

	//Check the arrow movement if the tower has to attack
	if (attackingTarget != nullptr && arrowParticle != nullptr)
		CheckArrowMovement(dt);

	//Check if the tower has to change the attacking target
	if (attackingTarget != nullptr && attackingTarget->GetCurrLife() <= 0) {

		attackingTarget = nullptr;
		enemyAttackList.pop_front();

		if (!enemyAttackList.empty())
			attackingTarget = enemyAttackList.front();
	}
}

void EnemyGuardTower::OnCollision(ColliderGroup * c1, ColliderGroup * c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		//Every time a enemy enters range it is added to the attack queue
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			LOG("Tower enter");
			enemyAttackList.push_back(c2->entity);

			if (attackingTarget == nullptr) {
				attackingTarget = enemyAttackList.front();
				attackTimer.Start();
			}
		}

		break;


	case CollisionState_OnExit:

		//Every time the enemy dies or exits sight this enemy is deleted from the atack queue
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			LOG("Tower exit");

			if (c2->entity == attackingTarget) {
				attackingTarget = nullptr;
				enemyAttackList.pop_front();
			}

			else if (c2->entity != attackingTarget) {
				enemyAttackList.remove(c2->entity);

			}

			if (!enemyAttackList.empty() && attackingTarget == nullptr) {
				attackingTarget = enemyAttackList.front();
				attackTimer.Start();

			}
		}

		break;

	}

}

void EnemyGuardTower::TowerStateMachine(float dt)
{
	switch (towerState) {
	case TowerState_Idle:
		//Nothing
		break;

	case TowerState_Attack:
	{
		if (attackingTarget != nullptr) {
			if (attackTimer.Read() >= (enemyGuardTowerInfo.attackWaitTime * 1000)) {

				attackTimer.Start();
				DetermineArrowDirection();
				CreateArrow();
				App->audio->PlayFx(24, 0); //Arrow sound
			}
		}
	}
	break;
	case TowerState_Die:
		//Nothing
		break;
	}
}

//Arrows
void EnemyGuardTower::DetermineArrowDirection()
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint towerTilePos = App->map->WorldToMap((int)this->GetPos().x, (int)this->GetPos().y);

	//Up
	if (targetTilePos.x == towerTilePos.x  && targetTilePos.y < towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP;

	//Down
	else if (targetTilePos.x == towerTilePos.x  && targetTilePos.y > towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN;

	//Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowDirection = LEFT;

	//Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowDirection = RIGHT;

	//Up Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP_LEFT;

	//Up Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP_RIGHT;

	//Down Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN_LEFT;

	//Down Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN_RIGHT;
}

void EnemyGuardTower::CreateArrow()
{
	switch (arrowDirection) {

	case UP:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.up, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.down, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.left, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.right, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case UP_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.upLeft, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case UP_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.upRight, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.downLeft, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.downRight, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	default:
		break;
	}

	float m = sqrtf(pow(attackingTarget->GetPos().x - arrowParticle->pos.x, 2.0f) + pow(attackingTarget->GetPos().y - arrowParticle->pos.y, 2.0f));
	arrowParticle->destination.x = (attackingTarget->GetPos().x - arrowParticle->pos.x) / m;
	arrowParticle->destination.y = (attackingTarget->GetPos().y - arrowParticle->pos.y) / m;
}

void EnemyGuardTower::CheckArrowMovement(float dt)
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint arrowTilePos = App->map->WorldToMap((int)arrowParticle->pos.x, (int)arrowParticle->pos.y);

	switch (arrowDirection) {
	case UP:
		if (arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN:
		if (arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case LEFT:
		if (arrowTilePos.x > targetTilePos.x)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x)
			InflictDamageAndDestroyArrow();
		break;

	case RIGHT:
		if (arrowTilePos.x < targetTilePos.x)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x)
			InflictDamageAndDestroyArrow();
		break;

	case UP_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case UP_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	default:
		break;
	}

}

void EnemyGuardTower::MoveArrowTowardsTarget(float dt)
{
	arrowParticle->pos.x += arrowParticle->destination.x * dt * enemyGuardTowerInfo.arrowSpeed;
	arrowParticle->pos.y += arrowParticle->destination.y * dt * enemyGuardTowerInfo.arrowSpeed;
}

void EnemyGuardTower::InflictDamageAndDestroyArrow()
{
	attackingTarget->ApplyDamage(enemyGuardTowerInfo.damage);
	arrowParticle->isRemove = true;
	arrowParticle = nullptr;
}

// Animations
void EnemyGuardTower::LoadAnimationsSpeed()
{

}
void EnemyGuardTower::UpdateAnimations(float dt)
{

}