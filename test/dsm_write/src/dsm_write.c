/*
 * dsm_write.c
 * An example multi-threaded Radix sort on a shared array.
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <argp.h>

#define MW 1 

int printTime(void);
int radix2();

int *shared_array;
int shared_aray_size;
int shared_sleep_time;

int sum=0;
int cur_index=0;
pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;

int check_file(){

	if (access("/tmp/haltcode", F_OK) == 0) {
		return 1;
	} else {
		return 0;
	}
		   
}


int multi_read(int tid,int count){

	// read the share page value;
	if(shared_array[count] == -99) 
		return -1;
	if(count%10 == 0)
		printf("Thread[%d] readvalue count=%d\n", tid,shared_array[count]);
    	if(count%50 ==0)
	    	printTime();

    	radix2(); radix2(); radix2();
	return 0; 
}

void dummy_read(int tid,int count)
{
	if(check_file()){
		printf("press enter\n");
		getchar();
	}

	if(count%10 == 0)
		printf("Thread[%d] readvalue i=%d local=%d\n", tid,cur_index,count);
    	if(cur_index%50 ==0)
	    	printTime();

    	radix2(); radix2(); radix2();
	return; 
}

void dummy(int tid)
{
    int index;
    if(check_file()){
	    printf("press enter\n");
	    getchar();
    }
    radix2(); radix2(); radix2();
    pthread_mutex_lock(&mutex);
    index = cur_index++;
    sum += shared_array[index];
    printf("Thread[%d] processed %d sum=%d\n", tid,index,sum);
    if(cur_index >= shared_aray_size){
    	printf("processed all values sum= %d\n",sum);
	printTime();
	exit(0);
    }   
    pthread_mutex_unlock(&mutex);
}

void *thread_func(void *arg)
{
    int thread_id = *((int *)arg);
    int count = 0;
	free(arg);

    while(1)
    {
	#ifdef MR
	count++;
	multi_read(thread_id,count);
	#endif
	#ifdef MW
	dummy(thread_id);
	#endif
	
	#ifdef SWMR
	count++;
	if(thread_id == 0)
		dummy(thread_id);
	else
		dummy_read(thread_id,count);
	#endif
    }
    return NULL;
}

// Structure to store command-line arguments
struct arguments {
    int threads;
    int arr_size;
    int sleep_time;
};

// Function to parse command-line arguments
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 't':
            arguments->threads = atoi(arg);
            break;
        case 'l':
            arguments->arr_size = atoi(arg);
            break;
        case 's':
            arguments->sleep_time = atoi(arg);
            break;
        case ARGP_KEY_END:
            if (arguments->threads == -1 || arguments->arr_size == -1 || arguments->sleep_time == -1) {
                fprintf(stderr, "Error: threads, arr_size, and sleep parameters are required.\n");
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// argp configuration
static struct argp_option options[] = {
    {"threads", 't', "NUM_THREADS", 0, "Number of threads"},
    {"arr_size", 'l', "ARR_SIZE", 0, "Size of the array"},
    {"sleep", 's', "SLEEP_TIME", 0, "Sleep time"},
    {0}
};

static struct argp argp = {options, parse_opt, NULL, NULL};

int main(int argc, char *argv[])
{
	struct arguments arguments;

	arguments.threads = -1;
	arguments.sleep_time = -1;
	arguments.arr_size = -1;
	
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if(arguments.threads < 1 || arguments.sleep_time < 0 || arguments.arr_size < 1)
	{
		perror("Invalid arguments.\n");
		exit(1);
	}

    shared_aray_size = arguments.arr_size;
    shared_sleep_time = arguments.sleep_time;

	printf("Number of threads: %d\n", arguments.threads);
	printf("Array size: %d\n", shared_aray_size);
	printf("Sleep interval (seconds): %d\n", shared_sleep_time);
	
	/* TODO: Clean this up, this shouldn't be macros. */
	#ifdef MR
	printf("MR\n");
	#endif
	#ifdef MW
	printf("MW\n");
	#endif
	#ifdef SWMR
	printf("SWMR\n");
	#endif

    printTime();
    pthread_t* ptid = malloc(sizeof(pthread_t) * arguments.threads);
	shared_array = malloc(sizeof(int) * shared_aray_size);

    for(int i = 0;i < shared_aray_size; i++){
	    shared_array[i]=i+1;
    }

	/* Create threads */
    for(int i = 0; i < arguments.threads; i++)
    {
		int *thread_id = malloc(sizeof(int));
		*thread_id = i + 1;
        int ret = pthread_create(&ptid[i], NULL, thread_func, (void*)thread_id);
		if (ret != 0)
		{
			perror("pthread_create");
			exit(1);
		}
    }

	/* Join threads */
	for (int i = 0; i < arguments.threads; i++)
	{
		int ret = pthread_join(ptid[i], NULL);
		if (ret != 0)
		{
			perror("pthread_join");
			exit(1);
		}
	}

    return 0;
}
