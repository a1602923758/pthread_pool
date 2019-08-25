#include "pthread_pool.h"
#include <unistd.h>
#include <iostream>
using namespace std;
class MyTask:public Task{
	public:
			MyTask(int d):Task(d){}
			MyTask(string n,int d):Task(n,d){}
			void run(){
				cout<<data<<endl;
				sleep(1);
			}
};

void* func(void* args){
	vector<Task*> *task=static_cast<vector<Task*>* >(args);
	vector<Task*>::iterator iter=task->begin();
	while(iter!=task->end()){
		(*iter)->run();
		++iter;
	}
}

int main(){
	PthreadPool pool(5);
	int a,b,c;
	a=2;b=5;c=7;
	vector<MyTask> vec;
	for (int i=0;i<10;++i){
		MyTask task(i);
		vec.push_back(task);
	}
	for (int i=0;i<10;++i){
		pool.submit(&vec[i]);
	}
	sleep(15);
	pool.wait();
	return 0;


}
