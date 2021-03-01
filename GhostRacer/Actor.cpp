#include "GraphObject.h"
#include "Actor.h"
#include "StudentWorld.h"
#include <string>

using namespace std;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* world,
	int imageID,
	int startX, // column first
	int startY, // then row!
	int startDirection,
	double size,
	int depth) :GraphObject(imageID, startX, startY, startDirection, size, depth) {
	m_world = world;
}

StudentWorld* Actor::getWorld() const {
	return m_world; //used to access student world andthe ghostracer we play as
}

Agent::Agent(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth, double sp, int hp) :
	Actor(world, imageID, startX, startY, startDirection, size, depth)
{
	healthpoints = hp;
	speed = sp;
}
bool Agent::isOverlappingWithGR() //checks to see if the agent in question is in the radius of the GH
{
	double ghost_x = getWorld()->getGhostracer()->getX();
	double ghost_y = getWorld()->getGhostracer()->getY();
	double ped_x = getX();
	double ped_y = getY();
	double delta_x = abs(ghost_x - ped_x);
	double delta_y = abs(ghost_y - ped_y);
	double radius_sum = getRadius() + getWorld()->getGhostracer()->getRadius();
	if ((delta_x < radius_sum * .25) && (delta_y < radius_sum * .6)) {
		return true;
	}
	return false;
}

GhostRacer::GhostRacer(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth) :
	Agent(world, imageID, startX, startY, startDirection, size, 0, 0, 100)
{
	water_tank = 10; //starts off with 10 ammo
}


void GhostRacer::doSomething() {
	int ch;
	if (isDead())
		return;

	if (getX() <= ((1.0 * ROAD_CENTER) - ROAD_WIDTH / 2) && getDirection() > 90) { //if we hit left boundary
		takeDamage(10);
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= ((1.0 * ROAD_CENTER) + ROAD_WIDTH / 2) && getDirection() < 90) { //if we hit right boundary
		//Damage by 10 hitpoints
		takeDamage(10);
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}

	else if (getWorld()->getKey(ch)) {
		double delta_x; 
		double delta_y;
		double spray_x;
		double spray_y;
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (water_tank > 0) {
				delta_x = cos(getDirection() * 3.14159265 / 180);
				delta_y = sin(getDirection() * 3.14159265 / 180);
				spray_x = getX() + delta_x;
				spray_y = getY() + delta_y;
				getWorld()->addSprayInFrontOfGr(spray_x, spray_y); //helper function to add spray in student world
				getWorld()->playSound(SOUND_PLAYER_SPRAY); //spray 
				decrementTank(1);
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) {
				int i = getDirection();
				setDirection(i + 8);
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) {
				int i = getDirection();
				setDirection(i - 8);
			}
			break;
		case KEY_PRESS_UP:
			if (getSpeed() < 5)
				setVerticalSpeed(getSpeed() + 1);
			break;
		case KEY_PRESS_DOWN:
			if (getSpeed() > -1)
				setVerticalSpeed(getSpeed() - 1);
			break;
		}
	}
	double max_shift_per_tick = 4.0;
	int direction = getDirection();
	double delta_x = cos(direction * 3.14159265 / 180) * max_shift_per_tick; //has to be in radians
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::spin()
{
	int x = randInt(0, 1);
	if (x == 0) { // spin counterclockwise
		int directionset = getDirection() + randInt(5, 20);
		if (directionset > 120)
			setDirection(120);
		else
			setDirection(directionset);
	} 
	else { //spin clockwise
		int directionset = getDirection() - randInt(5, 20);
		if (directionset < 120)
			setDirection(60);
		else
			setDirection(directionset);
	}
}




BorderLine::BorderLine(StudentWorld* world, int imageID, int startX, int startY) : Actor(world, imageID, startX, startY, 0, 2.0, 1) {
	isdead = false;
}

void BorderLine::doSomething()
{


	double ghostSpeed = getWorld()->getGhostracer()->getSpeed();
	double vert_speed = border_speed - ghostSpeed;
	double new_y = getY() + vert_speed;
	moveTo(getX(), getY() + vert_speed);
	if (getX() == (1.0 * ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3) || getX() == (1.0 * ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3)) { //we have a white borderline
		getWorld()->updateLastWhite(new_y);
	}
	if (getX() > VIEW_WIDTH || getX() < 0 || new_y > VIEW_HEIGHT || new_y < 0) {
		isdead = true;
		return;
	}
}


Pedestrian :: Pedestrian(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth, double horizontal_speed, int mvd) : Agent(world, imageID, startX, startY, startDirection, size, depth, -4.0, 2)
{
	horiz_speed = horizontal_speed;
	movement_plan_distance = mvd;
}

void Pedestrian::doSomething()
{

	double vert_speed = getSpeed() - (getWorld()->getGhostracer()->getSpeed());
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (new_x > VIEW_WIDTH || new_x < 0 || new_y > VIEW_HEIGHT || new_y < 0) {
		takeDamage(10);
		return;
	}
	movement_plan_distance--;
	if (movement_plan_distance > 0)
		return;
	else {
		do {
			horiz_speed = randInt(-3, 3);
		} while (horiz_speed== 0); //do while loop ensures it cannot be 0
		movement_plan_distance = randInt(4, 32);
		if (horiz_speed < 0)
			setDirection(180);
		else
			setDirection(0);
	}
}
void Pedestrian::Spray()
{
	getWorld()->playSound(SOUND_PED_HURT);
	return;
}
HumanPedestrian :: HumanPedestrian(StudentWorld* world, int startX, int startY) : Pedestrian(world, IID_HUMAN_PED, startX, startY, 0, 2.0, 0, 0, 0){
}
void HumanPedestrian::doSomething() {
	if (isDead())
		return;
	if (isOverlappingWithGR()) {
		//getWorld()->decLives();
		getWorld()->getGhostracer()->takeDamage(200);
	}
	Pedestrian::doSomething();
}
ZombiePedestrian::ZombiePedestrian(StudentWorld* world, int startX, int startY) : Pedestrian(world, IID_ZOMBIE_PED, startX, startY, 0, 3.0, 0, 0, 0) {
	tick = 0;
}
void ZombiePedestrian::doSomething() {
	if (isDead())
		return;
	if (isOverlappingWithGR()) { //if we hit zombie ped we need to kill it and take damage
		//getWorld()->decLives();
		getWorld()->getGhostracer()->takeDamage(5);
		takeDamage(2);
		getWorld()->playSound(SOUND_PED_DIE);
		return;
	}
	if ((abs(getX() - getWorld()->getGhostracer()->getX()) <= 30) && (getY() > getWorld()->getGhostracer()->getY())) {
		setDirection(270);
		if (getX() < getWorld()->getGhostracer()->getX()) //zombie is to the left
			setHorizSpeed(1);
		else if (getX() > getWorld()->getGhostracer()->getX()) //zombie is to the right
			setHorizSpeed(-1);
		else
			setHorizSpeed(0);
		tick--;
		if (tick <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			tick = 20;
		}
	}
	Pedestrian::doSomething();
	//cout << getHorizSpeed() << endl;
}

void ZombiePedestrian::Spray()
{
	takeDamage(1);
	getWorld()->playSound(SOUND_PED_HURT);
	if (isDead())
		getWorld()->ChanceSpawnHealthGoodie(getX(), getY());
}


ZombieCab::ZombieCab(StudentWorld* world, int startX, int startY, double vert_sp) : Agent(world, IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0, vert_sp, 3)
{
	horiz_speed = 0;
	isAlreadyDamaged = false; //bool so we dont have the interaction happenign multiple times per tick
	movement_plan_distance = 0; 
}

void ZombieCab::doSomething()
{
	if (isDead())
		return;
	if (isOverlappingWithGR() && !isAlreadyDamaged){
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getWorld()->getGhostracer()->takeDamage(20);
		if (getX() <= getWorld()->getGhostracer()->getX()) { //zombie is to the left
			horiz_speed = -5;
			setDirection(120 + randInt(0, 19));
		}
		else {
			horiz_speed = 5;
			horiz_speed = -5;
			setDirection(60 - randInt(0, 19));
		}
		isAlreadyDamaged = true;
	}
	double vert_speed = getSpeed() - (getWorld()->getGhostracer()->getSpeed());
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (new_x > VIEW_WIDTH || new_x < 0 || new_y > VIEW_HEIGHT || new_y < 0) {
		takeDamage(200); 
		return;
	}
	if (getSpeed() > getWorld()->getGhostracer()->getSpeed()) {
		if (getWorld()->isActorInFrontOfCab(getX(), getY())) {
			setVerticalSpeed(getSpeed() - 0.5); //so cab can go slower than aisha
			return;
		}	
	}
	if (getSpeed() <= getWorld()->getGhostracer()->getSpeed()) {
		if (getWorld()->isActorBehindCab(getX(), getY())) {
			setVerticalSpeed(getSpeed() + 0.5); //so cab can go faster than aisha
			return;
		}
	}
	movement_plan_distance--;
	if (movement_plan_distance > 0)
		return;
	else {
		movement_plan_distance = randInt(4, 32);
		setVerticalSpeed(getSpeed() + randInt(-2, 2));
	}
}

void ZombieCab::Spray()
{
	takeDamage(1); //takes damage from spray
	
	if (isDead())
		getWorld()->ChanceSpawnOilSlick(getX(), getY()); //helper function to generate oil slick 1/5th the time
}

GhostRacerActivatedObject :: GhostRacerActivatedObject(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth, double startVert, double startHoriz) : Actor(world, imageID, startX, startY, startDirection, size, depth)
{
	horizontal_speed = startHoriz;
	vertical_speed = startVert;
	isdead = false; //switchh back to bool for keeping track of dead or alive status
}

void GhostRacerActivatedObject::doSomething()
{
	double vert_speed = getVertSpeed() - (getWorld()->getGhostracer()->getSpeed());
	double horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (new_x > VIEW_WIDTH || new_x < 0 || new_y > VIEW_HEIGHT || new_y < 0) {
		isdead = true;
		return;
	}
}

bool GhostRacerActivatedObject::isGoodieOverlappingWithGr() //helps to see if goodies is in contact with GR
{
	double ghost_x = getWorld()->getGhostracer()->getX();
	double ghost_y = getWorld()->getGhostracer()->getY();
	double goodie_x = getX();
	double goodie_y = getY();
	double delta_x = abs(ghost_x - goodie_x);
	double delta_y = abs(ghost_y - goodie_y);
	double radius_sum = getRadius() + getWorld()->getGhostracer()->getRadius();
	if ((delta_x < radius_sum * .25) && (delta_y < radius_sum * .6)) {
		return true;
	}
	return false;
}


HealingGoodie::HealingGoodie(StudentWorld* world, int startX, int startY) : GhostRacerActivatedObject(world, IID_HEAL_GOODIE, startX, startY, 0, 1.0, 2, -4, 0)
{

}

void HealingGoodie::doSomething()
{
	GhostRacerActivatedObject::doSomething();
	if (isGoodieOverlappingWithGr()) {
		getWorld()->getGhostracer()->Heal(10);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
	}
}
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* world, int startX, int startY) : GhostRacerActivatedObject(world, IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, 2, -4, 0) {

}

void HolyWaterGoodie::doSomething()
{
	GhostRacerActivatedObject::doSomething();
	if (isGoodieOverlappingWithGr()) {
		getWorld()->getGhostracer()->increaseTank(10);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
	}
}

SoulGoodie::SoulGoodie(StudentWorld* world, int startX, int startY) : GhostRacerActivatedObject(world, IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2, -4, 0)
{

}

void SoulGoodie::doSomething()
{
	GhostRacerActivatedObject::doSomething();
	if (isGoodieOverlappingWithGr()) {
		getWorld()->decrementSouls();
		setDead();
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
	}
	setDirection(getDirection() + 10);
}

OilSlick::OilSlick(StudentWorld* world, int startX, int startY) : GhostRacerActivatedObject(world, IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2, -4, 0)
{

}

void OilSlick::doSomething()
{
	GhostRacerActivatedObject::doSomething();
	if (isGoodieOverlappingWithGr()) {
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getGhostracer()->spin();
	}
}

HolyWaterProjectile::HolyWaterProjectile(StudentWorld* world, int startX, int startY, int startDirection) : GhostRacerActivatedObject(world, IID_HOLY_WATER_PROJECTILE, startX, startY, startDirection, 1.0, 1, 0, 0)
{
	maximum_travel_distance = 0;
}

void HolyWaterProjectile::doSomething()
{
	if (isDead())
		return;
	
	if (getWorld()->HolyWaterAffectingActor(this)) {
		setDead();
		return;
	}
	
	GraphObject::moveForward(SPRITE_HEIGHT); //different move function than the rest of the goodies
	maximum_travel_distance += SPRITE_HEIGHT;
	if (maximum_travel_distance >= 160) {
		setDead();
		return;
	}
	if (getX()> VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT || getY() < 0) {
		setDead();
		return;
	}

}

bool HolyWaterProjectile::isActorOverlappingWithHWP(Actor* a) //helper function to see if the actor is overlapping with specifically a Holy Water Projectile. THis is called through student world
{
	double act_x = a->getX();
	double act_y = a->getY();
	double goodie_x = getX();
	double goodie_y = getY();
	double delta_x = abs(act_x - goodie_x);
	double delta_y = abs(act_y - goodie_y);
	double radius_sum = getRadius() + getWorld()->getGhostracer()->getRadius();
	if ((delta_x < radius_sum * .25) && (delta_y < radius_sum * .6)) {
		return true;
	}
	return false;
}

