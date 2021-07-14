#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#define total_ins 100
#define total_vp 10
#define clar_period 50
#define INVALID -1

/*页面结构*/
struct pl_type{
	int pn,pfn,counter,time;
};

pl_type pl[total_vp];//页面结构数组 

/*页面控制数组*/
struct pfc_type{
	int pn,pfn;
	pfc_type *next;
};


pfc_type pfc[total_vp],*freepf_head,*busypf_head,*busypf_tail;

int diseffect,  a[total_ins+1];
int page[total_ins],  offset[total_ins];

int  initialize(int);
int  FIFO(int);
int  LRU(int);
int  OPT(int);

bool isExist(int a,int rd[],int length){
	int i=1;
	for(i=1;i<length;i++){
		if(a==rd[i])
			return true;
	}
	return false;
}

int initialize(int total_pf){
	diseffect = 0;
	int i;
	for(i=0;i<total_vp;i++){
		pl[i].pn = i;
		pl[i].pfn = INVALID;
		pl[i].counter = 0;
		pl[i].time = -1;
	}
	for (int i = 0; i < total_pf - 1; i++){
		pfc[i].next = &pfc[i + 1];
		pfc[i].pfn = i;
	}
	pfc[total_pf - 1].next = NULL;
	pfc[total_pf - 1].pfn = total_pf - 1;
	freepf_head = &pfc[0];  //内存空页面队列的头指针为pfc[0] 
	return 0;
}

int FIFO(int total_pf) {             /*先进先出算法*/
	pfc_type *p;
	initialize(total_pf);  //初始化相关页面控制用数据结构
	busypf_head = busypf_tail = NULL;  //内存页的队列头，队列尾指针接
	for (int i = 0; i<total_ins; i++){
		if (pl[page[i]].pfn == INVALID) {  //页表项不在内存中
			diseffect += 1;                  //失效次数
			if (freepf_head == NULL) {        //内存无空闲页面
				p = busypf_head->next;
				pl[busypf_head->pn].pfn = INVALID;
				freepf_head = busypf_head;  //释放忙页面队列的第一个页面
				freepf_head->next = NULL;
				busypf_head = p;
			}
			// 按FIFO方式调新页面入内存页面
			p = freepf_head->next;  // 先保存内存表中当前位置的下一位置
			freepf_head->next = NULL;
			freepf_head->pn = page[i];  // 页表号
			pl[page[i]].pfn = freepf_head->pfn;  // 内存块号

			if (busypf_tail == NULL) {
				// busypf_head指向最老的，busypf_tail指向最新的
				busypf_head = busypf_tail = freepf_head;
			}
			else{
				busypf_tail->next = freepf_head;  //free页面减少一个
				busypf_tail = freepf_head;
			}
			freepf_head = p;
		}
	}
	printf("FIFO:%.4f\n", diseffect / (total_ins*1.0));

	return 0;
}

int LRU(int total_pf) {      /*最近最久未使用算法（使用时钟计数器）*/
	int min, minj, present_time;
	initialize(total_pf);
	present_time = 0;
	for (int i = 0; i<total_ins; i++){
		if (pl[page[i]].pfn == INVALID) {            //页面失效，不在内存中
			diseffect++;
			if (freepf_head == NULL) {             //内存无空闲页面
				min = 32767;
				for (int j = 0; j < total_vp; j++) {  //找出内存块中time的最小值
					if (min > pl[j].time && pl[j].pfn != INVALID)  // 查询页表
					{
						min = pl[j].time;
						minj = j;  // 记下内存块号
					}
				}
				freepf_head = &pfc[pl[minj].pfn];   //腾出一个单元（对应的内存块）
				pl[minj].pfn = INVALID;
				pl[minj].time = -1;
				freepf_head->next = NULL;
			}
			pl[page[i]].pfn = freepf_head->pfn;   //有空闲页面,改为有效（内存块号）
			pl[page[i]].time = present_time;
			freepf_head = freepf_head->next;      //减少一个free 页面
		}
		else {
			pl[page[i]].time = present_time;        //命中则设置时间
		}
		present_time++;
	}
	printf("LRU:%.4f\n", diseffect / (total_ins*1.0));
	return 0;
}


int OPT(int total_pf) {      /*最佳置换算法*/
	int max, maxpage, d, dist[total_vp];
	initialize(total_pf);
	for (int i = 0; i < total_ins; i++) {
		if (pl[page[i]].pfn == INVALID) {      //页面失效，不在内存中
			diseffect++;
			if (freepf_head == NULL) {        //无空闲页面
				for (int j = 0; j < total_vp; j++) {
					if (pl[j].pfn != INVALID) {
						dist[j] = 32767;  /* 最大"距离" */
					}
					else {
						dist[j] = 0;
					}
				}
				d = 1;
				for (int j = i + 1; j < total_ins; j++) {
					if (pl[page[j]].pfn != INVALID && dist[page[j]] == 32767) {
						dist[page[j]] = d;
					}
					d++;
				}
				max = -1;
				for (int j = 0; j < total_vp; j++) {
					if (max < dist[j]) {
						max = dist[j];
						maxpage = j;
					}
				}
				freepf_head = &pfc[pl[maxpage].pfn];
				freepf_head->next = NULL;
				pl[maxpage].pfn = INVALID;
			}
			pl[page[i]].pfn = freepf_head->pfn;
			freepf_head = freepf_head->next;
		}
	}
	printf("OPT:%.4f\n", diseffect / (total_ins*1.0));

	return 0;
}


int main(){
	int s,i,j;
	srand(time(NULL));//使用time初始化随机数种子  
	a[0]=0;
	int rd[total_ins+1];
	rd[1]=(float)total_ins*rand()/32767/32767/2+1;
	for(i=2;i<=total_ins;i++){
		s=(float)total_ins*rand()/32767/32767/2+1;
		while(isExist(s,rd,i)==true){
			s=(float)total_ins*rand()/32767/32767/2+1;
		}
		rd[i]=s;	
	}//1到100的数组
	
	printf("生成的地址序列为:\n%d ",a[0]);
	for(i=1;i<total_ins;i++){
		s=rd[i]; 
		if(s<=0.7*total_ins-1){ //a[0]是属于70%的指令 
			a[i]=a[i-1]+1;
			if(a[i]>total_ins)
				a[i]=0;
		} 
		else if(s>0.7*total_ins&&s<=0.8*total_ins){
			a[i]=(float)a[i-1]*rand()/32767/32767/2;//[0,m-1]的随机地址 
		}
		else if(s>0.8*total_ins&&s<=total_ins){
			a[i]=(float)rand()/32767/32767/2*(total_ins-1-a[i-1])+a[i-1]+1; //[m+1,99]的随机地址 
		}
		printf("%d ",a[i]);
		if(i%9==0)
			printf("\n");	
	}//生成100个指令地址 
	

	printf("生成的内存访问串为:");
	for(i=0;i<total_ins;i++) /*将指令序列变换为页地址流*/{
		if(i%10==0)
			printf("\n");
 		page[i]=a[i]/10;
 		offset[i]=a[i]%10;
		printf("%d ",page[i]);
	}
	for(i=2;i<=total_ins/10;i++){
		printf("\nblock==%d:\n",i);
		FIFO(i);
		LRU(i);
		OPT(i);
	
	}

}

 
