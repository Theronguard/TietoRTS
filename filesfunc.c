#include <stdio.h>
#include <stdint.h>
char statusFileName[64];
char ordersFileName[64];
char mapFileName[64];

void RemoveLineFromFile(char fileName[], int lineNo){


    FILE* file = fopen(fileName,"r");
    FILE* tempFile = fopen("temp.txt","w");
    if(file == NULL || tempFile == NULL) {
        printf("BLAD ODCZYTU PLIKU - ZRESTARTUJ PROGRAM");
        return;
    }


    char c;
    int lineNoCounter = 0;
    while((c=getc(file))!= EOF){
        if(c=='\n') lineNoCounter++;
        if(lineNoCounter != lineNo){
            fprintf(tempFile,"%c",c);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(statusFileName);
    rename("temp.txt",statusFileName);

}



