#include <stdio.h>
#include <stdlib.h>
#include "maps.h"
#include "entities.h"
#include "orders.h"
#include "rounds.h"
#include "filesfunc.h"

int ProcessStartCommand();

int main()
{
    int timeLimit = ProcessStartCommand();
    LoadMap();
    //DisplayMap();
    InitStatusFile();
    InitOrdersFile();

    StartGame(timeLimit);
    return 0;
}

int ProcessStartCommand(){
    printf("Wprowadz komende ./<Nazwa programu> <nazwa pliku mapy> <nazwa pliku status> <nazwa pliku rozkazow> <opcjonalnie: limit czasowy>");
    uint8_t isCommandCorrect = 0;
    while(isCommandCorrect == 0){
        putchar('\n');
        char inputCommands[512];
        for (int i=0;i<512;i++) inputCommands[i] = 0;
        fgets(inputCommands, 512, stdin);




        if(inputCommands[0] != '.' || inputCommands[1] != '/') continue; //Komenda musi zaczynac sie na ./
        uint16_t index = 2; //skip ./
        uint16_t programIndex = 0;

        char *programName = malloc(sizeof(char)); //wstepna alokacja pamieci dla tablicy znakow
        while(inputCommands[index] == ' ') index++; //pomijanie odstepow
        while(inputCommands[index] != ' '){
            if(inputCommands[index] == 0) break;
            programName[programIndex] = inputCommands[index]; //przypisywanie kolejnych znakow
            index++;
            programIndex++;
            programName = realloc(programName,sizeof(char)*(programIndex+1)); //realokacja pamieci
        }
        programName[programIndex] = 0; //nul terminacja stringa

        uint8_t mapIndex = 0;
        char *mapName = malloc(sizeof(char));
        while(inputCommands[index] == ' ') index++; //pomijanie odstepow
        while(inputCommands[index] != ' '){
            if(inputCommands[index] == 0) break;
            mapName[mapIndex] = inputCommands[index];
            index++;
            mapIndex++;
            mapName = realloc(mapName,sizeof(char)*(mapIndex+1));
        }
        mapName[mapIndex] = 0;

        //reszta kodu to powtorki z gory dla innych parametrow
        uint8_t statusIndex = 0;
        char *statusName = malloc(sizeof(char));
        while(inputCommands[index] == ' ') index++; //pomijanie odstepow
        while(inputCommands[index] != ' '){
            if(inputCommands[index] == 0) break;
            statusName[statusIndex] = inputCommands[index];
            index++;
            statusIndex++;
            statusName = realloc(statusName,sizeof(char)*(statusIndex+1));
        }
        statusName[statusIndex] = 0;

        uint8_t orderIndex = 0;
        char *orderName = malloc(sizeof(char));
        while(inputCommands[index] == ' ') index++; //pomijanie odstepow
        while(inputCommands[index] != ' '){
             if(inputCommands[index] == '\n') break;
             if(inputCommands[index] == 0) break;

            orderName[orderIndex] = inputCommands[index];
            index++;
            orderIndex++;
            orderName = realloc(orderName,sizeof(char)*(orderIndex+1));

        }
        orderName[orderIndex] = 0;

        uint8_t hasReadNum = 0; //flaga decydujaca o parametrze opcjonalnym
        uint8_t timeIndex = 0;
        char *timeLimit = malloc(sizeof(char));
        while(inputCommands[index] == ' ') index++; //pomijanie odstepow
        while(inputCommands[index] != ' '){
            if(inputCommands[index] == 0) break;
            timeLimit[timeIndex] = inputCommands[index];
            if((timeLimit[timeIndex]) >= 48 & (timeLimit[timeIndex] <= 57)){ //jesli odczytano numer
                hasReadNum = 1;
            }
            index++;
            timeIndex++;
            timeLimit = realloc(timeLimit,sizeof(char)*(timeIndex+1));

        }
        timeLimit[timeIndex] = 0;

        int timeLimitNum = -1;
        if (hasReadNum == 1) {
            timeLimitNum = atoi(timeLimit);
        }


        FILE* file = fopen(mapName,"r");
        if (file == NULL){
            printf("\n Plik %s nie istnieje!",mapName);
            continue;
        }
        else{
            if(mapIndex > 63) continue;
            for(int i=0;i<=mapIndex;i++){  //podanie danych do globala
                mapFileName[i] = mapName[i];
            }
            isCommandCorrect = 1;
        }

        if(statusIndex > 63) isCommandCorrect = 0;
        for(int i=0;i<=statusIndex;i++){
            statusFileName[i] = statusName[i];
        }

        if(orderIndex > 63) isCommandCorrect = 0;
        for(int i=0;i<=orderIndex;i++){
            ordersFileName[i] = orderName[i];
        }

        free(programName);
        free(statusName);
        free(timeLimit);
        free(mapName);
        free(orderName);


        if(isCommandCorrect == 1){
            return timeLimitNum;
        }
    }

}
