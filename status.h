//Inicjalizacja pliku status.txt
void InitStatusFile();

/*funkcja do grabowania jednostek ze status.txt (dane podawane przez argumenty
Pamietaj o zwolnieniu zaalokowanej tablicy */
void GetUnitsFromStatus(Unit **unitList,unsigned int *amount, char owner);

//Pobiera nowe ID jednostki na podstawie statusu
uint16_t GrabUniqueUnitID();

//Dodaje nowe jednostki do pliku status.txt
void AddUnitToStatus(Player player);

//Usuwa jednostke z pliku status.txt po ID
void RemoveUnitFromStatus(uint16_t id);

/*Zwracanie jednostki po ID z pliku Status: Zwraca 0 gdy nie znajdzie, zwraca 1 gdy jednostka jest znaleziona.
Z racji na narzucone przez zadanie ograniczenie, ktore wymaga by przeciwnicy komunikowali sie plikiem status,
jest to funkcja dzieki ktorej mozna dostac informacje o wrogich jednostkach, nie uzywajac globalnych wartosci Player czy Enemy*/
uint8_t GetUnitByIDFromStatus(uint16_t id,Unit *returnedUnit);

/*Podobnie jak powyzej, lecz wylacznie do sprawdzania swoich informacji, lecz nie przeciwnika.
Zwracany jest unit z bardziej kompletnymi informacjami*/
uint8_t GetUnitByID(char owner,uint16_t id,Unit *returnedUnit);
