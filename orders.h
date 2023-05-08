typedef struct OrderData{
    uint16_t id;
    char action;
    uint16_t x;
    uint16_t y;
    uint16_t attackId;
    char unitToBuild;
    char owner;
} OrderData;

//sprawdzanie czy w tablicy sa tylko numery. Return 1 -> sa tylko numery, Return 2-> sa inne znaki
uint8_t IsNumbersOnly(char chars[],int amount);

//Inicjalizacja rozkazow
void InitOrdersFile();

//odczytywanie rozkazow
uint8_t ReadOrders();

//przetwarzanie rozkazow
uint8_t ProcessOrder(OrderData orderData);
//ruszanie jednostek
uint8_t Move(OrderData orderData);
//Sprawdza czy podczas odczytywania wylacznie jednego znaku, np. Mozliwa akcja (Znaki M,A,B), wczytano niepoprawne znaki
//np M232. Wykrycie = strata tury
uint8_t CheckCorrectCharFormat(char chars[],int amount);
//kalkulacja dystansu na mapie
uint16_t GetDistance(int x1,int x2,int y1,int y2);

//dobywa ID dla tablicy ataku na podstawie znaku jednostki, np 'S'
uint8_t GetTableUnitIndex(char unit);

//Atakowanie innej jednostki
uint8_t Attack(OrderData orderData);

//Budowanie jednostki
uint8_t Build(OrderData orderData);
