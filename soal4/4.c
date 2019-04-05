//template from https://github.com/desyrahmi/sisop-modul-3

#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdlib.h>
/*******************************************************
*compile dengan cara gcc -pthread -o [output] input.c *
*******************************************************/

pthread_t tid[5];//inisialisasi array untuk menampung thread dalam kasusu ini ada 2 thread
int iter;
void* membuat(void *arg)
{
    pthread_t id=pthread_self();
    iter=0;
    if(pthread_equal(id,tid[0]))//thread untuk menjalankan counter
    {
        system("ps aux --no-heading | head -10 > ~/Documents/FolderProses1/SimpanProses1.txt");   
    }
    else if(pthread_equal(id,tid[1]))
    {
        system("ps aux --no-heading | head -10 > ~/Documents/FolderProses2/SimpanProses2.txt");
    }
    iter=1;
    return NULL;
}
void* mengkompres(void *arg)
{
    while(iter!=1)
    {

    }
    pthread_t id=pthread_self();
    if(pthread_equal(id,tid[2]))//thread untuk menjalankan counter
    {
        system("zip -qmj ~/Documents/FolderProses1/KompresProses1 ~/Documents/FolderProses1/SimpanProses1.txt");
    }
    else if(pthread_equal(id,tid[3]))
    {
        system("zip -qmj ~/Documents/FolderProses2/KompresProses2 ~/Documents/FolderProses2/SimpanProses2.txt");
    }
    iter=2;
    return NULL;
}
void* mengekstrak(void *arg)
{
    while(iter!=2)
    {

    }
    sleep(15);
    pthread_t id=pthread_self();
    if(pthread_equal(id,tid[4]))//thread untuk menjalankan counter
    {
        system("unzip -qd ~/Documents/FolderProses1 ~/Documents/FolderProses1/KompresProses1.zip");
    }
    else if(pthread_equal(id,tid[5]))
    {
        system("unzip -qd ~/Documents/FolderProses2 ~/Documents/FolderProses2/KompresProses2.zip");  
    }
    return NULL;
}
int main(void)
{
    int i=0;
    int err;
    while(i<2)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&membuat,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else 
        {
            printf("\n create thread success");
        }
        i++;
    }
    while(i<4)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&mengkompres,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            printf("\n create thread success");
        }
        i++;
    }
    printf("\n Menunggu 15 detik untuk mengekstrak kembali");
    while(i<6)//looping membuat thread 2x
    {
        err=pthread_create(&(tid[i]),NULL,&mengekstrak,NULL);//membuat thread
        if(err!=0)//cek error
        {
            printf("\n can't create thread : [%s]",strerror(err));
        }
        else
        {
            // printf("\n create thread success");
        }
        i++;
    }
    printf("\n DONE!");
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);
    pthread_join(tid[3],NULL);
    pthread_join(tid[4],NULL);
    pthread_join(tid[5],NULL);
    return 0;
}