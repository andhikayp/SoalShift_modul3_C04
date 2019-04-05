#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <termios.h>

#define ATT 20
#define standby 0
#define battle 1
#define shop 2

pthread_t t[5];
int *value;
char a = '\0';

int health=300;
int hunger=200;
int food_stock = 5;
int hyg=100;
int cd=20;
int eat=0;
int bath = 0;
int aksi = 0;
int turn = 0;
int stop = 0;
int opponent = 100;

key_t key = 1234;
int mem;
struct termios old, new;
/* Read 1 character - echo defines echo mode */
char getch() 
{
    char ch;
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    // new.c_lflag |= ECHO; /* set echo mode */
    new.c_lflag &= ~ECHO; /* set no echo mode */
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
    ch = getchar();
    tcsetattr(0, TCSANOW, &old);
    return ch;
}

void* input()
{
    while ((a = getch())) 
    {
        if(aksi == standby)
        {
            if(a=='1')
            {
                if(food_stock>0)
                {
                    eat = 1;
                    food_stock--;
                }
                else
                    printf("Tidak punya makanan");
            }
            else if(a=='2' && !cd)
                bath=1;
            else if(a=='3')
            {
                system("clear");
                turn=0;
                aksi=battle;
            }
            else if(a=='4')
            {
                system("clear");
                aksi=shop;
            }
            else if(a=='5')
                stop=1;
        }
        else if(aksi==battle)
        {
            if(a=='1')
            {
                if(turn==0&&opponent>=20)
                {
                	opponent=opponent-ATT;
                	sleep(1);
                	turn=1;
                }
                else 
                	printf("Your monster has already won\n");
            }
            else if(a=='2')
            {
                system("clear");
                turn=0;
                opponent=100;
                aksi=standby;
            }
        }
        else if(aksi==shop)
        {
            if(a=='1')
            {
                if(*value>0)
                {
                    food_stock++;
                    *value=*value-1;
                }
                else
                    printf("Stock is empty");
            }
            else if(a=='2')
            {
                system("clear");
                aksi=standby;
            }
        }
    }
}
void* interface()
{
    while(1)
    {
        if(aksi==standby)
        {
            printf("Standby Mode\nHealth : %d\nHunger : %d\nHygiene : %d\n", health, hunger, hyg);
            printf("Food left : %d\n",food_stock);
            if(cd>0)
            	printf("Bath will be ready in %ds\n",cd);
            else 
            	printf("Bath is ready\n");
            printf("Choices\n1. Eat\n2. Bath\n3. Battle\n4. Shop\n5. Exit\n");
            sleep(1);
            system("clear");
        }
        else if(aksi==battle)
        {
            printf("Battle Mode\nMonster's Health : %d\nOpponent's Health : %d\n", health, opponent);
            printf("Choices\n1. Attack\n2. Run\n");
            sleep(1);
            system("clear");
            if(turn==1 && opponent>0)
            {
                turn=0;
            	health=health-ATT;
            }
        }
        else if(aksi == shop)
        {
            printf("Shop Mode\nShop Food Stock : %d\nYour Food Stock : %d\n", *value, food_stock);
            printf("Choices\n1. Buy\n2. Back\n");
            sleep(1);
            system("clear");
        }
    }
}
void* hunger_status()
{
    int i=0;
    while(i<10)
    {
        if(aksi!=battle)
        {
            sleep(1);
            if(eat)
            {
                i=0;
                eat=0;
                hunger=hunger+15;
                continue;
            }
            i++;
            if(i==10)
            	hunger=hunger-5;
            i=i%10;
        }
    }
}
void* health_status()
{
    int i=0;
    while(i<10)
    {
        if(aksi!=battle)
        {
            sleep(1);
            i++;
            if(i==10) 
            	health=health+5;
            i%=10;
        }
    }
}
void* hygiene_status()
{
    int i=0;
    while(i<30)
    {
        if(aksi!=battle)
        {
            sleep(1);
            if(bath)
            {
                i=0;
                cd=20;
                bath=0;
                hyg=hyg+30;
                continue;
            }
            i++;
            if(cd>0)
            	cd--;
            if(i==30) 
            	hyg=hyg-10;
            i%=30;
        }
    }
}
int main(int argc, char const *argv[]) 
{
    mem = shmget(key,sizeof(int),IPC_CREAT|0666);
    value = shmat(mem,NULL,0);

    pthread_create(&t[0],NULL,input,NULL);
    pthread_create(&t[1],NULL,hunger_status,NULL);
    pthread_create(&t[2],NULL,health_status,NULL);
    pthread_create(&t[3],NULL,hygiene_status,NULL);
    pthread_create(&t[4],NULL,interface,NULL);

    while(hunger>0&&hyg>0&&health>0&&!stop);
    pthread_cancel(t[0]);
    tcsetattr(0, TCSANOW, &old);
    printf("game has ended\n");
    return 0;
}