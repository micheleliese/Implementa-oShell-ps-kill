#include "initkernel.h"

void StartKernel(void)
{
  SaidaLed();
  LCDinit();
  U0init();
  ini_serial_fila();
  RTC_CCR=0; RTC_CCR=2; RTC_CCR=0;
  RTC_CCR=0x11;
//  TCPLowLevelInit();
  StartReadyList();
  CreateIdleAndMainAndShell();
  InitTimer0();
//  drv_accept(5432);
  MoveToSP(&Descriptors[1].Stack[SizeTaskStack-1]);
  IRQTimer0(); //liga o kernel
}

void CreateIdleAndMainAndShell(void)
{
  static int TidIdle= 0, TidMain= 1, TidShell= 2; 
  /*Criaçao da Idle Task */
  Descriptors[0].Tid= TidIdle;
  Descriptors[0].EP= idletask;
  Descriptors[0].State= INITIAL;
  Descriptors[0].Join= 0;
  Descriptors[0].SP= &Descriptors[0].Stack[SizeTaskStack-1];
  Descriptors[0].Prio= 0;
  Descriptors[0].name= "idle";
  /*Main*/
  Descriptors[1].Tid= TidMain;
  Descriptors[1].EP= (void(*)())main;
  Descriptors[1].State= INITIAL;
  Descriptors[1].Join= 0;
  Descriptors[1].SP= &Descriptors[1].Stack[SizeTaskStack-1];
  Descriptors[1].Prio= 0;
  Descriptors[1].name= "main";
  /*Shell*/
  Descriptors[2].Tid= TidShell;
  Descriptors[2].EP= shell;
  Descriptors[2].State=INITIAL;
  Descriptors[2].Join=0;
  Descriptors[2].SP=&Descriptors[2].Stack[SizeTaskStack-1];
  Descriptors[2].Prio= 0;
  Descriptors[2].name= "shell";
  TaskRunning= 1;
  NumberTaskAdd= 2;
}

void StartReadyList(void)
{
  int i;
  for(i=0;i<MaxNumberTask-1;i++)
	  ready_queue.queue[i]=0;
  ready_queue.head=0;
  ready_queue.tail=0;
  return;
}

void ini_serial_fila(void)
{
   int i;
	for(i=0; i<MaxNumberTask; i++)
	  serial_fila[i] = 0;
}

void shell(void)
{
  while (1)
  {
    int i = 0, Id = 0;
    char comando[5];
    printk("\n>>: ", 0);
    nkread("%s", &comando);
    
    if(strcmp(comando, "ps") == 0){
      for (i = 0; i <=NumberTaskAdd; i++){
        if(Descriptors[i].State != TERMINATED){
          printk("\nID: %d", Descriptors[i].Tid);
          printk("\tNome: %s \n", Descriptors[i].name);
        }
      }
    }else if(strcmp(comando, "kill") == 0){
      printk("\nDigite o ID da task que deseja matar: ", 0);
      nkread("%d", &Id);
      if(Id > 1 && Id <= NumberTaskAdd){
        if(Descriptors[Id].State != TERMINATED){
          Descriptors[Id].State = TERMINATED;
          printk("\n\nStatus da Task [%d]: TERMINATED = [ %d ]\n", Descriptors[Id].Tid, Descriptors[Id].State);
          printk("Nome da task assassinada: %s :(\n", Descriptors[Id].name);
        }else{
          printk("\nTarefa de Id: [%d] INVALIDA\n\a", Id);
        }
      }else{
        printk("\nId : [%d] , Invalido\n\a", Id);
      }
    }else{
      printk("\n\nComando [%s] nao contem nesta implementacao\n\n", &comando);
    }
  }
}

