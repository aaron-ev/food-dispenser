
/**
 ******************************************************************************
 * @file         msp.h
 * @author       Aaron Escoboza
 * @brief        Header for msp.c
 ******************************************************************************
 */

#ifndef __MSP__H
#define __MSP__H

void mspEnableBuzzerIT(void);
void mspEnableButtonInterrupts(void);
void mspDisableButtonInterrupts(void);

#endif
