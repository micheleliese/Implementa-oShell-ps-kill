#include "syscall.h"
#include <string.h>

void DoSystemCall(unsigned int *stack,Parameters *arg)
{ 
  Descriptors[TaskRunning].SP=stack;
  MoveToSP(&KernelStack[289]);
#ifdef SHARED_NUMBER
  TTYshared(1);
#endif
  switch(arg->CallNumber)
  {
    case TASKCREATE:
      sys_taskcreate((int *)arg->p0,(void(*)())arg->p1);
      break;
    case SEM_WAIT:
      sys_semwait((sem_t *)arg->p0);
      break;
    case SEM_POST:
      sys_sempost((sem_t *)arg->p0);
      break;
    case SEM_INIT:
      sys_seminit((sem_t *)arg->p0,(int)arg->p1);
      break;
    case WRITELCDN:
      LCDcomando((int)arg->p1);
      LCDnum((int)arg->p0);
      break;
    case WRITELCDS:
      LCDcomando((int)arg->p1);
      LCDputs((char*)arg->p0);
      break;
    case EXITTASK:
      sys_taskexit();
      break;
    case SLEEP:
      sys_sleep((int)arg->p0);
      break;
    case DSLEEP:
        sys_dsleep((int)arg->p0); // nova chamada de sistema
    break;
    case MSLEEP:
      sys_msleep((int)arg->p0);
      break;
    case USLEEP:
      sys_usleep((int)arg->p0);
      break;
    case LIGALED:
      sys_ligaled((int)arg->p0);
      break;
    case START:
      sys_start((int)arg->p0);
      break;
    case TASKJOIN:
      sys_taskjoin((int)arg->p0);
      break;
    case SETMYNAME:
      sys_setmyname((const char *)arg->p0);
      break;
    case NKPRINT:
       sys_nkprint((char *)arg->p0,(void *)arg->p1);
        break;
    case GETMYNUMBER:
        sys_getmynumber((int *)arg->p0);
        break;
    case NKREAD:
        sys_nkread((char *)arg->p0,(void *)arg->p1); // 28/02/2015
        break;
    default:
       break;
  }
  RestoreContext(Descriptors[TaskRunning].SP);
}

void sys_taskcreate(int *ID,void (*task)())
{
  NumberTaskAdd++;
  *ID=NumberTaskAdd;
  Descriptors[NumberTaskAdd].Tid=*ID;
  Descriptors[NumberTaskAdd].EP=task;
  Descriptors[NumberTaskAdd].State=INITIAL;
  Descriptors[NumberTaskAdd].Join=0;
  Descriptors[NumberTaskAdd].Time=0;
  Descriptors[NumberTaskAdd].Prio=0;
  Descriptors[NumberTaskAdd].SP=&Descriptors[NumberTaskAdd].Stack[SizeTaskStack-1];
  return;
}

void sys_semwait(sem_t *semaphore)
{
    semaphore->count--;
    if(semaphore->count < 0)
    {
	    semaphore->sem_queue[semaphore->tail] = TaskRunning;
	    Descriptors[TaskRunning].State = BLOCKED ;
	    semaphore->tail++;
	    if(semaphore->tail == MaxNumberTask-1) semaphore->tail = 0;
	      Dispatcher();
    }
}

void sys_sempost(sem_t *semaphore)
{
    semaphore->count++;
    if(semaphore->count <= 0)
    {
      Descriptors[semaphore->sem_queue[semaphore->header]].State = READY;
      InsertReadyList(semaphore->sem_queue[semaphore->header]);
      semaphore->header++;
      if(semaphore->header == MaxNumberTask-1) semaphore->header = 0;
    }
}

void sys_seminit(sem_t *semaphore, int value)
{
    semaphore->count = value;
    semaphore->header = 0;
    semaphore->tail = 0;
}

void sys_taskexit(void)
{
  int i;
  Descriptors[TaskRunning].State=TERMINATED;
  for(i=1;i<=NumberTaskAdd;i++)
  {
    if(TaskRunning==Descriptors[i].Join)
    {
      Descriptors[i].Join=0;
      Descriptors[i].State=READY;
      InsertReadyList(i);
    }
  }
  Dispatcher();
}


void sys_sleep(unsigned int segundo)
{
  Descriptors[TaskRunning].Time = segundo/ClkT;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    Dispatcher();
  }
}

//===============================================================================
void sys_dsleep(unsigned int deci) // Nova chamada de sistema
{
  Descriptors[TaskRunning].Time = (deci/ClkT)/10;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    Dispatcher();
  }
}
//===============================================================================

void sys_msleep(unsigned int mili)
{
  Descriptors[TaskRunning].Time = (mili/ClkT)/1000;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    Dispatcher();
  }
}

void sys_usleep(unsigned int micro)
{
  Descriptors[TaskRunning].Time = (micro/ClkT)/1000000;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    Dispatcher();
  }
}

void sys_ligaled(int value)
{
  FIO4PIN=~value;
}

void sys_start(int scheduler)
{
  int i=2;
  SchedulerAlgorithm=scheduler;
  Descriptors[1].State=BLOCKED;
  switch (SchedulerAlgorithm)
  {
    case RR:
      for(i=2;i<=NumberTaskAdd;i++)
	     InsertReadyList(i);
      break;
    default:
      break;
  }
#ifdef DEBUG_START
  TTYstart();
#endif
  Dispatcher();
}

void sys_taskjoin(int id)
{
  if(Descriptors[id].State!=TERMINATED)
  {
    Descriptors[TaskRunning].State=BLOCKED;
    Descriptors[TaskRunning].Join=id;
    Dispatcher();
  }
}

void sys_setmyname(const char *name)
{
  Descriptors[TaskRunning].name=name;
}

void sys_getmynumber(int *number)
{
  *number=Descriptors[TaskRunning].Tid;
}

void sys_nkread(char *tipo, void *value)// endereço da variavel que recebe o dado solicitado será armazenado em serial_queue[posicao_a_inserir].value
{
  serial_fila[posicao_a_inserir] = TaskRunning;
  serial_queue[posicao_a_inserir].tid=TaskRunning;
	     if(!strcmp(tipo,"%f"))//28/02/15 
	   serial_queue[posicao_a_inserir].real=(void *)value;// value é o endereço da variável que recebe o dado solicitado na app
	else if(!strcmp(tipo,"%.1f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%.2f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%.3f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%.4f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%.5f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%.6f"))
        serial_queue[posicao_a_inserir].real=(void *)value;
	else if(!strcmp(tipo,"%s"))
        serial_queue[posicao_a_inserir].string=(void *)value;
	else if(!strcmp(tipo,"%c"))//07/03/2015
        serial_queue[posicao_a_inserir].caracter=(void *)value;
	else if(!strcmp(tipo,"%d"))
        serial_queue[posicao_a_inserir].inteiro=(void *)value;
  Descriptors[TaskRunning].State=BLOCKED;
  //printk("task %d está bloqueada aguardando dado\n",TaskRunning);
  posicao_a_inserir++;  	
  if(posicao_a_inserir == MaxNumberTask) posicao_a_inserir = 0;
    //printk("[posicao_a_inserir] = %d \n", posicao_a_inserir);
  Dispatcher();
}
