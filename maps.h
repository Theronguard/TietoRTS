#include <stdint.h>
void LoadMap();
void DisplayMap();
char GetTile(uint16_t x, uint16_t y);
void GetBase(char owner,uint16_t *x, uint16_t *y);
void GetClosestMine(uint16_t unitx, uint16_t unity,uint16_t *retX, uint16_t *retY);
