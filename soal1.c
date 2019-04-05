#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

int array[100];
pthread_t tid[100];

struct Factorial{
	int value;
};

void* tulis(void* args){
	unsigned long long fctr=1;
    	struct Factorial*extract =(struct Factorial*)args;
 	for(int i=1; i<=extract->value; ++i)
        {
         	fctr = i*fctr;          
        }
   	printf("\n");
    	printf("%d! = %llu\n", extract->value, fctr);
}

int main(){
  	int n=0, i=0, j=0, order, tmp;
  	char blank;
  	do {
      		scanf("%d%c", &array[i], &blank);
      		i++;
		n++;
  	} while(blank != '\n');
 

 	for (i = 0; i<n-1; ++i) 
        {
        	for (j = i + 1; j<n; ++j)
            	{
                	if (array[i] > array[j]) 
                	{
                    		tmp =  array[i];
                    		array[i] = array[j];
                    		array[j] = tmp;
                	}
            	}     
        }

   	i=0;
	
  	struct Factorial factorial;
  	while(i<n) {
 		factorial.value = array[i];
 		order=i;
 		pthread_create(&tid[order], NULL, &tulis, (void *)&factorial);
        	pthread_join(tid[order], NULL);
     		i++;
  	}
    
    	return 0;
}
