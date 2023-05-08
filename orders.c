#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "entities.h"
#include "filesfunc.h"
#include "orders.h"
#include "status.h"


void InitOrdersFile(){
    FILE* ordersFile = fopen(ordersFileName,"w");
    if(ordersFile == NULL) {
        printf("\nERROR: NIE MOZNA STWORZYC PLIKU, ZRESTARTUJ PROGRAM LUB ZMIEN LOKALIZACJE PROGRAMU LUB PRZYWILEJE");
        return;
    }
    fclose(ordersFile);

}


uint8_t IsNumbersOnly(char chars[],int amount){
    for (int i=0;i<amount;i++){
        //sprawdzanie zakresu numerow w ASCII)
        if ((chars[i] < 48) & (chars[i] > 57) & (chars[i] != 0)) return 0;
    }
    return 1;
}
uint8_t CheckCorrectCharFormat(char chars[],int amount){
    if((chars[0]<65) || (chars[0]>90)) return 0;//Sprawdza czy pierwszy znak to litera

    for (int i=1;i<amount;i++){
        if(chars[i] != 0) return 0;
    }
    return 1;
}

uint8_t ReadOrders(){
    FILE* ordersFile = fopen(ordersFileName,"r");
    char c;
    char previousC = 0; //poprzednie dane znaku C, w celu np traktowania wielu odstepow po kolei jako jeden (by ulatwic wpisywanie danych graczowi)

    uint8_t chars[4] = {0,0,0,0}; // tablica znakow w przypadku odczytywania liczb, np 200 zlota to tablica '2','0','0';
    uint8_t readingIndex = 0; //ktory znak tablicy chars[] jest wczytywany
    uint8_t currParameter = 0; //ktory parametr jest odczytywany wg statusu - np. parametr 0 to ID

    OrderData orderData;
    orderData.owner = 'P'; //ustawienie wlasciciela jako gracza, gdyz to tylko jego rozkazy sa w pliku rozkazy.txt
    do {
        c=getc(ordersFile);
        if((previousC == c) & (c == ' ') || ((previousC=='\n')&(c==EOF)) ) continue; //ignorowanie paru odstepow pod rzad - traktowanie ich jako jeden

        if ((c==' ') || (c=='\n') || (c==EOF)){
            switch(currParameter){ //odczytywanie poszczegolnych parametrow, np w przypadku rozkazu 1 M 1 1, sa 4 parametry (id, rozkaz, x,y)
                case 0:
                    if(IsNumbersOnly(chars,4) == 0) return 1;
                    orderData.id = atoi(chars);
                    break;
                case 1:
                    if(CheckCorrectCharFormat(chars,4) == 0) return 1;
                    if((chars[0] != 'M')&(chars[0] != 'A')&(chars[0] != 'B')) return 1; //niedozwolona akcja = strata tury
                    orderData.action = chars[0];
                    break;
                case 2:
                    switch(orderData.action){
                        case 'M':
                            //Parametr X dla akcji M
                            if(IsNumbersOnly(chars,4) == 0) return 1;
                            orderData.x = atoi(chars);
                            break;
                        case 'A':
                            if(IsNumbersOnly(chars,4) == 0) return 1;
                            orderData.attackId = atoi(chars);
                            break;
                        case 'B':
                            if(CheckCorrectCharFormat(chars,4) == 0) return 1;
                            if((chars[0] != 'K')&(chars[0] != 'S')&(chars[0] != 'A')
                               &(chars[0] != 'P')&(chars[0] != 'C')&(chars[0] != 'R')
                               &(chars[0] != 'W')) return 1; //niedozwolona akcja = strata tury
                            orderData.unitToBuild = chars[0];
                            break;
                    }
                    break;
                case 3:
                    if ((orderData.action == 'A') || (orderData.action == 'B')) return 1; //W przypadku jakby wprowadzono wiecej argumentow, strata tury.
                    //Parametr Y dla akcji M
                    if(orderData.action == 'M'){
                        if(IsNumbersOnly(chars,4) == 0) return 1;
                        orderData.y = atoi(chars);
                    }
                    break;
                case 4:
                    //W przypadku jakby wprowadzono wiecej argumentow, strata tury.
                    return 1;
                    break;
            }



            currParameter++;
            if((c=='\n') || (c==EOF)){
                uint8_t retVal = ProcessOrder(orderData);
                if(retVal) return retVal;
                currParameter = 0;
            }
            if(readingIndex > 0){

                readingIndex = 0;
                for (int i=0;i<4;i++) chars[i] = 0;
                previousC = c;
                continue;
            }
        }
        if(readingIndex > 3) return 1; //wyjscie poza zakres znakow = strata rundy
        chars[readingIndex] = c;
        readingIndex++;

        previousC = c;
    } while (c != EOF);

    fclose(ordersFile);
    return 0;
}

uint8_t ProcessOrder(OrderData orderData){
    uint8_t retVal;
    switch(orderData.action){
        case 'M':;
             retVal = Move(orderData);
            if(retVal) return retVal;
        break;
        case 'A':;
            retVal = Attack(orderData);
            if(retVal) return retVal;
        break;
        case 'B':;
            retVal = Build(orderData);
            if(retVal) return retVal;
        break;
    }
    return 0;
}

uint8_t Move(OrderData orderData){

    Unit unit;

    if (GetUnitByID(orderData.owner,orderData.id,&unit)){
        uint16_t moveCost = GetDistance(unit.x,orderData.x,unit.y,orderData.y);
        if ((moveCost > unit.movePoints) ||
            (GetTile(orderData.x,orderData.y) == '9') || (GetTile(orderData.x,orderData.y) == 0)) return 1;
        /*Za malo punktow ruchu, staniecie na barierce, wyjscie poza mape = strata ruchu*/

        if(unit.unitType == 'B') return 1; //nie mozna ruszyc bazy

        unit.movePoints -= moveCost;
        unit.x = orderData.x;
        unit.y = orderData.y;
        printf("\nSTATUS: %c ID: %d ruszyl sie na X:%d Y:%d",unit.unitType,unit.ID,unit.x,unit.y);
        UpdateUnit(unit); //aktualizacja jednostki w liscie i statusie

    }
    else {
        return 1; //Zle wpisane id = strata tury
    }
    return 0;
}

const uint8_t attackTable[8][8] =  {{35,35,35,35,35,50,35,35},
                                    {30,30,30,20,20,30,30,30},
                                    {15,15,15,15,10,10,15,15},
                                    {35,15,15,15,15,10,15,10},
                                    {40,40,40,40,40,40,40,50},
                                    {10,10,10,10,10,10,10,50},
                                    {5,5,5,5,5,5,5,1}};
const char attackTableUnits[8] = {'K','S','A','P','C','R','W'};

uint8_t GetTableUnitIndex(char unit){
    for(int i=0;i<8;i++){
        if(attackTableUnits[i] == unit){
            return i;
        }
    }
}

uint8_t Attack(OrderData orderData){
    Unit attacker,victim;
    Player *ply;
    if(orderData.owner == 'P') ply = &player;
    if(orderData.owner == 'E') ply = &enemy;

    if(!GetUnitByID(orderData.owner,orderData.id,&attacker)) return 1; //nie znaleziono jednostki? - strata tury
    if(!GetUnitByIDFromStatus(orderData.attackId,&victim)) return 1; //nie znaleziono jednostki? - strata tury
    if(attacker.owner == victim.owner) return 1; //samobój - strata tury
    if((*ply).canAttack == 0) return 1; //jesli juz atakowal - strata tury
    if(attacker.movePoints == 0) return 1; //jesli nie ma punktow ruchu - strata tury
    if(GetDistance(attacker.x,attacker.x,victim.y,victim.y) > attacker.attackRange) return 1; //atak poza zasiegiem - strata tury



    (attacker.movePoints)--;
    ply->canAttack = 0;
    uint8_t dmg = attackTable[GetTableUnitIndex(attacker.unitType)][GetTableUnitIndex(victim.unitType)];
    int resultHealth = victim.endurance - dmg;
    printf("\nSTATUS: %c ID: %d zaatakowal wroga %c ID: %d za %d DMG",attacker.unitType,attacker.ID,victim.unitType,victim.ID,dmg);
    if (resultHealth <= 0)
    {
        printf("\nSTATUS: Jednostka %c ID: %d zostala zniszczona",victim.unitType,victim.ID);
        RemoveUnitByID(victim.ID);
        if((victim.unitType == 'B') & (attacker.owner == 'P')) return 2; //Zwraca 2 jesli wygra Gracz
        if((victim.unitType == 'B') & (attacker.owner == 'E')) return 3; //Zwraca 3 jesli wygra Wrog
    }
    else{
        victim.endurance = resultHealth;
        UpdateUnit(victim);
    }
    return 0;
}

uint8_t Build(OrderData orderData){

    /* Kod wyszukuje instancji bazy dla poszczegolnego gracza, sprawdza co produkuje,
        jesli nic nie produkuje lub produkuje co innego niz aktualny rozkaz nakazuje,
        zmienia swoja produkcje i resetuje timer budowania.
        Dekrementuje timer co runde, gdy osiagnie 0, spawni jednostke i updatuje status.txt
    */
    Player *ply;
    if(orderData.owner == 'P') ply = &player;
    if(orderData.owner == 'E') ply = &enemy;
    for (int i=0;i<((*ply).amountOfUnits);i++){
        if((*ply).units[i].unitType == 'B'){
            if((*ply).units[i].producedUnit != orderData.unitToBuild){
                (*ply).units[i].producedUnit = orderData.unitToBuild;
                (*ply).units[i].buildTime = GetBaseUnit(orderData.unitToBuild).buildTime;
            }
            else{
                (*ply).units[i].producedUnit = orderData.unitToBuild;
                ((*ply).units[i].buildTime)--;
                if ((*ply).units[i].buildTime <= 0){
                    SpawnUnit(ply,orderData.unitToBuild);
                    (*ply).units[i].producedUnit = '0';
                    printf("\nSTATUS: Baza %c wyprodukowala jednostke %c",(*ply).owner,orderData.unitToBuild);
                }
                UpdateUnit((*ply).units[i]);
            }
            break;
        }
    }

    return 0;
}

uint16_t GetDistance(int x1,int x2,int y1,int y2){
    return abs(x1-x2)+abs(y1-y2);
}
