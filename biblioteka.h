#ifndef FUNKCJA_H   /* zapewnia najwyzej jednokrotne wlaczenie tego pliku */
#define FUNKCJA_H

void szybkiDruk();

void stworzZbiorSemaforow();

void inicjujSemafory();
//0-semafor ilosci miejsca (poczatkowa wartosc - rozmiarBufora)
//1-semafor ilosc danych w buforze (poczatkowa wartosc - 0)
//2-semafor ochrony przed jednoczesnym zapisem (pocz wartosc - 1)
//3-semafor ochrony przed jednoczesnym odczytem (pocz wartosc -1)

int stanSemafora(int num);
//wypisuje komunikat o stanie 2 pierwszych semaforow dla num==-1
//dla argumentu 0 i 1 zwroci odpowiednio wartosc semafora 0 albo 1


void podnies(int x);//podnies semafor x o 1

void obniz(int x);//oposc semafor x o 1

void stworzPamiecWspoldzielona();//tworzy pamiec wspoldzielona

void odwzoruj();//odwzorowuje pamiec wspoledzielona
//pamiec dzieli sie na 3 czesci
//pierwsza czesc to psedo wskaznik zapisu
//druga to pseudo-wskaznik odczytu
//trzecia to bufor cykliczny

void inicjujPamiec();//inicjuje pamiec poczatkowymi wartosciami
//w pierwszej i drugiej czesci wstawia 0

void zapisz(char c);//zapisz znak c w buforze

char odczytaj(int kasujDane);//odczytaj znak z bufora i opcjonalnie skasuj
//dane po odczytaniu

int dodaj(int x);//zwieksz pseudo-wskaznik x tak zeby wskazywal na nastepny
//element bufora cyklicznego. Uzywane tylko w ramach biblioteki

void drukujBufory();//drukuj graficzna reprezentacje bufora cyklicznego

void zerujBufor();//zeruj bufor

void usunSemafory();//usun semafory

void usunPamiec();//usun pamiec wspoldzielona

void odnowSemaforyWielow();//inicjalizuj semafory ochrony wielowatkowej

#endif
