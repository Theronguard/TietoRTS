#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filesfunc.h"


//dane wczytanej mapy
char ** loadedMap;
int mapXSize, mapYSize;

//Wyswietlanie mapy w celach sprawdzenia czy zostala poprawnie wczytana
void DisplayMap(){
    if(!loadedMap) return;
    for(int y=0;y<mapYSize;y++){
       for(int x=0;x<mapXSize;x++){
          printf("%c",loadedMap[x][y]);
        }
        putchar('\n');
    }
}

//Wyswietla mape w konsoli
void LoadMap(){
    FILE* mapFile = fopen(mapFileName, "r");
    if(!mapFile) return;


    int xSize=0,ySize=0;
    int stopCountingX = 0;
    char c = 0;

    //Zliczanie rozmiaru mapy
    while((c=getc(mapFile))!= EOF){

        //zliczanie X po pierwszym wierszu
        if(c != '\n' && stopCountingX == 0) xSize++;
        else stopCountingX = 1;

        //zliczanie Y po newline'ach
        if(c=='\n') ySize++;
    }

    //Na koncu pliku nie ma newline'a, tylko end of file
    if(c==EOF) ySize++;

    //Deklaracja mapy 2D
    char ** map = (char**)calloc(xSize,sizeof(char *));
    for(int i=0;i<xSize;i++){
        map[i] = (char*)calloc(ySize,sizeof(char));
    }

    //ponowne odczytanie pliku i zapisanie go do tablicy 2D
    int X=0,Y=0;
    rewind(mapFile);
    while((c=getc(mapFile))!= EOF){
         if(c=='\n'){
            X=0;
            Y++;
        }
        map[X][Y] = c;
       if(c!='\n') X++;
    }

    loadedMap = map;
    mapXSize = xSize;
    mapYSize = ySize;

    fclose(mapFile);

    printf("Wczytano mape!\n");
    //zwalnianie tablicy 2d
    /*
    for(int i=0;i<xSize;i++)
        free(map[i]);
    free(map);
    */
}

char GetTile(uint16_t x, uint16_t y){
    if(x>(mapXSize-1) || y>(mapYSize-1)) return 0; //0 oznacza niepoprawne pole poza mapa
    return loadedMap[x][y];
}


void GetBase(char owner,uint16_t *x, uint16_t *y){
    for((*y)=0;(*y)<mapYSize;(*y)++){
       for((*x)=0;(*x)<mapXSize;(*x)++){
            if(owner == 'P')
                if (loadedMap[*x][*y] == '1') return;

            if(owner == 'E')
                if (loadedMap[*x][*y] == '2') return;
        }
    }
}


void GetClosestMine(uint16_t unitx, uint16_t unity,uint16_t *retX, uint16_t *retY){
    uint16_t dist = mapXSize+mapYSize;

    for(int y=0;y<mapYSize;y++){
       for(int x=0;x<mapXSize;x++){
          if(GetTile(x,y)=='6'){
            if(GetDistance(unitx,x,unity,y) < dist){
                dist = GetDistance(unitx,x,unity,y);
                *retX = x;
                *retY = y;
            }
          }
        }
    }
}
