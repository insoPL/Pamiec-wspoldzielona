#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<signal.h>
#include<unistd.h>
#include<time.h>

#include "biblioteka.h"


#define plikWyjscia "wyjscie.txt"
#define plikWejscia "wejscie.txt"
#define plikWejscia2 "wejscie2.txt"//ustal 2 plik wejscia w razie wielokrotnego zapisu/odczytu

#define kasujDanePoOdczytaniu 1
#define sposobDrukowania 1//0 - drukuj bufor, 1 - drukuj dane
//warto zmienic czas lagu przy zmienianianiu rodzaju druku

#define szansaNaLagOdczytu 100// Szansa wyrazona w procentach
#define szansaNaLagZapisu 100//  Szansa wyrazona w procentach
#define czasLaguOdczytu 2
#define czasLaguZapisu 1

char c;

pid_t pid=-1;
int trybW=0;

void porzadki();

int main(int argc,char** argv){
atexit(porzadki);
signal(SIGINT,exit);

stworzZbiorSemaforow();
stworzPamiecWspoldzielona();
odwzoruj();

if(argc>1){
	if(argv[1][0]=='r'){
		pid=0;
		trybW=1;
	}
	else if(argv[1][0]=='w'){
		pid=1;
		trybW=1;
	}
	else{
		perror("bledny argument programu\n");
		exit(0);
	}
}
else{//jesli bezargumentowe wywolanie programu to:
inicjujPamiec();
inicjujSemafory();
pid=fork();
}

if(pid<0)perror("blad tworzenia procesu");

if(pid==0){///////////////////////////////////////////////////////////////////////////
//ODCZYT Z BUFORA

int czas=time(NULL);
srand(czas);

FILE *plik;
if(trybW==0){plik = fopen(plikWyjscia,"w");
if(plik==NULL){
        perror("Nie udalo sie otworzyc pliku");
        return 1;
}}

while(1){
    if(rand()%100<szansaNaLagOdczytu)usleep(1000000*czasLaguOdczytu);

/////////////////////Strefa Krytyczna
    obniz(3);//obniz semafor ochrony przed jednoczesnym odczytem
    obniz(1);//obniz semafor ilosci danych w buforze

    c=odczytaj(kasujDanePoOdczytaniu);

#if sposobDrukowania==0
    drukujBufory();
#endif

    podnies(3);//podnies semafor ochrony przed jednoczesnym odczytem
    podnies(0);//podnies semafor ilosci miejsca
////////////////////////////////////koniec strefy krytycznej

#if sposobDrukowania==1
    putc(c,stdout);fflush(stdout);//proste drukowanie odczytanych liter
#endif

 if(trybW==0) putc(c,plik);//zapis do pliku
}

    fclose(plik);
    porzadki();
}


if(pid>0){//////////////////////////////////////////////////////////////////////////////
//ZAPIS DO BUFORA

#if sposobDrukowania==0
if(trybW==0)zerujBufor();
#endif

int czas=time(NULL);
srand(czas);

FILE *plik;
if(trybW==0)plik = fopen(plikWejscia,"r");
else plik = fopen(plikWejscia2,"r");
if(plik==NULL){
	perror("Nie udalo sie otworzyc pliku");
	return 1;
}

while(1){
    if(rand()%100<szansaNaLagZapisu)usleep(1000000*czasLaguZapisu);

    c=getc(plik);

/////////////////////////////////////Strefa Krytyczna
    obniz(2);//obniz semafor ochrony przed jednoczesynm zapisem
    obniz(0);//Obniz samafor ilosci miejsca

    if(feof(plik)){//jesli koniec pliku
	podnies(1);
	podnies(2);
	break;
    }

	zapisz(c);//zapisz c do bufora

#if sposobDrukowania==0
    drukujBufory();
#endif

    podnies(2);//podnies semafor ochrony przed jednoczesym zapisem
    podnies(1);//podnies semafor ilosci danych w buforze
//////////////////////////////////////koniec strefy krytycznej
}

    fclose(plik);
}
    return 0;
}


void porzadki(){
if(trybW==1){//TRYB JEDNOSTRONNEGO ODCZYTU
odnowSemaforyWielow();//upewnij sie ze samafory ochrony przed podwojynm zapisem sa w gorze
exit(0);
}
else if(pid==0){//ODCZYT Z BUFORA
	usunPamiec();
	usunSemafory();
}
else if(pid>0){}//ZAPIS DO BUFORA
else {//BLAD TWORZENIEA PROCESOW
	usunPamiec();
	usunSemafory();
}

printf("KONIEC PROCESU\n");
}
