#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "entities.h"
#include "filesfunc.h"



void InitStatusFile(){
    FILE* statusFile = fopen(statusFileName,"w");
    if(statusFile == NULL) {
        printf("ERROR: NIE MOZNA STWORZYC PLIKU, ZRESTARTUJ PROGRAM LUB ZMIEN LOKALIZACJE PROGRAMU LUB PRZYWILEJE");
        return;
    }
    fprintf(statusFile,"%d\n",2000); //Poczatkowe Zloto
    fclose(statusFile);

}

void AddUnitToStatus(Unit u){
    FILE* statusFile = fopen(statusFileName,"a");
    if(statusFile == NULL) {
        printf("ERROR: STATUS.TXT NIE ISTNIEJE - ZRESTARTUJ PROGRAM");
        return;
    }
    fprintf(statusFile,"%c %c %d %d %d %d %c\n",u.owner,u.unitType,u.ID,u.x,u.y,u.endurance,u.producedUnit);
    fclose(statusFile);
}

void RemoveUnitFromStatus(uint16_t id){
    FILE* statusFile = fopen(statusFileName,"r");
    if(statusFile == NULL) {
        printf("ERROR: STATUS.TXT NIE ISTNIEJE - ZRESTARTUJ PROGRAM");
        return;
    }

    //4 znakowa tablica do wczytywania numerow
    char chars[4] = {0,0,0,0};
    uint8_t idIndex = 0;
    uint8_t hasStartedReading = 0;
    uint8_t currParameter = 0;
    uint16_t readID;
    int lineNo = 0;
    char c;
    while((c=getc(statusFile))!= EOF){
        if(hasStartedReading){
            if (currParameter == 2){ //parametr o indexie 2 to ID jednostki
                chars[idIndex] = c;
                idIndex++;
            }

            if(c==32){ //odczytywanie nowego parametru przez wykrycie spacji
                currParameter++;
                if(idIndex > 0){
                    readID = atoi(chars);
                    if (readID == id) {
                        fclose(statusFile);
                        RemoveLineFromFile(statusFileName, lineNo);
                        return;//Wstrzymaj odczyt po znalezieniu pasujacego ID;
                    }
                    idIndex=0;
                }
            }
        }
        if (c=='\n'){
            lineNo++;
            hasStartedReading = 1;
            currParameter = 0;
        }
    }

    fclose(statusFile);
}

void GetUnitsFromStatus(Unit **unitList,unsigned int *amount, char owner){
    /*
    Dobranie parametru owner decyduje ktore jednostki zostana odczytane z pliku status.txt
    'P' = Gracz
    'E' = Wrog
    'A' = Wszyscy
    */
    *amount = 0;
    FILE* statusFile = fopen(statusFileName,"r");
    char c;
    uint8_t hasStartedReading = 0; //flaga informujaca o momencie w ktorym plik konczy czytac linie z goldem
    uint8_t currParameter = 0; //ktory parametr jest odczytywany wg statusu - np. parametr 0 to wlasciciel jednostki

    uint8_t chars[4] = {0,0,0,0}; // tablica znakow w przypadku odczytywania liczb, np 200 zlota to tablica '2','0','0';
    uint8_t isReadingChars = 0; //flaga informujaca o wczytywaniu znakow
    uint8_t readingIndex = 0; //ktory znak tablicy chars[] jest wczytywany

    Unit readUnit;
    while((c=getc(statusFile))!= EOF){
        if(c=='\n' & !hasStartedReading)
        {
           hasStartedReading = 1;
           continue;
        }

        if(!hasStartedReading) continue;



        switch(currParameter){
            case 0:
                if(c==32) break;
                readUnit.owner = c;
                if (c != owner & owner != 'A') hasStartedReading = 0;
                break;
            case 1:
                if(c==32) break;
                readUnit.unitType = c;
                break;
            case 2:
                isReadingChars = 1;
                if(c==32) readUnit.ID = atoi(chars);
                break;
            case 3:
                isReadingChars = 1;
                if(c==32) readUnit.x = atoi(chars);
                break;
            case 4:
                isReadingChars = 1;
                if(c==32) readUnit.y = atoi(chars);
                break;
            case 5:
                isReadingChars = 1;
                if(c==32) readUnit.endurance = atoi(chars);
                break;
            case 6:
                if(c=='\n') break; //w przypadku ostatniego parametru, \n, a nie spacja terminuje linie
                readUnit.producedUnit = c;
                break;
        }

        if(c==32) { //jesli wykryto spacje, czytaj nowy parametr
            if(isReadingChars == 1){
                isReadingChars = 0;
                readingIndex = 0;
                for(int i=0;i<4;i++) chars[i] = 0; //wyczyszczenie bufora
            }
            currParameter++;
            continue;
        }

        if(isReadingChars==1){
            chars[readingIndex] = c;
            readingIndex++;
        }

        if(hasStartedReading & (c == '\n')){ //Gdy skonczono czytac linie, dodaje sie jednostke
            currParameter = 0;

            *unitList = realloc(*unitList,sizeof(Unit)*(*amount+1));
            (*unitList)[*amount] = readUnit;
            (*amount)++;


        }
    }
    fclose(statusFile);
}

uint8_t GetUnitByIDFromStatus(uint16_t id,Unit *returnedUnit){
    Unit *unitList = malloc(sizeof(Unit));
    Unit newUnit;
    int amount;
    GetUnitsFromStatus(&unitList,&amount,'A');
    for (int i=0;i<amount;i++){
        if(unitList[i].ID == id){
            newUnit = unitList[i];
            *returnedUnit = newUnit;
            return 1; //Przebieglo pomyslnie
        }
    }
    free(unitList);
    return 0; //Nie znaleziono ID
}


uint8_t GetUnitByID(char owner,uint16_t id,Unit *returnedUnit){
    Player *ply;
    if(owner == 'P') ply = &player;
    if(owner == 'E') ply = &enemy;
    Unit newUnit;
    for (int i=0;i<(*ply).amountOfUnits;i++){
        if(((*ply).units)[i].ID == id){
            newUnit = ((*ply).units)[i];
            *returnedUnit = newUnit;
            return 1; //Przebieglo pomyslnie
        }
    }
    return 0; //Nie znaleziono ID
}


uint16_t GrabUniqueUnitID(){
    Unit* unitList = malloc(sizeof(Unit));
    int amount;
    GetUnitsFromStatus(&unitList,&amount,'A');
    uint16_t uniqueId = 0;
    for(int i=0;i<amount;i++){
        if (uniqueId < unitList[i].ID){
            uniqueId = unitList[i].ID;
        }
    }
    free(unitList);
    return uniqueId+1;
}

