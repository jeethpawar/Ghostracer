#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_ghostracer = nullptr;
    actor_vect.clear();
    last_whiteborder_y = 0;
    bonusPts = 5000;
    SoulsForLevel = 1; //getSouls(getLevel());
}

int StudentWorld::init()
{
    SoulsForLevel = getSouls(getLevel());
    int i;
    m_ghostracer = new GhostRacer(this, IID_GHOST_RACER, 128, 32, 90, 4.0, 0);
    int n = VIEW_HEIGHT / SPRITE_HEIGHT;
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    //adding in yellow borderlines on the left
    for (int i = 0; i < n; i++) {
        actor_vect.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
    }
    //adding in yellow borderlins on the right
    for (int j = 0; j < n; j++) {
        actor_vect.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j * SPRITE_HEIGHT));
    }
    //adding in white borderlines
    int m = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    int x = LEFT_EDGE + ROAD_WIDTH / 3;
    // for left borderlines
    for (i = 0; i < m; i++) {
        actor_vect.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, x, i * (4 * SPRITE_HEIGHT)));
        last_whiteborder_y = i * (4 * SPRITE_HEIGHT);
    }
    x = RIGHT_EDGE - ROAD_WIDTH / 3;
    for (i = 0; i < m; i++) {
        actor_vect.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, x, i * (4 * SPRITE_HEIGHT)));
        last_whiteborder_y = i * (4 * SPRITE_HEIGHT);
    }
    // need to convert to oss           
    setGameStatText(updateGameText());
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_ghostracer->doSomething();
    if (m_ghostracer->isDead()) {
        playSound(SOUND_PLAYER_DIE);
        delete m_ghostracer;
        m_ghostracer = nullptr;
        decLives();
        SoulsForLevel = 1;
        return GWSTATUS_PLAYER_DIED;
    }
    vector<Actor>::iterator it;
    for (int i = 0; i < actor_vect.size(); i++) {
        if (!actor_vect[i]->isDead()){
            actor_vect[i]->doSomething();
            if (m_ghostracer->isDead()) {
                playSound(SOUND_PLAYER_DIE);
                delete m_ghostracer;
                m_ghostracer = nullptr;
                decLives();
                SoulsForLevel = getSouls(getLevel());
                return GWSTATUS_PLAYER_DIED;
            }
            else {
                if (SoulsForLevel <= 0) {
                    increaseScore(bonusPts);
                    delete m_ghostracer;
                    m_ghostracer = nullptr;
                    cleanUp();
                    return GWSTATUS_FINISHED_LEVEL;
                }
            }
        }
    }
    removeDeadActors();
    if (bonusPts > 0)
        bonusPts--;
    setGameStatText(updateGameText());
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - last_whiteborder_y;
    if (delta_y >= SPRITE_HEIGHT){
        actor_vect.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2, new_border_y));
        actor_vect.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT) {
        actor_vect.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y));
        actor_vect.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y));
        last_whiteborder_y = new_border_y;
    }
    
    // adding in human peds
    
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int ChanceGenerateHumanPed = randInt(0, ChanceHumanPed - 1);
    if (ChanceGenerateHumanPed == 0) {
        actor_vect.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), randInt(0, VIEW_HEIGHT)));
    }
    
    //adding in zombie peds
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int ChanceGenerateZombiePed = randInt(0, ChanceZombiePed - 1);
    if (ChanceGenerateZombiePed == 0) {
        actor_vect.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), randInt(0, VIEW_HEIGHT)));
    }
     // adding in zombie cabs
    
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int ChanceGenerateVehicle = randInt(0, ChanceVehicle - 1);
    if (ChanceGenerateVehicle == 0) {
        double init_speed = 0;
        int startX = 0;
        int startY = 0;
        bool addHere = false;
        int cur_lane = randInt(1, 3);
        for (int i = 0; i < 3; i++) {
            bool canAdd = true;
            for (int j = 0; j < actor_vect.size();j++) {
                if ((getLane(actor_vect[j]->getX()) == cur_lane && actor_vect[j]->isCollisionAvoidanceWorthyActor()) || getLane((m_ghostracer->getX()) == cur_lane)) { //if actor is in the lane and is marked as collision avoidance worthy or ghostracer is in the lane
                    if (actor_vect[j]->getY() <= VIEW_HEIGHT / 3 || getLane(m_ghostracer->getX()) == cur_lane) { // if the actor is too close to the bottom or ghostracer is in the lane(automatically too close) we cannot add
                        canAdd = false;
                    }
                }
            }
            if (canAdd) {
                init_speed = (m_ghostracer->getSpeed()) + randInt(2, 4);
                startX = getLaneCenter(cur_lane);
                startY = SPRITE_HEIGHT / 2;
                addHere = true;
                break;
            }
            canAdd = true;
            for (int j = 0; j < actor_vect.size(); j++) {
                if (getLane(actor_vect[j]->getX()) == cur_lane && actor_vect[j]->isCollisionAvoidanceWorthyActor()) {
                    if (actor_vect[j]->getY() >= (VIEW_HEIGHT * 2) / 3) // if the actor is too close to the top or ghostracer is in the lane(automatically too close) we cannot add
                        canAdd = false;
                }
            }
            if (canAdd) {
                init_speed = (m_ghostracer->getSpeed()) - randInt(2, 4);
                startX = getLaneCenter(cur_lane);
                startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                addHere = true;
                break;
            }
            cur_lane++;
            if (cur_lane > 3)
                cur_lane = 1;
        }
        if (addHere)
            actor_vect.push_back(new ZombieCab(this, startX, startY, init_speed));
        
    }
    
    // adding in goodies
    
    // oilslick
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int ChanceGenerateOilSlick = randInt(0, ChanceOilSlick - 1);
    if (ChanceGenerateOilSlick == 0) {
        actor_vect.push_back(new OilSlick(this, randInt(ROAD_CENTER - ROAD_WIDTH/2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));
    }
    // holy water refill
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int ChanceGenerateHolyWater = randInt(0, ChanceOfHolyWater - 1);
    if (ChanceGenerateHolyWater== 0) {
        actor_vect.push_back(new HolyWaterGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));
    }
    
    // lost soul goodieh
    int ChanceOfLostSoul = 100 + 10 * getLevel();
    int ChanceGenerateLostSoul = randInt(0, ChanceOfLostSoul - 1);
    if (ChanceGenerateLostSoul == 0) {
        actor_vect.push_back(new SoulGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (auto p : actor_vect)
        delete p;
    actor_vect.clear();
    m_ghostracer = nullptr;
    delete m_ghostracer;
}
StudentWorld :: ~StudentWorld(){
    cleanUp();
}

GhostRacer* StudentWorld::getGhostracer() const
{
    return m_ghostracer;
}

void StudentWorld::removeDeadActors()
{
    if (m_ghostracer->isDead()){
        delete m_ghostracer;
        m_ghostracer = nullptr;
    }
    vector<Actor*>::iterator it;
    for (it = actor_vect.begin(); it != actor_vect.end();) {
        if ((*it)->isDead()) {
            delete (*it);
            it = actor_vect.erase(it);
        }
        else {
            it++;
        }
    }
}

void StudentWorld :: updateLastWhite(double n) {
    last_whiteborder_y = n;
}

string StudentWorld :: updateGameText()
{
    ostringstream oss;
    oss.setf(ios::fixed);
    oss << "Score: " + to_string(getScore()) + "  Lvl: " + to_string(getLevel()) +
        "  Souls2Save: " + to_string(SoulsForLevel) + "  Lives: " +
        to_string(getLives()) + "  Health : " + to_string(m_ghostracer->getHealth()) +
        "  Sprays: " + to_string(m_ghostracer->getTank())+"  Bonus: " + to_string(bonusPts);
    string gameText = oss.str();
    return gameText;
}

bool StudentWorld::isActorInFrontOfCab(double x, double y)
{
    //Lane 1: [LEFT_EDGE, LEFT_EDGE + ROAD_WIDTH / 3) // Lane 2: [LEFT_EDGE + ROAD_WIDTH/3, LEFT_EDGE + (2 * ROAD_WIDTH)/3) // Lane 3:  [LEFT_EDGE + (2 * ROAD_WIDTH)/3, RIGHT_EDGE)
    for (int i = 0; i < actor_vect.size(); i++) {
        if (getLane(actor_vect[i]->getX()) == getLane(x) && ((actor_vect[i]->getY() - y) < 96) && ((actor_vect[i]->getY() - y) > 0) && actor_vect[i]->isCollisionAvoidanceWorthyActor())
            return true;
    }
    return false;
}
bool StudentWorld::isActorBehindCab(double x, double y)
{
    //Lane 1: [LEFT_EDGE, LEFT_EDGE + ROAD_WIDTH / 3) // Lane 2: [LEFT_EDGE + ROAD_WIDTH/3, LEFT_EDGE + (2 * ROAD_WIDTH)/3) // Lane 3:  [LEFT_EDGE + (2 * ROAD_WIDTH)/3, RIGHT_EDGE)
    for (int i = 0; i < actor_vect.size(); i++) {
        if (getLane(actor_vect[i]->getX()) == getLane(x) && ((y - actor_vect[i]->getY()) < 96) && ((y - actor_vect[i]->getY()) > 0) && actor_vect[i]->isCollisionAvoidanceWorthyActor())
            return true;
    }
    return false;
}

int StudentWorld::getLane(double x) //returns either 1(left), 2(middle), 3 (right) depedning on range x coordinate falls in
{
    if (x >= 1.0 * ROAD_CENTER - ROAD_WIDTH / 2 && x < (1.0 * ROAD_CENTER - ROAD_WIDTH / 2) + ROAD_WIDTH / 3) {
        return 1;
    }

    else if ((x >= (1.0 * ROAD_CENTER) - ROAD_WIDTH / 2) + ROAD_WIDTH / 3 && x < (1.0 * ROAD_CENTER - ROAD_WIDTH / 2) + (2 * ROAD_WIDTH) / 3) {
        return 2;
    }
    else if ((x >= (1.0 * ROAD_CENTER - ROAD_WIDTH / 2) + (2 * ROAD_WIDTH) / 3) && x < ((1.0 * ROAD_CENTER) + ROAD_WIDTH / 2)) {
        return 3;
    }
    return 0;
}

int StudentWorld::getLaneCenter(int x)  //returns the center of the lane based on the lane inputted
{
    if (x == 1)
        return ROAD_CENTER - ROAD_WIDTH / 3;
    if (x == 2)
        return ROAD_CENTER;
    if (x == 3)
        return ROAD_CENTER + ROAD_WIDTH / 3;
    return 0;
}

bool StudentWorld::HolyWaterAffectingActor(HolyWaterProjectile* a) // checks to see if the there are sprayable actors that need to be sprayed in student world 
{
    for (int i = 0; i < actor_vect.size(); i++) {
        if (actor_vect[i]->isSprayable() && (a->isActorOverlappingWithHWP(actor_vect[i]))){
            actor_vect[i]->Spray();
            return true;
        }
    }
    return false;

}

void StudentWorld::addSprayInFrontOfGr(double x, double y) //adds spray projectile
{
    actor_vect.push_back(new HolyWaterProjectile(this, x, y, m_ghostracer->getDirection()));
}

void StudentWorld::ChanceSpawnHealthGoodie(double x, double y) // adds health goodie 1/5th of the time
{
    int ChanceHealthGoodie = randInt(0, 4);
    if (ChanceHealthGoodie == 0) {
        actor_vect.push_back(new HealingGoodie(this, x, y));
    }
}

void StudentWorld::ChanceSpawnOilSlick(double x, double y) // adds oil slick 1/5th of the time
{
    int ChanceOilSlick = randInt(0, 4);
    if (ChanceOilSlick == 0) {
        actor_vect.push_back(new OilSlick(this, x, y));
    }
}



