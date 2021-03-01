#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <vector>
#include <string>
class GhostRacer;
class Actor;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();
    StudentWorld();
    GhostRacer* getGhostracer() const;
    void removeDeadActors();
    void updateLastWhite(double n);
    std::string updateGameText();
    bool isActorInFrontOfCab(double x, double y);
    bool isActorBehindCab(double x, double y);
    int getLane(double x);
    int getLaneCenter(int x);
    int getSouls(int level) {
        return (level * 2) + 5;
    }
    void decrementSouls() {
        SoulsForLevel--;
    }
   bool HolyWaterAffectingActor(HolyWaterProjectile* a);
   void addSprayInFrontOfGr(double x, double y);
   void ChanceSpawnHealthGoodie(double x, double y);
   void ChanceSpawnOilSlick(double x, double y);
private:
    GhostRacer* m_ghostracer;
    std::vector<Actor*> actor_vect;
    double last_whiteborder_y;
    int bonusPts;
    int SoulsForLevel;
};

#endif // STUDENTWORLD_H_
