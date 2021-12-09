#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#define MAX_READ 10

struct proc {                                           //Structure to store processes
        char name[5];                                   //Character array to store name of process
        long unsigned int n;                            //Long unsigned int to store size of process
        long unsigned int start;                        //Long unsigned int to store the starting position of process
};
typedef struct proc proc;                               //type definition of structure

int isEmpty(proc *tp) {                                 //Function to find if structure array is empty or not
        if (tp[0].name[0] != '\0') {                    //looping through array searching for stored values
                return 1;                               //returns 1 if not empty
        }
        return 0;                                       //returns 0 if empty
}

int allocated(proc *tp) {                               //Function to find how many processes are allocated
        int total = 0;                                  //int to store total processes
        for (int i = 0; i < 20; ++i) {                  //looping through array 
                if (tp[i].name[0] != '\0') {            //finding if there is a process allocated 
                        ++total;                        //if allocated process found, increments total
                }
                else {
                        break;
                }
        }
        return total;                                   //return total amount of processes
}

void available(proc *tp, long unsigned int tot) {       //Function to find and print all available memory
        long unsigned int av = 0;                       //Long unsigned int to store available memory
        long unsigned int remainder = 0;                //Long unsigned int to store remainder of memory
        long unsigned int st = 0;                       //Long unsigned int to store starting position of available memory
        if (isEmpty(tp) == 0) {                         //checking if array is empty
                printf("(%lu, 0)\n", tot);              //if empty, prints total memory starting at the 0 position
                return;                                 //returns from function
        }
        for (int i = 0; i < allocated(tp); ++i) {       //loops through array to find remainder of memory
                remainder += tp[i].n;
        }
        
        if ((remainder - tot) == 0) {                   //if remainder is zero, prints FULL
                printf("FULL\n");
                return;
        }
        if (tp[0].start != 0) {                         //checks for available memory that is before the first process
                printf("(%lu, 0) ", tp[0].start);
        }
        for  (int i = 0; i < allocated(tp) + 1; ++i) {  //loops through array to find all available memory and prints
                if (tp[i+1].name[0] != '\0'){
                        av =  tp[i+1].start - (tp[i].start + tp[i].n);
                        if (av != 0){
                                st = tp[i].start + tp[i].n;
                                printf("(%lu, %lu) ", av, st);
                        }
                  }
                else {
                        remainder = tot - (tp[i].start + tp[i].n);
                        if (remainder == 0) {
                                printf("\n");
                                return;
                        }
                        av = tp[i].start + tp[i].n;
                        printf("(%lu, %lu)\n", remainder, av);
                        return;
                }
                av = 0;
        }
        return;                                         //returns after printing all available memory
}
void assigned(proc *tp) {                               //Function for finding and printing all allocated memory
        if (isEmpty(tp) == 0) {                         //checks if array is empty, if so prints NONE and returns
                printf("NONE\n");
                return;
        }
        for (int i = 0; i < allocated(tp); ++i) {       //looping through array to find and print all allocated memory
                if (tp[i].name[0] != '\0') {
                        printf("(%s, %lu, %lu) ", tp[i].name, tp[i].n, tp[i].start);
                }
                else {
                        break;
                }
        }
        printf("\n");                                   //prints new line
        return;                                         //returns
}
void find(proc *tp, char *name) {                       //Function to find desired process, if found prints allocated memory, if not prints FAULT
        for (int i = 0; i < allocated(tp); ++i) {       //looping through array to find desired process
                if (strcmp(tp[i].name, name) == 0) {
                       printf("(%s, %lu, %lu)\n", tp[i].name, tp[i].n, tp[i].start);
                       return;
                }
                else if(tp[i].name[0] == '\0') {
                        break;
                }                
        }
        printf("FAULT\n");
        return;                                         //returns
}
int release(proc *tp, char *name) {                     //Function to release a desired process from array, if found prints process that is freed
        for (int i = 0; i < allocated(tp); ++i) {       //looping through array to find desired process
                if (strcmp(tp[i].name, name) == 0) {
                        printf("FREE %s %ld %ld\n", tp[i].name, tp[i].n, tp[i].start);
                        tp[i].name[0] = '\0';
                        tp[i].n = 0;
                        tp[i].start = 0;
                        for (int j = i; j < (allocated(tp) +1); ++j) {
                                strcpy(tp[j].name, tp[j+1].name);
                                tp[j].n = tp[j+1].n;
                                tp[j].start = tp[j+1].start;
                                tp[j+1].name[0] = '\0';
                                tp[j+1].n = 0;
                                tp[j+1].start = 0;
                        }
                        return  i;
                }
                else if(tp[0].name[0] == '\0') {
                        break;
                }
        }
        return -1;                                      //returns -1 if not found
}


void addToTp(proc *tp, char *name, long unsigned int n, int j) {        //Function to add process to array to the desired position
        if (isEmpty(tp) == 0) {                                         //if array empty, adds process to start of array
                strcpy(tp[0].name, name);
                tp[0].n = n;
                tp[0].start = j;
                printf("ALLOCATED %s %ld\n", tp[0].name, tp[0].start);
                return;
        }
        for (int i = allocated(tp)+ 1; i >= j; --i) {                           //looping through array to shift all processes to make space for new process
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        strcpy(tp[i+1].name, tp[i].name);
                        tp[i+1].n = tp[i].n;
                        tp[i+1].start = tp[i].start;
                        tp[i].name[0] = '\0';
                        tp[i].n = 0;
                        tp[i].start = 0;
                }
        }
        long unsigned int st = 0;                                               //long unsigned int to store new processes starting position
        strcpy(tp[j].name, name);
        tp[j].n = n;
        if (j != 0) {
                st = tp[j-1].start + tp[j-1].n;
        }
        tp[j].start = st;
        printf("ALLOCATED %s %ld\n", tp[j].name, tp[j].start);                  //prints newly allocated process
        return;                                                                 //returns
}

long unsigned int findBestFit(proc *tp, char *name, long unsigned int n, long unsigned int tot) {       //Function to find opening for process using bestfit algorithm
        int j = 0;                                                              //int to store indicies of desired position for process
        long unsigned int diff = 0;                                             //long unsigned int to store difference
        long unsigned int bestDiff = 0;                                         //long unsigned int to store the best difference 
        if (n > tot) {                                                          //if size of new process is too large, returns -1
                return -1;
        }
        if (isEmpty(tp) == 0) {                                                 //checks if array is empty, if so returns 0
                return j;
        }
        bestDiff = tp[0].start;                                                 //start best difference with the start of the first process to check for available memory before the first process
        for (int i = 0; i < allocated(tp) + 1; ++i) {                           //looping through array to find available that is the best fit for the process
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] != '\0')) {
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if ((n <= diff) && (bestDiff == 0)){
                                bestDiff = diff;
                                j = i+1;
                        }
                        else if ((n <= diff) && (bestDiff > diff)){
                                bestDiff = diff;
                                j = i+1;
                        }
                }
                else if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (((bestDiff == 0) || (diff < bestDiff)) && (diff >= n)) {
                                bestDiff = diff;
                                j = i+1;
                        }
                }
        }
        if (bestDiff == 0) {                                                    //if best fit is not found, returns -1
                return -1;
        }
        return j;                                                               //returns indicies for best fit position
}
void bestfit(char** argv, long unsigned int tot) {                              //Function for 'best fit' algorithm
        char job[10];                                                           //char array to store job(REQUEST, RELEASE, LIST AVAILABLE, LIST ASSIGNED, FIND)
        char name[5];                                                           //char array to store name of process
        char request[8] = {'R','E','Q','U','E','S','T','\0'};                   //char array to compare with job
        char rlease[8] = {'R','E','L','E','A','S','E','\0'};
        char list[5] = {'L','I','S','T','\0'};
        char av[10] = {'A','V','A','I','L','A','B','L','E','\0'};
        char as[9] = {'A','S','S','I','G','N','E','D','\0'};
        char f[5] = {'F','I','N','D','\0'};
        long unsigned int j;                                                    //long unsigned int to store indicies of the best fit
        long unsigned int n;                                                    //long unsigned int to store size of process
        FILE *fd;                                                               //input file 
        proc tp[20];                                                            //structure array for processes
        for (int i = 0; i < 20; ++i){                                           //for loop to initialize structure 
                tp[i].name[0] = '\0';
                tp[i].n = 0;
                tp[i].start = 0;
        }
        fd = fopen(argv[3], "r");                                               //opening input file
        while (fscanf(fd, "%s", job) != EOF) {                                  //looping through input file
                if (strcmp(job, request) == 0) {                                //REQUEST job code
                        fscanf(fd, "%s", name);                                 
                        fscanf(fd, "%lu", &n);
                        j = findBestFit(tp, name, n, tot);
                        if (j == -1){
                                printf("FAIL REQUEST %s %ld\n", name, n);       //if there is not a fit for process, prints fail request
                                }
                        else {
                                addToTp(tp, name, n, j);
                        }
                }
                else if (strcmp(job, rlease) == 0) {                            //RELEASE job code
                        fscanf(fd, "%s", name);
                        if (release(tp, name) == -1){
                                printf("FAIL RELEASE %s\n", name);              //if process is not found in order to release, prints fail release
                        }
                }
                else if (strcmp(job, list) == 0) {                              
                        fscanf(fd, "%s", job);
                        if (strcmp(job, av) == 0) {                             //LIST AVAILABLE job code
                                available(tp, tot);
                        }
                        else if (strcmp(job, as) == 0) {                        //LIST ASSIGNED job code
                                assigned(tp);
                        }
                }
                else if (strcmp(job, f) == 0) {                                 //FIND job code
                        fscanf(fd, "%s", name);
                        find(tp, name);
                }
        }
}
long unsigned int findWorstFit(proc *tp, char *name, long unsigned int n, long unsigned int tot) {      //Function to find worst fit for process
        int j = 0;                                      //int for storing indicies of worst fit
        long unsigned int diff = 0;                     //long unsigned int to store difference
        long unsigned int worstDiff = 0;                //long unsigned int to store worst difference
        
        if (n > tot) {                                  //if size of desired process is too large return -1
                return -1;
        }
        
        if (isEmpty(tp) == 0) {                         //if array is empty returns 0
                return j;
        }

        worstDiff = tp[0].start;                        //start worst difference with first processes starting position to account for possible available memory before first process
        for (int i = 0; i < allocated(tp) + 1; ++i) {                           //loop through array to find worst difference of available allocated memory
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] != '\0')) {
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if ((n < diff) && (worstDiff == 0)){
                                worstDiff = diff;
                                j = i+1;
                        }
                        else if ((n < diff) && (worstDiff < diff)){
                                worstDiff = diff;
                                j = i+1;
                        }
                }
                else if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (((worstDiff == 0) || (diff > worstDiff)) && (diff >= n)) {
                                worstDiff = diff;
                                j = i+1;
                        }
                }
        }
        if (worstDiff == 0) {                           //if there worst difference is not found, returns -1
                return -1;
        }
        return j;                                       //returns indicies of array where there is the worst difference
}

void worstfit(char** argv, long unsigned int tot) {                     //Function for 'worst fit' algorithm
        char job[10];                                                   //char array to store job
        char name[5];                                                   //char array to store name of process
        char request[8] = {'R','E','Q','U','E','S','T','\0'};           //char array to compare with job
        char rlease[8] = {'R','E','L','E','A','S','E','\0'};
        char list[5] = {'L','I','S','T','\0'};
        char av[10] = {'A','V','A','I','L','A','B','L','E','\0'};
        char as[9] = {'A','S','S','I','G','N','E','D','\0'};
        char f[5] = {'F','I','N','D','\0'};
        long unsigned int j;                                            //long unsigned int to store indicies of the worst fit
        long unsigned int n;                                            //long unsigned int to store size of process
        FILE *fd;                                                       //input file
        proc tp[20];                                                    //structure array for processes
        for (int i = 0; i < 20; ++i){                                   //loop through array to initialize structure
                tp[i].name[0] = '\0';
                tp[i].n = 0;
                tp[i].start = 0;
        }
        fd = fopen(argv[3], "r");                                       //open file
        while (fscanf(fd, "%s", job) != EOF) {                          //looping through file
                if (strcmp(job, request) == 0) {                        //REQUEST job code
                        fscanf(fd, "%s", name);
                        fscanf(fd, "%lu", &n);
                        j = findWorstFit(tp, name, n, tot);
                        if (j == -1){
                                printf("FAIL REQUEST %s %ld\n", name, n);
                        }
                        else {
                                addToTp(tp, name, n, j);
                        }
                }
                else if (strcmp(job, rlease) == 0) {                    //RELEASE job code
                        fscanf(fd, "%s", name);
                        if (release(tp, name) == -1){
                                printf("FAIL RELEASE %s\n", name);}
                }
                else if (strcmp(job, list) == 0) {
                        fscanf(fd, "%s", job);
                        if (strcmp(job, av) == 0) {                     //LIST AVAILABLE job code
                                available(tp, tot);
                        }
                        else if (strcmp(job, as) == 0) {                //LIST ASSIGNED job code
                                assigned(tp);
                        }
                }
                else if (strcmp(job, f) == 0) {                         //FIND job code
                        fscanf(fd, "%s", name);
                        find(tp, name);
                }
        }
}
int findFirstFit(proc *tp, char *name, long unsigned int n, long unsigned int tot){     //Function to find first available fit for process
        if (n > tot) {                                          //if size of process is too large, returns -1
                return -1;
        }
       
        if (isEmpty(tp) == 0) {                                 //if array is empty, returns 0
                return 0;
        }
        long unsigned int diff = tp[0].start;                   //long unsinged int that stores difference between processes
        if (diff >= n) {                                        //checks if available memory before first process is quality size for new process
                return 0;
        }

        for (int i = 0; i < allocated(tp) + 1; ++i) {           //loop through array to find first available fit for process
                if (tp[i+1].name[0] != '\0'){
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if (diff >= n){
                         return (i + 1);
                        }
                }
                else if (tp[i].name[0] != '\0') {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (diff >= n) {
                               return (i + 1);
                        }
                }
        }
        return -1;                                              //if there are no available fits, returns -1
}
void firstfit(char** argv, long unsigned int tot) {             //Function for 'first fit' algorithm
        char job[10];
        char name[5];
        char request[8] = {'R','E','Q','U','E','S','T','\0'};
        char rlease[8] = {'R','E','L','E','A','S','E','\0'};
        char list[5] = {'L','I','S','T','\0'};
        char av[10] = {'A','V','A','I','L','A','B','L','E','\0'};
        char as[9] = {'A','S','S','I','G','N','E','D','\0'};
        char f[5] = {'F','I','N','D','\0'};
        long unsigned int j;
        long unsigned int n;
        FILE *fd;
        proc tp[20];
        for (int i = 0; i < 20; ++i){
                tp[i].name[0] = '\0';
                tp[i].n = 0;
                tp[i].start = 0;
        }
        fd = fopen(argv[3], "r");
        while (fscanf(fd, "%s", job) != EOF) {
                if (strcmp(job, request) == 0) {
                        fscanf(fd, "%s", name);
                        fscanf(fd, "%lu", &n);
                        j = findFirstFit(tp, name, n, tot);
                        if (j == -1){
                                printf("FAIL REQUEST %s %ld\n", name, n);
                        }
                        else {
                                addToTp(tp, name, n, j);
                        }
                }
                else if (strcmp(job, rlease) == 0) {
                        fscanf(fd, "%s", name);
                        if (release(tp, name) == -1){
                                printf("FAIL RELEASE %s\n", name);}
                }
                else if (strcmp(job, list) == 0) {
                        fscanf(fd, "%s", job);
                        if (strcmp(job, av) == 0) {
                                available(tp, tot);
                        }
                        else if (strcmp(job, as) == 0) {
                                assigned(tp);
                         }
                }
                else if (strcmp(job, f) == 0) {
                        fscanf(fd, "%s", name);
                        find(tp, name);
                }
                }
}

int findNextFit(proc *tp, char *name, int pos, long unsigned int n, long unsigned int tot) {    //Function to find next fit for process
        if (n > tot) {                                          //if process size is too large, returns -1
                return -1;
        }
        if (isEmpty(tp) == 0) {                                 //if array is empty, returns 0
                return 0;
        }
        long unsigned int diff = tp[0].start;                   //long unsigned int to store available memory between processes
        
        for (int i = pos; i < allocated(tp) + 4; ++i) {         //looping through array to find next fit for process starting at the last position of the last allocated process
                if (tp[i+1].name[0] != '\0'){
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if (diff >= n){
                         return (i + 1);
                        }
                }
                else if (tp[i].name[0] != '\0') {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (diff >= n) {
                               return (i + 1);
                        }
                }
        }
        
        diff = tp[0].start;                                     //restarting search through array if next fit was not found
        if (diff >= n) {
                return 0;
        }
        
        for (int i = 0; i <= pos; ++i) {
                if (tp[i+1].name[0] != '\0'){
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if (diff >= n){
                         return (i + 1);
                        }
                }
                else if (tp[i].name[0] != '\0') {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (diff >= n) {
                               return (i + 1);
                        }
                }
        }
        return -1;                                              //if next fit was not found, returns -1
}

void nextfit(char** argv, long unsigned int tot) {              //Function for 'next fit' algorithm
        char job[10];
        char name[5];
        char request[8] = {'R','E','Q','U','E','S','T','\0'};
        char rlease[8] = {'R','E','L','E','A','S','E','\0'};
        char list[5] = {'L','I','S','T','\0'};
        char av[10] = {'A','V','A','I','L','A','B','L','E','\0'};
        char as[9] = {'A','S','S','I','G','N','E','D','\0'};
        char f[5] = {'F','I','N','D','\0'};
        int pointer = 0;
        long unsigned int j;
        long unsigned int n;
        FILE *fd;
        proc tp[20];
        for (int i = 0; i < 20; ++i){
                tp[i].name[0] = '\0';
                tp[i].n = 0;
                tp[i].start = 0;
        }
        fd = fopen(argv[3], "r");
        while (fscanf(fd, "%s", job) != EOF) {
                if (strcmp(job, request) == 0) {
                        fscanf(fd, "%s", name);
                        fscanf(fd, "%lu", &n);
                        j = findNextFit(tp, name, pointer, n, tot);
                        if (j == -1){
                                        printf("FAIL REQUEST %s %ld\n", name, n);
                        }
                        else {
                                if (isEmpty(tp) == 0) {
                                        addToTp(tp, name, n, j);
                                }
                                else {
                                        pointer = j;
                                        addToTp(tp, name, n, j);
                                }
                        }
                }
                else if (strcmp(job, rlease) == 0) {
                        fscanf(fd, "%s", name);
                        j = release(tp, name);
                        if (j == -1){
                                printf("FAIL RELEASE %s\n", name);
                        }
                        else {
                                --pointer;
                        }
                }
                else if (strcmp(job, list) == 0) {
                        fscanf(fd, "%s", job);
                        if (strcmp(job, av) == 0) {
                                available(tp, tot);
                        }
                        else if (strcmp(job, as) == 0) {
                                assigned(tp);
                        }
                }
                else if (strcmp(job, f) == 0) {
                        fscanf(fd, "%s", name);
                        find(tp, name);
                }
        }
}

int main (int argc, char** argv) {                              //main function
        char fit[MAX_READ];                                     //char array to get desired algorithm
        char bf[8] = {'B','E','S','T','F','I','T','\0'};        //char arrays to compare to confirm desired algorithm
        char wf[9] = {'W','O','R','S','T','F','I','T','\0'};
        char ff[9] = {'F','I','R','S','T','F','I','T','\0'};
        char nf[8] = {'N','E','X','T','F','I','T','\0'};
        long int N;                                             //long int to store given total allocation memory
        strcpy(fit, argv[1]);                                   //get desired algorithm
        N =  atoi(argv[2]);                                     //get desired total allocation memory
        
        if (strcmp(fit, bf) == 0) {                             //BESTFIT 
                bestfit(argv, N);
        }
        else if (strcmp(fit, wf) == 0) {                        //WORSTFIT
                worstfit(argv, N);
        }
        else if (strcmp(fit, ff) == 0) {                        //FIRSTFIT
                firstfit(argv, N);
        }
        else if (strcmp(fit, nf) == 0) {                        //NEXTFIT
                nextfit(argv, N);
        }
        
        return 0;
}
