#ifndef __INITKERNEL_H
#define __INITKERNEL_H

#include "kernel.h"
#include "../Placa/interrupt.h"
#include "../Placa/init.h"
#include "../Placa/lcd.h"

void StartKernel(void);
void StartReadyList(void);
void ini_serial_fila(void);
void CreateIdleAndMainAndShell(void);
void shell(void);

#endif
