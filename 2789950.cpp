// Development environment: Ubuntu 20.04.1 LTS
// To compile: g++ Homework1.cpp -pthread
// To run: ./a.out interval_min interval_max np nt
// Example: ./a.out 101 200 2 2

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

int size;
struct interval {
        int beginInterval;
        int endInterval;
        int index;
        int* result = new int[size];
};
using namespace std;
int checkPrimeNumber(int n) {
    bool isPrime = true;
    if (n == 0 || n == 1) isPrime = false; 
    else for(int j = 2; j <= n/2; ++j) if (n%j == 0) {
        isPrime = false;
        break;
    }
    return isPrime;
}

void* primeInterval(void* in){
    interval* current = (interval*) in;
    int start = current->beginInterval, end= current->endInterval,i;
    for(i=start;i<=end;i++){
        if(checkPrimeNumber(i)){
            current->result[current->index] = i;
            current->index++;
        }
    }
    pthread_exit((void*) in);
}
int main(int argc, char* argv[]){
	int start= atoi(argv[1]), end = atoi(argv[2]), slave= atoi(argv[3]), thread= atoi(argv[4]);
    int i,counter=0,current=start,f;
    size= (end-start)/(slave*thread);
    
    interval* intervals = new interval[slave*thread];
    interval* ptr = intervals;

    while (counter!=slave*thread) {
        ptr->beginInterval = current; 
        current+=size;
        ptr->endInterval = current;
        current++;
        ptr->index = 0;
        ptr++;
        counter++;
    }

    cout << "Starting..." << endl;
    for (i=0; i<slave;i++){
        f = fork();
        if (f==0){
            long t;
            void *status;
            pthread_attr_t attr;
            pthread_t threads[thread];
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
            for(t=0; t<thread;t++) {
                cout<< "Main: In procces: "<< i+1 <<", creating thread "<<t+1<<". Intervals: "<<intervals[i*thread+t].beginInterval<<"-"<<intervals[i*thread+t].endInterval<<endl;
                void *ptr = &intervals[i*thread+t];
                sleep(1);
                if(pthread_create(&threads[t],&attr,primeInterval,ptr)) exit(-1);
            }
            sleep(2);
            cout << "Slave "<< i+1 << ": Done. Prime numbers are: ";
            for(t=0; t<thread;t++) {
                for(int j=0;intervals[i*thread+t].result[j] !=0 ;j++){
                    cout << intervals[i*thread+t].result[j] << ", ";
                }
            }
            cout << endl;
            sleep(1);
            pthread_attr_destroy(&attr);
            for(t=0; t<thread;t++) {
                if(pthread_join(threads[t], &status)) exit(-1);
                cout<<"Main: In procces: "<<i+1<<", completed join with thread "<< t+1<<" having a status of "<<(long)status<<endl;
            }
            sleep(2);
            if (i==slave-1&&f==0) cout<<"Main: Done."<<endl;
            break;
        }
    }
    wait(NULL);
}