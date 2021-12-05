#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#define MAX_READ 10
struct proc {
        char name[5];
        long unsigned int n;
        long unsigned int start;
};
typedef struct proc proc;
int isEmpty(proc *tp) {
        if (tp[0].name[0] != '\0') {
                return 1;
        }
        return 0;
}
long unsigned int allocated(proc *tp) {
        long unsigned int ans = 0;
        for (int i = 0; i < sizeof(tp); ++i) {
                if (tp[i].name[0] != '\0') {
                        ans += tp[i].n;
                }
 }
        return ans;
}
void available(proc *tp, long unsigned int tot) {
        long unsigned int av = 0;
        long unsigned int remainder = tot;
        long unsigned int st = 0;
        if (isEmpty(tp) == 0) {
                printf("(%lu,0)\n", tot);
                return;
        }
        for (int i = 0; i < sizeof(tp); ++i) {
                remainder += tp[i].n;
        }
        remainder -= tot;
        if (remainder == 0) {
                printf("FULL\n");
                return;
        }
        for  (int i = 0; i < sizeof(tp); ++i) {
                if (tp[i+1].name[0] != '\0'){
                        av =  tp[i+1].start - (tp[i].start + tp[i].n);
                        if (av != 0){
                                st = tp[i].start + tp[i].n;
                                printf("(%lu,%lu)", av, st);
                        }
                  }
                else if (remainder != 0) {
                        remainder = tot - (tp[i].start + tp[i].n);
                        av = tp[i].start + tp[i].n;
                        printf("(%lu,%lu)\n", remainder, av);
                        break;
                }
                av = 0;
        }
        return;
}
void assigned(proc *tp) {
        if (isEmpty(tp) == 0) {
                printf("NONE");
                return;
        }
        for (int i = 0; i < sizeof(tp); ++i) {
                if (tp[i].name[0] != '\0') {
                        printf("(%s,%lu,%lu)", tp[i].name, tp[i].n, tp[i].start);
                }
        }
        printf("\n");
        return;
}
void find(proc *tp, char *name) {
        for (int i = 0; i < sizeof(tp); ++i) {
          if (strcmp(tp[i].name, name) == 0) {
                       printf("(%s,%lu,%lu)\n", tp[i].name, tp[i].n, tp[i].start);
                       return;
                }
        }
        printf("FAULT\n");
        return;
}
int release(proc *tp, char *name) {
        for (int i = 0; i < sizeof(tp); ++i) {
                if (strcmp(tp[i].name, name) == 0) {
                        printf("FREE %s %ld %ld\n", tp[i].name, tp[i].n, tp[i].start);
                        tp[i].name[0] = '\0';
                        tp[i].n = 0;
                        tp[i].start = 0;
                        for (int j = i; j < (sizeof(tp) - 2); ++j) {
                                strcpy(tp[j].name, tp[j+1].name);
                                tp[j].n = tp[j+1].n;
                                tp[j].start = tp[j+1].start;
                                tp[j+1].name[0] = '\0';
                                tp[j+1].n = 0;
                                tp[j+1].start = 0;
                        }
                        return  1;
                }
        }
        return -1;
}


void addToTp(proc *tp, char *name, long unsigned int n, int j) {
        if (isEmpty(tp) == 0) {
                strcpy(tp[0].name, name);
                tp[0].n = n;
                tp[0].start = j;
                printf("ALLOCATED %s %ld\n", tp[0].name, tp[0].start);
                return;
        }
        for (int i = (sizeof(tp) - 2); i > j; --i) {
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        strcpy(tp[i+1].name, tp[i].name);
                        tp[i+1].n = tp[i].n;
                        tp[i+1].start = tp[i].start;
                        tp[i].name[0] = '\0';
                        tp[i].n = 0;
                        tp[i].start = 0;
                }
        }
        long unsigned int st = 0;
        strcpy(tp[j+1].name, name);
        tp[j+1].n = n;
        for (int i = 0; i < (j+1); ++i) {
                st += tp[i].n;
        }
        tp[j+1].start = st;
        printf("ALLOCATED %s %ld\n", tp[j+1].name, tp[j+1].start);
        return;
}

long unsigned int findBestFit(proc *tp, char *name, long unsigned int n, long unsigned int 
tot) {
        int j = 0;
        long unsigned int diff = 0;
        long unsigned int bestDiff = 0;
        if (isEmpty(tp) == 0) {
                return j;
        }
        for (int i = 0; i < sizeof(tp); ++i) {
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] != '\0')) {
                        diff = tp[i+1].start - tp[i].n;
                        if ((n < diff) && (bestDiff == 0)){
                                bestDiff = diff;
                                j = i;
                        }
                        else if ((n < diff) && (bestDiff > diff)){
                                bestDiff = diff;
                                j = i;
                        }
                }
                else if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        diff = tot - allocated(tp);
                        if ((bestDiff == 0) || (diff < bestDiff)) {
                                bestDiff = diff;
                                j = i;
                        }
                }
        }
        if (bestDiff == 0) {
                return -1;
                }
        return j;
}
void bestfit(char** argv, long unsigned int tot) {
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
        }
        fd = fopen(argv[3], "r");
        while (fscanf(fd, "%s", job) != EOF) {
                if (strcmp(job, request) == 0) {
                        fscanf(fd, "%s", name);
                        fscanf(fd, "%lu", &n);
                        j = findBestFit(tp, name, n, tot);
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
                                printf("FAIL RELEASE %s\n", name);
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
long unsigned int findWorstFit(proc *tp, char *name, long unsigned int n, long unsigned int
 tot) {
 int j = 0;
        long unsigned int diff = 0;
        long unsigned int worstDiff = 0;
        if (isEmpty(tp) == 0) {
                return j;
        }
        for (int i = 0; i < sizeof(tp); ++i) {
                if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] != '\0')) {
                        diff = tp[i+1].start - tp[i].n;
                        if ((n < diff) && (worstDiff == 0)){
                                worstDiff = diff;
                                j = i;
                        }
                        else if ((n < diff) && (worstDiff < diff)){
                                worstDiff = diff;
                                j = i;
                        }
                }
                else if ((tp[i].name[0] != '\0') && (tp[i+1].name[0] == '\0')) {
                        diff = tot - allocated(tp);
                        if ((worstDiff == 0) || (diff > worstDiff)) {
                                worstDiff = diff;
                                j = i;
                        }
                }
        }
        if (worstDiff == 0) {
                return -1;
        }
        return j;
}

void worstfit(char** argv, long unsigned int tot) {
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
        }
        fd = fopen(argv[3], "r");
        while (fscanf(fd, "%s", job) != EOF) {
                if (strcmp(job, request) == 0) {
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
int findFirstFit(proc *tp, char *name, long unsigned int n, long unsigned int tot){
        if (isEmpty(tp) == 0) {
                return 0;
        }
        long unsigned int diff = 0;
        for (int i = 0; i < sizeof(tp); ++i) {
                if (tp[i+1].name[0] != '\0'){
                        diff = tp[i+1].start - (tp[i].start + tp[i].n);
                        if (diff > n){
                         return i;
                        }
                }
                else {
                        diff = tot - (tp[i].start + tp[i].n);
                        if (diff > n) {
                                return i;
                        }
                }
        }
        return -1;
}
void firstfit(char** argv, long unsigned int tot) {
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
int findNextFit(proc *tp, char *name, char *prevName, long unsigned int n, long unsigned in
t tot) {
        int pos;
        for (int i = 0; i < sizeof(tp); ++i) {
                if (strcmp(prevName, tp[i].name) == 0) {
                        pos = i;
                }
        }
        for (int i = pos; i < sizeof(tp); ++i) {
                if (tp[i+1].name[0] != '\0') {
                        if ((tp[i+1].start - (tp[i].start + tp[i].n)) > n) {
                                return i;
                        }
                }
                else {
                        if ((tot - (tp[i].start + tp[i].n)) > n){
                                return i;
                        }
                }
        }
        return -1;
}

void nextfit(char** argv, long unsigned int tot) {
        char job[10];
        char name[5];
        char prevName[5];
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
        }
        fd = fopen(argv[3], "r");
        while (fscanf(fd, "%s", job) != EOF) {
                if (strcmp(job, request) == 0) {
                        fscanf(fd, "%s", name);
                        fscanf(fd, "%lu", &n);
                        if (isEmpty(tp) == 0){
                                addToTp(tp, name, n, 0);
                        }
                        else {
                                j = findNextFit(tp, name, prevName, n, tot);
                                if (j == -1){
                                        printf("FAIL REQUEST %s %ld\n", name, n);
                                }
                                addToTp(tp, name, n, j);
                                }
                        strcpy(prevName,name);
                }
                else if (strcmp(job, rlease) == 0) {
                        fscanf(fd, "%s", name);
                        if (release(tp, name) == -1){
                                printf("FAIL RELEASE %s\n", name);
                        }
                        strcpy(prevName,name);
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

int main (int argc, char** argv) {
        char fit[MAX_READ];
        char bf[8] = {'B','E','S','T','F','I','T','\0'};
        char wf[9] = {'W','O','R','S','T','F','I','T','\0'};
        char ff[9] = {'F','I','R','S','T','F','I','T','\0'};
        char nf[8] = {'N','E','X','T','F','I','T','\0'};
        long int N;
        strcpy(fit, argv[1]);
        N =  atoi(argv[2]);
        if (strcmp(fit, bf) == 0) {
                bestfit(argv, N);
        }
        else if (strcmp(fit, wf) == 0) {
                worstfit(argv, N);
        }
        else if (strcmp(fit, ff) == 0) {
                firstfit(argv, N);
        }
        else if (strcmp(fit, nf) == 0) {
                nextfit(argv, N);
        }
        return 0;
}