#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
int average,max,min;
int count;

void *average_thread(void *param);
void *max_thread(void *param);
void *min_thread(void *param);

int main(int argc,char **argv){
	int i;
	count=argc-1;
	pthread_t Thread1,Thread2,Thread3;//线程标识符 
	pthread_attr_t attr1,attr2,attr3;//创建线程的属性
	
	if(count<=0){
		printf("error!no input");
		return -1;
	} 
	
	int *numbers=(int *)malloc(sizeof(int)*count);
	if(numbers==NULL){
		printf("error!malloc error");
		return -1;
	}
	
	for(i=0;i<count;i++){
		numbers[i]=atoi(argv[i+1]);
	}
	
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_init(&attr3);
	
	pthread_create(&Thread1,&attr1,average_thread,numbers);
	pthread_join(Thread1,NULL);
	
	pthread_create(&Thread2,&attr2,max_thread,numbers);
	pthread_join(Thread2,NULL);
	
	pthread_create(&Thread3,&attr3,min_thread,numbers);
	pthread_join(Thread3,NULL);
	
	printf("The average value is %d\n",average);
	printf("The minimun value is %d\n",min);
	printf("The maximun value is %d\n",max);
	
	free(numbers);
	return 0;
}

void *average_thread(void *param){
	printf("average_thread...running\n");
	int *num=(int *)param;
	int sum=0;
	int i;
	for(i=0;i<count;i++){
		sum+=num[i];
	}
	average=sum/count;
	pthread_exit(0);
} 

void *max_thread(void *param){
	printf("max_thread...running\n");
	int *num=(int *)param;
	max=num[0];
	int i;
	for(i=1;i<count;i++){
		if(num[i]>max){
			max=num[i];
		}
	}
	pthread_exit(0);
}

void *min_thread(void *param){
	printf("min_thread...running\n");
	int *num=(int *)param;
	min=num[0];
	int i;
	for(i=1;i<count;i++){
		if(num[i]<min){
			min=num[i];
		}
	}
	pthread_exit(0);
}
