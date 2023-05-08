#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "entities.h"
#include "filesfunc.h"
#include "maps.h"

Player player;
Player enemy;

//Inicjalizacja podstawowych statystyk jednostek
const Unit BaseUnits[] = {{'P','K',0,0,0,70,5,400,1,5,'0',0,5},
                            {'P','S',0,0,0,60,2,250,1,3,'0',0,2},
                            {'P','A',0,0,0,40,2,250,5,3,'0',0,2},
                            {'P','P',0,0,0,50,2,200,2,3,'0',0,2},
                            {'P','R',0,0,0,90,2,500,1,4,'0',0,2},
                            {'P','C',0,0,0,50,2,800,7,6,'0',0,2},
                            {'P','W',0,0,0,20,2,100,1,2,'0',0,2},
                            {'P','B',0,0,0,200,0,0,0,0,'0',0,0}};

void DisplayUnits(Unit *unitList, unsigned int amount)
{
    //Funkcja glownie do debugowania
    printf("\n\nWlasciciel \tJednostka \tID \tPozycja X \tPozycja Y \tWytrzymalosc \tProdukcja \tPunkty ruchu");
    for(int i=0;i<amount;i++){
        printf("\n%c \t\t%c \t\t%d \t\t%d \t\t%d \t\t%d \t\t%c \t\t%d",unitList[i].owner,unitList[i].unitType,unitList[i].ID,unitList[i].x,unitList[i].y,unitList[i].endurance,unitList[i].producedUnit,unitList[i].movePoints);
    }

}

Unit GetBaseUnit(char c){
    switch(c){
        case 'K':
            return BaseUnits[0];
            break;
        case 'S':
            return BaseUnits[1];
            break;
        case 'A':
            return BaseUnits[2];
            break;
        case 'P':
            return BaseUnits[3];
            break;
        case 'R':
            return BaseUnits[4];
            break;
        case 'C':
            return BaseUnits[5];
            break;
        case 'W':
            return BaseUnits[6];
            break;
        case 'B':
            return BaseUnits[7];
            break;
    }
}

void AddUnit(Player *player, Unit unit, int forcedId)
{
    uint16_t id = GrabUniqueUnitID(); //generuje unikalne ID

    /*ustawienie tego parametru na wartosc wieksza niz -1 spowoduje wymuszenie ID jednostki
    UWAGA: moze powodowac konflikty jednostek jesli nie uzyte poprawnie. Stosowane
    w celu aktualizacji jednostki poprzez usuniecie i dodanie jej na nowo.*/
    if (forcedId >= 0) id = forcedId;

    unit.owner = player->owner;
    unit.ID = id;
    (player->amountOfUnits)++;
    AddUnitToStatus(unit); //dodanie jednostki do status.txt
    player->units = realloc(player->units,sizeof(Unit)*(player->amountOfUnits));
    (player->units)[(player->amountOfUnits)-1] = unit;
}


void RemoveUnit(Player *player, uint16_t id){
    //Znajdowanie jednostki po ID
    for(int i=0;i<(player->amountOfUnits);i++){
        if ((player->units)[i].ID == id){
            //W przypadku usuniecia w innym miejscu niz koniec listy
            //przesuwa nastepujace rekordy o jedno miejsce wstecz
            //by zapelnic dziure w tablicy, nastepnie realokuje ją
            //żeby nie zostawiać smieci w pamięci
            RemoveUnitFromStatus(id);
            for(int j=i;j<(player->amountOfUnits);j++){
                if( (j+1) < (player->amountOfUnits) ){
                    (player->units)[j] = (player->units)[j+1];
                }
            }
            (player->amountOfUnits)--;
            player->units = realloc((player->units),sizeof(Unit)*(player->amountOfUnits));
        }
    }

}
uint8_t RemoveUnitByID(uint16_t id){
    Unit returnedUnit;
    if( GetUnitByIDFromStatus(id,&returnedUnit)){
        Player *ply;

        if(returnedUnit.owner == 'P') ply = &player;
        if(returnedUnit.owner == 'E') ply = &enemy;
        RemoveUnit(ply, id);
        return 1; //Usunieto poprawnie
    }
    return 0; //Nie znaleziono jednostki
}


void UpdateUnit(Unit unit){
    Player *ply;
    if(unit.owner == 'P') ply = &player;
    if(unit.owner == 'E') ply = &enemy;

    for(int i=0;i<(*ply).amountOfUnits;i++){
        if((*ply).units[i].ID == unit.ID){
            (*ply).units[i].x = unit.x;
            (*ply).units[i].y = unit.y;
            (*ply).units[i].endurance = unit.endurance;
            (*ply).units[i].producedUnit = unit.producedUnit;
            (*ply).units[i].order = unit.order;
            (*ply).units[i].movePoints = unit.movePoints;
            unit = (*ply).units[i];
            break;
        }
    }

    RemoveUnit(ply, unit.ID);
    AddUnit(ply, unit, unit.ID);

}

void SpawnUnit(Player *ply, char unit){
    Unit _unit = GetBaseUnit(unit);
    uint16_t x,y;
    GetBase(ply->owner,&x,&y);

    _unit.x = x;
    _unit.y = y;

    AddUnit(ply, _unit,-1);
}

void InitPlayer(char owner)
{
    //Inicjalizacja gracza
    Player *ply;
    if(owner == 'P') ply = &player;
    if(owner == 'E') ply = &enemy;

    (*ply).gold = 2000;
    (*ply).owner = owner;
    (*ply).amountOfUnits = 0;
    (*ply).canAttack = 1;
    (*ply).units =  malloc( sizeof(Unit) );

    SpawnUnit(ply, 'B');
}



