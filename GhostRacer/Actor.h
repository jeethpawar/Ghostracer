#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth);
	virtual void doSomething() = 0; // pure virtual since this changes completely depending on each type of actor
	virtual bool isCollisionAvoidanceWorthyActor() { return false; };
	virtual bool isDead() { return false; }; // virtual function returns false by default
	virtual ~Actor() {
		m_world = nullptr;
	};
	StudentWorld* getWorld() const;
	virtual bool isSprayable() { return false; }
	virtual void Spray() {
		return;
	}
private:
	StudentWorld* m_world;
};
class Agent : public Actor {
public:
	Agent(StudentWorld* world, int imageID,
		int startX, // column first
		int startY, // then row!
		int startDirection,
		double size,
		int depth, double sp, int hp);
	virtual void doSomething() = 0;
	virtual bool isDead() { return healthpoints <= 0; }
	double getSpeed() {
		return speed;
	}
	void setVerticalSpeed(double input) {
		speed = input;
	}
	virtual bool isCollisionAvoidanceWorthyActor() { return true; };
	int getHealth() {
		return healthpoints;
	}
	void takeDamage(int dmg) {
		healthpoints -= dmg;
	}
	bool isOverlappingWithGR();
	virtual ~Agent() {};
	void Heal(int x) {
		healthpoints += x;
		if (healthpoints > 100)
			healthpoints = 100;
	}
	virtual bool isSprayable() { return false; }

private:
	double speed;
	int healthpoints;
};

class GhostRacer : public Agent {
	public: 
		GhostRacer(StudentWorld* world, int imageID,
			int startX, // column first
			int startY, // then row!
			int startDirection,
			double size,
			int depth = 0);
		virtual void doSomething();
		virtual bool isCollisionAvoidanceWorthyActor() { return true; }
		virtual ~GhostRacer() {};
		void increaseTank(int x) {
			water_tank += x;
		}
		void decrementTank(int y) {
			water_tank -= y;
		}
		void spin();
		int getTank() {
			return water_tank;
		}
		virtual void Spray() {
			return;
		}
	private:
		int water_tank;
};

class BorderLine : public Actor {
	public:
		BorderLine(StudentWorld* world, int imageID, int startX, int startY);
		virtual void doSomething();
		virtual ~BorderLine() {}
		virtual bool isDead() { return isdead; }
		virtual void Spray() {
			return;
		}
	private:	
		const int border_speed = -4;
		bool isdead;
};

class Pedestrian : public Agent {
public:
	Pedestrian(StudentWorld* world, int imageID, int startX, int startY, int startDirection, double size, int depth, double horizontal_speed, int mvd);
	double getHorizSpeed() const { return horiz_speed; }
	void setHorizSpeed(double sp) { horiz_speed = sp; };
	void moveAndPossiblyPickPlan() {};
	virtual void doSomething();
	virtual bool isSprayable() { return true; }
	virtual ~Pedestrian() {};
	virtual void Spray();
private:
	double horiz_speed;
	int movement_plan_distance;
};
class HumanPedestrian : public Pedestrian {
public:
	HumanPedestrian(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual ~HumanPedestrian() {}
private:
};

class ZombiePedestrian : public Pedestrian {
public:
	ZombiePedestrian(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual void Spray();
	virtual ~ZombiePedestrian() {}
private:
	int tick;
};

class ZombieCab : public Agent {
public:
	ZombieCab(StudentWorld* world, int startX, int startY, double vert_sp);
	virtual void doSomething();
	virtual bool isSprayable() { return true; }
	virtual void Spray();
	virtual ~ZombieCab() {}
private:
	double horiz_speed;
	bool isAlreadyDamaged;
	int movement_plan_distance;
};

class GhostRacerActivatedObject : public Actor {
public:
	GhostRacerActivatedObject(StudentWorld* world, int imageID, int startX, int startY, int startDireciton, double size, int depth, double startVert, double startHoriz);
	virtual void doSomething();
	double getHorizSpeed() const { return horizontal_speed; }
	void setHorizSpeed(double sp) { horizontal_speed = sp; };
	double getVertSpeed() const { return vertical_speed; }
	void setVertSpeed(double sp) { vertical_speed = sp; }
	bool isGoodieOverlappingWithGr();
	virtual bool isDead() { return isdead; };
	virtual ~GhostRacerActivatedObject() {}
	void setDead() {
		isdead = true;
	}
	virtual bool isSprayable() { return false; }
	virtual void Spray() {
		return;
	}
private:
	double horizontal_speed;
	double vertical_speed;
	bool isdead;

};
class OilSlick : public GhostRacerActivatedObject {
public:
	OilSlick(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual ~OilSlick() {}
};


class HealingGoodie : public GhostRacerActivatedObject {
public:
	HealingGoodie(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual bool isSprayable() { return true; }
	virtual ~HealingGoodie() {}
	virtual void Spray() {
		setDead();
	}
};

class HolyWaterGoodie : public GhostRacerActivatedObject {
public:
	HolyWaterGoodie(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual bool isSprayable() { return true; }
	virtual ~HolyWaterGoodie() {}
	virtual void Spray() {
		setDead();
	}
};
class SoulGoodie : public GhostRacerActivatedObject{
public:
	SoulGoodie(StudentWorld * world, int startX, int startY);
	virtual ~SoulGoodie() {}
	virtual void doSomething();
};

class HolyWaterProjectile : public GhostRacerActivatedObject {
public:
	HolyWaterProjectile(StudentWorld* world, int startX, int startY, int startDirection);
	virtual void doSomething();
	virtual ~HolyWaterProjectile() {}
	bool isActorOverlappingWithHWP(Actor* a);
private:
	double maximum_travel_distance;
};

#endif // ACTOR_H_
