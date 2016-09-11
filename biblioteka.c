#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<errno.h>
#include<unistd.h>
#include "biblioteka.h"

#define rozmiarBufora 25

int semid,memid;
char* poczatek;//bufora
int* wskOdczytu;
int* wskZapisu;


void stworzZbiorSemaforow(){
key_t klucz = ftok("a",'a');

if(klucz == (key_t) -1 ||(semid=semget(klucz,4,IPC_CREAT|0666))==-1){
          printf("klucz: %i semid: %i\n",klucz,semid);fflush(stdout);
          perror("blad otwerania zbioru semaforow\n");
          exit(-1);}

}


union semun {
int val;
struct semid_ds *buf;
ushort *array;
};


void inicjujSemafory(){

union semun sem;
sem.val=rozmiarBufora;

if(semctl(semid,0,SETVAL,sem) == -1){
          perror("blad inicjalizowania zbioru semaforow\n");
          exit(-1);}

sem.val=0;
if(semctl(semid,1,SETVAL,sem) == -1){
          perror("blad inicjalizowania zbioru semaforow\n");
          exit(-1);}

odnowSemaforyWielow();
}

void odnowSemaforyWielow(){
union semun sem;
sem.val=1;
if(semctl(semid,2,SETVAL,sem) == -1){
          perror("blad inicjalizowania zbioru semaforow\n");
          exit(-1);}

sem.val=1;
if(semctl(semid,3,SETVAL,sem) == -1){
          perror("blad inicjalizowania zbioru semaforow\n");
          exit(-1);}


}

int stanSemafora(int num){
int x,y;
if((x=semctl(semid,0,GETVAL,NULL))==-1){
          printf("blad stan semaforow %i\n",getpid());
          exit(-1);}

if((y=semctl(semid,1,GETVAL,NULL))==-1){
          printf("blad stan semaforow %i\n",getpid());
          exit(-1);}

if(num==-1){
printf("\nstan semafora 0:%i\n",x);
printf("stan semafora 1:%i\n",y);
}

if(num==0)return x;
else if(num==1)return y;
else return x;

}


void podnies(int x){

struct sembuf buff;

buff.sem_num=x;
buff.sem_op=1;
buff.sem_flg=0;

if(semop(semid,&buff,1)==-1){
         perror("blad w kolejce przed semaforem\n");
          exit(-1);}
}


void obniz(int x){

struct sembuf buff;

buff.sem_num=x;
buff.sem_op=-1;
buff.sem_flg=0;

if(semop(semid,&buff,1)==-1){
         perror("blad w kolejce przed semaforem");
          exit(-1);}
}


void stworzPamiecWspoldzielona(){
key_t klucz = ftok("a",'a');
memid=shmget(klucz,rozmiarBufora+(2*sizeof(int)),IPC_CREAT|0666);

if(klucz == (key_t) -1 ||memid==-1){
          printf("klucz: %i semid: %i\n",klucz,semid);fflush(stdout);
          perror("blad tworzenia pamieci wspoldzielonej\n");
          exit(-1);}
}


void odwzoruj(){
char *wsk;
wsk=shmat(memid,0,0);

poczatek=wsk+sizeof(int)*2;
wskOdczytu=wsk+sizeof(int);
wskZapisu=wsk;


if(wsk==(char*)-1){
          perror("blad odwzorowanie pamieci wspol\n");
          exit(-1);}
}


void inicjujPamiec(){
*wskOdczytu=0;
*wskZapisu=0;
}


char odczytaj(int kasujDane){
char c;

c=(*(poczatek+(*wskOdczytu)*sizeof(char)));

if(kasujDane){
(*(poczatek+sizeof(char)*(*wskOdczytu)))='0';
}

*wskOdczytu=dodaj(*wskOdczytu);

return c;
}


void zapisz(char c){
(*(poczatek+(*wskZapisu)*sizeof(char)))=c;
*wskZapisu=dodaj(*wskZapisu);
}


int dodaj(int x){
if(x==rozmiarBufora-1){
x=0;
}
else x++;

return x;
}


void drukujBufory(){

char *asd=poczatek;

int x;
for(x=0;x<rozmiarBufora;x++){
  char abc = ' ';
	if(*wskZapisu==x)abc='w';
  if(*wskOdczytu==x)abc='r';
  if((*wskZapisu==x) && (*wskOdczytu==x))abc='a';
	printf("|%c|",abc);
	abc++;
}
printf("\n");
for(x=0;x<rozmiarBufora;x++){printf("|%c|",*asd);asd++;}
printf("\n");
printf("\n");
fflush(stdout);
}


void zerujBufor(){
int x;
char *asd=poczatek;
for(x=0;x<rozmiarBufora;x++){*asd='0';asd++;}
}


void usunSemafory(){
semctl(semid,0,IPC_RMID);
semctl(semid,1,IPC_RMID);
semctl(semid,3,IPC_RMID);
semctl(semid,4,IPC_RMID);
}


void usunPamiec(){
if(shmdt((void*)wskZapisu)==-1)printf("blad kasowania pamieci");
shmctl(memid,IPC_RMID,NULL);
}


void szybkiDruk(){
char *asd=poczatek;
int x;
for(x=0;x<rozmiarBufora;x++){printf("|%c|",*asd);asd++;}
}
