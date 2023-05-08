#include <stdio.h>
#include <stdlib.h>
#include "maps.h"
#include "entities.h"
#include "orders.h"
#include "filesfunc.h"
#include <sys/stat.h>

uint16_t turn = 0;

void StartGame(int timeLimit){
    InitPlayer('P'); //inicjalizacja gracza
    InitPlayer('E'); //inicjalizacja wroga
    printf("\nGra Rozpoczeta!");
    uint8_t turnStatus;
    if(timeLimit == -1) timeLimit = 5; //domyslnie jesli nie podano czasu rundy, ustawione jest 5s
    const unsigned long maxRoundTime = timeLimit; //maksymalny czas na wykonanie ruchu

    do {
        turnStatus = 0;
        turn++;
        OnTurnBegin('P');
        OnTurnBegin('E');
        printf("\nTura: %d",turn);

        //Kod do rozpoczynania nastepnej tury po modyfikacji pliku status.txt
        struct stat file_stat;
        if (stat(ordersFileName, &file_stat) != 0) {
            printf("BLAD: Problem z plikiem %s",ordersFileName);
            return;
        }


        time_t lastModTime = file_stat.st_mtime;
        time_t currModTime = time(NULL)+1;

        /*dodanie jednej sekundy to fix, poniewaz program mogl sie wykonac pare razy w sekunde, tym samym
        wykonujac pare tur, z racji ze czas ograniczony jest do unixowego w sekundach*/

        unsigned long roundTime = (unsigned long)time(NULL); //kalkulacja czasu wykonania tury
        while(lastModTime < currModTime ){
            if (stat(ordersFileName, &file_stat) != 0) {
                printf("BLAD: Problem z plikiem %s",ordersFileName);
                return;
            }
            lastModTime = file_stat.st_mtime;
            if(((unsigned long)time(NULL) - roundTime) > maxRoundTime) {
                printf("\nMinal czas");
                turnStatus = 1; //stracona tura
                break;
            }
        }


        if(turnStatus != 1)
            turnStatus = ReadOrders(); //Tura Gracza

        if(turnStatus == 1) printf("\nStracono ture!");



        if(turnStatus == 2) {
            printf("\nWygrales gre!");
            break;
        }

        turnStatus = ProcessBot(); //tura bota

        if(turnStatus == 3) {
            printf("\nPrzegrales gre - wygral BOT!");
            break;
        }

    } while (turn < 1000);
    if(turnStatus < 2) printf("\nPrzekroczono ilosc tur! REMIS!");
}

void OnTurnBegin(char owner){
    Player *ply;
    if(owner == 'P') ply = &player;
    if(owner == 'E') ply = &enemy;

    //Przywracanie punktow ruchu co runde
    ply->canAttack = 1;
    for(int i=0;i<ply->amountOfUnits;i++){
        Unit _unit;
        _unit = GetBaseUnit(ply->units[i].unitType);
        ply->units[i].movePoints = _unit.movePoints;
    }

    Unit *unitList = malloc(sizeof(Unit));
    unsigned int amount;
    GetUnitsFromStatus(&unitList,&amount,owner);
    for (int i=0;i<amount;i++){
        if((GetTile(unitList[i].x,unitList[i].y) == '6') & (unitList[i].unitType == 'W')){
            AddGold(50,owner); //dodawanie 50 golda co ture za workera
            printf("\nSTATUS: Jednostka %c ID: %d wyprodukowala 50 zlota",unitList[i].unitType,unitList[i].ID);
        }
    }

}

void AddGold(uint16_t gold, char owner){

    Player *ply;
    if(owner == 'P') ply = &player;
    if(owner == 'E') ply = &enemy;

    ply->gold = ply->gold + gold;


    if(owner == 'P'){
        RemoveLineFromFile(statusFileName,0);
        FILE* file = fopen(statusFileName,"r");
        FILE* tempFile = fopen("temp.txt","w");
        if(file == NULL || tempFile == NULL) {
            printf("BLAD ODCZYTU PLIKU - ZRESTARTUJ PROGRAM");
            return;
        }


        fprintf(tempFile,"%d",ply->gold);
        char c;
        while((c=getc(file))!= EOF){
                fprintf(tempFile,"%c",c);
        }

        fclose(file);
        fclose(tempFile);

        remove(statusFileName);
        rename("temp.txt",statusFileName);
    }


}

void ClearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
