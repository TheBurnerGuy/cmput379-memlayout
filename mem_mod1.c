#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include "memlayout.h"

int main(){
	
	struct memregion* regionlist = malloc(sizeof(struct memregion)*35);
	if (regionlist == NULL){
		printf("Failed to allocate memory\n");
		exit(0);
	}
	printf("Entering function\n");
	int memlayout = get_mem_layout(regionlist,35);
	printf("End: %u\n",memlayout);
	int i;
	for(i = 0; i<35; ++i){
		printf("memregion[%d]:\tfrom:%p\tto:%p\tmode:%d\n",i,regionlist[i].from,regionlist[i].to,regionlist[i].mode);
	}
}

//jmp_buf env;

//Test Function
//~ void signal_handler(int sig){
	//~ printf("I received SEGFAULT!\n");
	//~ siglongjmp(env, 1);
//~ }

//int main(){
	
	//~ jmp_buf env;
	//~ int test = sigsetjmp(env,1);
	//~ 
	//~ void signal_handler(int sig){
		//~ siglongjmp(env, 1);
	//~ }
	//~ 
	//~ struct sigaction segv;
	//~ segv.sa_handler = signal_handler;
	//~ sigemptyset(&segv.sa_mask);
	//~ segv.sa_flags = 0;
	//~ sigaction(SIGSEGV,&segv,0);
	//~ 
	//~ unsigned int* testpointer = (unsigned int*)0xFFFF;
	//~ if(!test) printf("Printing: %d\n",*testpointer);
	//~ printf("Cannot be written, reading\n");
	//~ 
	//~ test = sigsetjmp(env,1);
	//~ if(!test) *testpointer = 1;
	//~ printf("Done\n");
	
	//return 0;
//}
