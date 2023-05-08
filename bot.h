#include <stdint.h>

//petla glowna przetwarzania bota

void GetEnemyBaseData(uint16_t *x, uint16_t *y, uint16_t *id);
void MoveAlgorithm(uint16_t id,uint16_t targetX,uint16_t targetY);
uint16_t CheckForWorker();
uint8_t IsCurrentlyProducing();
uint8_t ProcessBot();

