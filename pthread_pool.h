#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__
#include <vector>
#include <pthread.h>
#include <string>
#include <future>
using namespace std;
class Task{
		protected:
			string name;
			int data;
		public:
			Task(){}
			Task(int d):data(d){
				name="Task";
			}
			Task(string n,int d):name(n),data(d){}
			virtual void run(){};
};

class PthreadPool{
		private:
				static vector<Task*> myTaskList;
				int maxPthreadNum;
				static vector<pthread_t> ablePthreads;
				static vector<pthread_t> buzyPthreads;
				static pthread_mutex_t myMutex;
				static pthread_cond_t myCond;
				static bool flag;
		public:
				PthreadPool(int n);
				int submit(Task* task);
				int wait();
				
		protected:
				void create(int n);
				static void* pthreadFunc(void* args);
				static int moveBuzyPthreads(pthread_t pid);
				static int moveAblePthreads(pthread_t pid);
};
#endif
