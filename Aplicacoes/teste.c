#include "../Kernel/kernel.h"

void task1()
{
    setmyname("task1");
    while (1){

    }
    taskexit();
}

void task2()
{
    setmyname("task2");
    while(1){
 
    }
    taskexit();
}

int main(int argc, char *argv[])
{
  int t1,t2;
  taskcreate(&t1,task1);
  taskcreate(&t2,task2);
  start(RR);
  return 0;
}