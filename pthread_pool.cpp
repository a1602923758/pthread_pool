#include "pthread_pool.h"
#include <iostream>
#include <future>
using namespace std;

vector<Task*> PthreadPool::myTaskList;
vector<pthread_t> PthreadPool::ablePthreads;
vector<pthread_t> PthreadPool::buzyPthreads;
pthread_mutex_t PthreadPool::myMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t PthreadPool::myCond=PTHREAD_COND_INITIALIZER;
bool PthreadPool::flag=true;


int PthreadPool::moveBuzyPthreads(pthread_t pid){
	vector<pthread_t>::iterator ableIter=ablePthreads.begin();
	while(ableIter!=ablePthreads.end()){
		if (pid==*ableIter){
			ablePthreads.erase(ableIter);
			buzyPthreads.push_back(pid);
			return 0;
		}
		++ableIter;
	}
	return -1;
}
int PthreadPool::moveAblePthreads(pthread_t pid){
	vector<pthread_t>::iterator buzyIter=buzyPthreads.begin();
	while(buzyIter!=buzyPthreads.end()){
		if (pid==*buzyIter){
			buzyPthreads.erase(buzyIter);
			ablePthreads.push_back(pid);
			return 0;
		}
		++buzyIter;
	}
	return -1;
}

void PthreadPool::create(int n){
	pthread_t pid;
	for (int i=0;i<n;++i){
		if(pthread_create(&pid,NULL,pthreadFunc,NULL)!=0) exit(0);
		ablePthreads.push_back(pid);
	}
	cout<<"PthreadPool create success"<<endl;
}

void* PthreadPool::pthreadFunc(void* args){
	pthread_t pid=pthread_self();
	while(true){
		pthread_mutex_lock(&myMutex);
		//vector<Task*>* taskList=static_cast<vector<Task*>*>(args);
		pthread_cond_wait(&myCond,&myMutex);
		vector<Task*>* taskList=&myTaskList;
		vector<Task*>::iterator taskIter=taskList->begin();
		if(taskIter!=taskList->end()){
			moveBuzyPthreads(pid);
			Task* task=*taskIter;
			taskList->erase(taskIter);
			cout<<"total:"<<ablePthreads.size()+buzyPthreads.size()<<",thread "<<pid<<" run:";
			task->run();
			moveAblePthreads(pid);
		}
		pthread_mutex_unlock(&myMutex);
	}
}

PthreadPool::PthreadPool(int n){
	maxPthreadNum=2*n;
	create(n);

}

int PthreadPool::submit(Task* task){
	pthread_mutex_lock(&myMutex);
	myTaskList.push_back(task);
	if (ablePthreads.size()==0 && (buzyPthreads.size()<maxPthreadNum)){
		create(1);
	}
	else if (buzyPthreads.size()>=maxPthreadNum) while (ablePthreads.size()==0);
	pthread_mutex_unlock(&myMutex);
	pthread_cond_signal(&myCond);
	return 0;
}

int PthreadPool::wait(){
	vector<pthread_t>::iterator iter=ablePthreads.begin();
	while(iter!=ablePthreads.end()){
		pthread_cancel(*iter);
		//pthread_join(*iter,NULL);
		++iter;
	}
	iter=buzyPthreads.begin();
	while(iter!=buzyPthreads.end()){
		pthread_cancel(*iter);
		//pthread_join(*iter,NULL);
		++iter;
	}
	return 0;
}

