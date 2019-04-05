#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
 
pthread_t tid1, tid2;
int WakeUp_Status = 0;
int Spirit_Status = 100;
int flag=0, i=0, j=0, flag_agmal=0, flag_iraj=0;

void* Agmal(void *arg)
{
    	while(1){
        	if(flag_agmal==1){
            		sleep(10);
			          flag_agmal=0;
        	}
    	}
}

void* Iraj(void *arg)
{
	    while(1){
        	if(flag_iraj==1){
           		  sleep(10);
            		flag_iraj=0;
        	}
    	}
}
 
int main(void)
{
    	char query[50];
    	char blank;

   	  pthread_create(&(tid1), NULL, Agmal, NULL);
   	  pthread_create(&(tid2), NULL, Iraj, NULL);
	
	    printf("Agmal WakeUp_Status = %d\n",WakeUp_Status);
      printf("Iraj Spirit_Status = %d\n",Spirit_Status);
	    printf("\n");

    	while(1){
        	if(flag==1){
          	  	break;
       	 	}
        	scanf("%[^\n]s",query);
        	scanf("%c",&blank);

        	if(strcmp(query,"Agmal Ayo Bangun") == 0){
            		if(flag_agmal==0){
                		WakeUp_Status += 15;
                		i++;
            		}else{
                		printf("Agmal Ayo Bangun disabled 10 s \n");
            		}
        	}
        	else if(strcmp(query,"Iraj Ayo Tidur") == 0){
            		if(flag_iraj==0){
                		Spirit_Status -= 20;
                		j++;
            		}else{
                	printf("Fitur Iraj Ayo Tidur disabled 10 s \n");
            		}
        	}	
		      else if(strcmp(query,"All Status") == 0){
			          printf("Agmal WakeUp_Status = %d\n",WakeUp_Status);
        		    printf("Iraj Spirit_Status = %d\n",Spirit_Status);
			          //printf("i=%d j=%d \n", i, j);
		      }
        	if(j==3){
            		flag_agmal=1;
			          j=0;
        	}
        	if(i==3){
            		flag_iraj=1;
			          i=0;
        	}
        	if(WakeUp_Status >= 100){
            		printf("Agmal Terbangun,mereka bangun pagi dan berolahraga\n");
			          flag = 1;
        	}
        	if(Spirit_Status <= 0){
           		  printf("Iraj ikut tidur, dan bangun kesiangan bersama Agmal\n");
			          flag = 1;
        	}
		      //printf("Agmal WakeUp_Status = %d\n",WakeUp_Status);
        	//printf("Iraj Spirit_Status = %d\n",Spirit_Status);
		      //printf("i=%d j=%d \n", i, j);
		      printf("\n");
    	}
    
    	return 0;
}
