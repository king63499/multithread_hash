#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#define SIZE 20
#define TSIZE 2
#define M 500

typedef struct nd{
	int key;
	int value;
	int isFull;
	struct nd * n;
}Node;

typedef struct pr{
	int id;
	Node * table;
}Param;

//int datak[M] = {32,12,67,48,99,63,31,56,78,86,12,60,89,85,23,36,34,61,12,90};
//int datav[M] = {12,63,89,85,23,53,32,63,12,90,67,86,99,34,31,56,78,83,12,23};
int datak[M];
int datav[M];
pthread_mutex_t mt[SIZE];

void initTable(Node * table){
	//Node * table = (Node *)malloc(SIZE*sizeof(Node));
	int i;
	for(i = 0;i < SIZE;i++){
		(*(table+i)).n = NULL;
		(*(table+i)).isFull = 0;
	}
}

Node * initNode(){
	Node * a = (Node *)malloc(sizeof(Node));
	a->isFull = 0;
	a->n = NULL;
	return (a);
}

int fct(int key){
	return ((int)(key%SIZE));
}
void set(Node * table,int key,int val){
	////lock////////
	int k = fct(key);
	pthread_mutex_lock(&mt[k]);
	Node * start = &table[k];

	do{
		if(start->isFull == 0 || start->n == NULL || start->key == key){
			break;
		}
		start = start->n;
	}while(1);
	// if it has same key
	if(start->key == key){
		start->value = val;
		start->isFull = 1;
		printf("current id: %lx,k=%d,key=%d,value=%d\n",pthread_self(),k,key,val);
		pthread_mutex_unlock(&mt[k]);
		return;
	}

	// start from last element
	if(start->isFull != 0){
		Node * new = initNode();
		start->n = new;
		start = start->n;
	}
	start->key = key;
	start->value = val;
	start->isFull = 1;
	start->n = NULL;
	sleep(1);
	
	printf("current id: %lx,k=%d,key=%d,value=%d\n",pthread_self(),k,key,val);
	////unlock//////
	pthread_mutex_unlock(&mt[k]);

}
void delete(Node * table){
	Node * q, * p;
	int i;
	for(i =0; i < SIZE; i++){
		p = &table[i];
		p = p->n;
		while(p != NULL){
			q = p;
			p = p->n;
			free(q);
		}
	}
}
int get(Node * table,int key){
	int k = fct(key);
	Node * start = &table[k];

	do{
		if(start->key == key && start->isFull == 1){
			return start->value;
		}
		start = start->n;
	}while(start != NULL );
	return (-1);
}


// id = 0...4
void * run(void * arg){  // thread Id
	int i;
	Param * p = (Param *)arg;
	Node * table = p->table;
	int id = p->id;
	for(i = 0; i < M; i++){
		if(id == (i + 1) % TSIZE){
			set(table,datak[i],datav[i]);
		}
	}
}
void main(){
	time_t t;
	int j,y;
    j = time(&t);
	
	Node table[SIZE];
	initTable(table);
	/*
	initTable(table);
	set(table,9,8);
	set(table,2,7);
	set(table,14,3);
	set(table,14,9);
	printf("%d\n",get(table,9));
	printf("%d\n",get(table,2));
	printf("%d\n",get(table,14));
	delete(table);
	*/
	int i = 0;
	void * r; 
	pthread_t tid[TSIZE];
	for(i =0; i< SIZE; i++){
		pthread_mutex_init(&mt[i],NULL);
	}
	for(i=0; i < M;i++){
		datak[i] = i;
		datav[i] = i+100;
	}
	
	Param p[TSIZE];
	for(i = 0;i < TSIZE;i++){
		p[i].id = i;
		p[i].table = table;
	}
	for(i = 0;i < TSIZE;i++){
		pthread_create(&tid[i],NULL,run,(void *)&p[i]);

	}
	for(i = 0;i < TSIZE;i++){
		pthread_join(tid[i],&r);
	}
	delete(table);
	printf("all data has been inserted \n");
	
	y = time(&t);
	printf("%d\n",y-j);
}


