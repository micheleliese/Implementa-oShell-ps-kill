#ifndef __sysCALL_H
#define __sysCALL_H

#include <arch/nxp/lpc23xx.h>
#include "scheduler.h"
#include "../Placa/lcd.h"
#include "kernel.h"

enum sys_temCall{
  TASKCREATE,
  SEM_WAIT,
  SEM_POST,
  SEM_INIT,
  WRITELCDN,
  WRITELCDS,
  EXITTASK,
  SLEEP,
  MSLEEP,
  USLEEP,
  LIGALED,
  START, 
  TASKJOIN,
  SETMYNAME,
  NKPRINT,
  GETMYNUMBER,
  NKREAD,
  DSLEEP, // nova chamada de sistema
};

enum TaskEstados{
  INITIAL,
  READY,
  RUNNING,
  TERMINATED,
  BLOCKED
};

void DoSystemCall(unsigned int *stack,Parameters *arg);
void sys_taskcreate(int *ID,void (*task)());
void sys_semwait(sem_t *semaphore);
void sys_sempost(sem_t *semaphore);
void sys_seminit(sem_t *semaphore, int value);
void sys_taskexit(void);
void sys_sleep(unsigned int segundo);
void sys_dsleep(unsigned int deci);// nova chamada de sistema
void sys_msleep(unsigned int mili);
void sys_usleep(unsigned int micro);
void sys_ligaled(int value);
void sys_start(int scheduler);
void sys_taskjoin(int id);
void sys_setmyname(const char *name);
void sys_nkprint(char *fmt,void *number);
void sys_getmynumber(int *number);
void sys_nkread(char *tipo,void *value);


#endif
