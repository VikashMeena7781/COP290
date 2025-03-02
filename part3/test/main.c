#include <stdio.h>
#include "hm.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>
#include "mythread.h"

void readFile(void *);

struct hashmap_s hashmap;

int printer(struct hashmap_element_s *const e) {
	int* count = (int*) e->data;
	printf("key %s, count %d\n", e->key, *count);
	return 0;
}

int main(int argc, char** argv) {
	hashmap_create(&hashmap);
	printf("Testing threads!\n");
	mythread_init();
	for(int i=1;i<argc;i++) {
		mythread_create(readFile, (void *) argv[i]);
	}
	mythread_join();
	hashmap_iterator(&hashmap, printer);
	printf("Testing threads done!\n\n");
}

static void inc_word_count (char* word) {
	printf("Inside inc_word_count %s\n", word);
	acquire_bucket(&hashmap, word);
	int* c = (int*) hashmap_get(&hashmap, word);
	int* c1 = (int*) malloc(sizeof(int));
	*c1 = 1;
	if(c != NULL) {
		for(int i = 0; i < *c; i ++) {
			mythread_yield();
		}
		*c1 = *c + 1;
	}
	printf("Inside inc_word_count: c1 %d\n", *c1);
	hashmap_put(&hashmap, word, c1);
	release_bucket(&hashmap, word);
	puts("finish inc_word_count");
}

void readFile(void *args) {
	// Perform following steps
	// 1. Read a word from the file
	// 2. Acquire lock on relevent hashmap bucket
	// 3. Get count of word from hashmap. Let value returened by hashmap be x.
	// 4. Yield thread
	// 5. Set new count of the word as x+1.
	// 6. Release lock
	// 7. Repeat for all words in the file.
	char *filename = (char*)args;
	FILE *fp = fopen(filename,"r");
	if(fp==NULL)
		return;
	char ch;
	int i=0;
	char arr[25];
	int val =-1;
	while((ch = fgetc(fp))!=EOF) {
		if(ch!=' ' && ch!='\n' && ch!='\t' && ch!='\0') {
			arr[i] = ch;
			i++;
		} else {
			arr[i] = 0;
			inc_word_count(arr);
			for(int j=0;j<25;j++)
				arr[j]='\0';
			i=0;
		}
	}
}
