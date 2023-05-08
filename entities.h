#include <stdint.h>


//Definicja jednostki jako struct
typedef struct Unit
{
    char owner; //wlasciciel
    char unitType; //typ jednostki 'K','S', etc.
    uint16_t ID; //id jednostki
    uint16_t x; //pozycja x na mapie
    uint16_t y; //pozycja y na mapie
    uint16_t endurance; //wytrzymalosc / zdrowie
    uint16_t speed; //predkosc ruchu
    uint16_t cost; //koszt produkcji jednostki
    uint8_t attackRange; //zasieg ataku
    uint8_t buildTime; //czas produkcji jednostki - dla bazy, zmienna ta okresla za ile zbuduje sie dana jednostka
    char producedUnit; //Dziala tylko dla baz
    char order; //aktualny rozkaz
    uint8_t movePoints; //dostepna ilosc punktow ruchu
} Unit;



//Definicja gracza, z dynamiczna lista jednostek Units
typedef struct Player
{
    char playerName[32]; //nazwa gracza
    char owner; //wlasciciel P - gracz (czlowiek) E - wrog (bot)
    long gold; // zloto
    unsigned int amountOfUnits; //ilosc posiadanych jednostek
    uint8_t canAttack; //zmienna zapewniajaca jeden atak na ture
    Unit* units; //lista jednostek
} Player;


//Globalne zmienne dla gracza i wroga
extern Player player;
extern Player enemy;

//inicjalizacja gracza i wroga
void InitPlayer(char owner);

//Zwraca jednostke na podstawie jej znaku np. Knight po znaku 'K'
Unit GetBaseUnit(char c);

void DisplayUnits(Unit *unitList, unsigned int amount);


//Dodawanie jednostek do listy Units poszczegolnych graczy
void AddUnit(Player *player, Unit unit, int forcedId);
//Usuwa jednostke danego gracza poprzez ID i reference do gracza
void RemoveUnit(Player *player, uint16_t id);
//Usuwa jednostke na podstawie samego id
uint8_t RemoveUnitByID(uint16_t id);


//Aktualizuje jednostke w status.txt oraz pamieci gracza
void UpdateUnit(Unit unit);


