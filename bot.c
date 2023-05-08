#include <stdint.h>
#include "orders.h"
#include "bot.h"
#include "entities.h"
#include "status.h"

//bardzo prymitywny bot, jego strategia to atak bazy i zawsze posiadanie workera w kopalni
uint8_t ProcessBot(){
    if (CheckForWorker() == 0){
        OrderData order;
        order.action = 'B';
        order.unitToBuild = 'W';
        order.owner = 'E';
        Build(order);
    }
    else{


        uint16_t x,y,baseX,baseY;
        for (int i=0;i<enemy.amountOfUnits;i++){
            //szukanie kopalni i wysylanie tam workera
            if(enemy.units[i].unitType == 'B'){
                baseX=enemy.units[i].x;
                baseY = enemy.units[i].y;
            }
        }
        GetClosestMine(baseX, baseY,&x, &y);
        MoveAlgorithm(CheckForWorker(),x,y);


        for (int i=0;i<enemy.amountOfUnits;i++){
            if(enemy.units[i].unitType != 'B' & enemy.units[i].unitType != 'W'){
                if(enemy.units[i].movePoints <= 0) continue;
                uint16_t enemyBaseX,enemyBaseY;
                uint16_t baseID;
                GetEnemyBaseData(&enemyBaseX, &enemyBaseY,&baseID);
                //jesli jest przy bazie, atakuj baze
                if(GetDistance(enemy.units[i].x,enemyBaseX,enemy.units[i].y,enemyBaseY) <= enemy.units[i].attackRange){
                    OrderData order;
                    order.action = 'A';
                    order.id = enemy.units[i].ID;
                    order.attackId = baseID;
                    order.owner = 'E';
                    uint8_t retVal = Attack(order);
                    if (retVal == 3) return 3;
                }
                //jesli nie, rusz sie w strone bazy
                else{

                    MoveAlgorithm(enemy.units[i].ID,enemyBaseX,enemyBaseY);
                }
            }
        }



        //produkowanie knightow i szybki atak
        OrderData order;
        order.action = 'B';
        order.unitToBuild = 'K';
        order.owner = 'E';
        Build(order);
    }
    return 0;
}

void GetEnemyBaseData(uint16_t *x, uint16_t *y, uint16_t *id){
    Unit* units = malloc(sizeof(Unit));
    unsigned int amount;

    GetUnitsFromStatus(&units,&amount,'P');
    for(int i=0;i<amount;i++){
        if (units[i].unitType == 'B'){
            *x = units[i].x;
            *y = units[i].y;
            *id = units[i].ID;
            break;
        }
    }
}

uint16_t CheckForWorker(){ //sprawdzanie czy bot ma workera, jesli nie to produkuje go by zdobyc zloto. Przy okazji zwraca jego id.
    for (int i=0;i<enemy.amountOfUnits;i++){
        if(enemy.units[i].unitType == 'W'){
            return enemy.units[i].ID;
        }
    }
    return 0;
}

/*sprawdzenie czy baza cos produkuje, lub dopiero zaczela produkowac.
bot nie zmieni produkcji jesli produkuje jednostke dluzej niz ture.
*/
uint8_t IsCurrentlyProducing(){
    for (int i=0;i<enemy.amountOfUnits;i++){
        if(enemy.units[i].unitType == 'B'){
            if(enemy.units[i].producedUnit == '0') return 0;
            Unit u = GetBaseUnit(enemy.units[i].producedUnit);
            if (u.buildTime == enemy.units[i].buildTime) return 0;

            return 1;
        }
    }
}


//bardzo prymitywny algorytm ruchu
void MoveAlgorithm(uint16_t id,uint16_t targetX,uint16_t targetY){
    Unit unit;
    GetUnitByID('E',id,&unit);

    uint16_t bestX = unit.x;
    uint16_t bestY = unit.y;
    uint16_t bestDist = GetDistance(bestX,targetX,bestY,targetY);
    for(int x = -unit.movePoints;x<=unit.movePoints;x++){
        for(int y = -unit.movePoints;y<=unit.movePoints;y++){

            if ((GetDistance(targetX,(int)unit.x+x,targetY,(int)unit.y + y) < bestDist) &
                (GetTile((int)unit.x + x,(int)unit.y + y) != 0) &
                (GetTile((int)unit.x+x,(int)unit.y+y) != 9) &
                (GetDistance(unit.x,(int)unit.x+x,unit.y ,(int)unit.y + y) <= unit.movePoints)){
                    bestDist = GetDistance(targetX,(int)unit.x+x,targetY,(int)unit.y + y);
                    bestX = (int)unit.x+x;
                    bestY = (int)unit.y+y;
                }
        }
    }


    OrderData order;
    order.action = 'M';
    order.x = bestX;
    order.y = bestY;
    order.id = id;
    order.owner = 'E';
    Move(order);
}
