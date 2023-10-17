/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2020 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashPrg.c:  Flash Programming Functions adapted for               */
/*               ST Microelectronics STM32h7B3I/B0-Disco Flash         */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures
#include "main.h"
#include "quadspi.h"
#include "qspi_w25q64.h"
#include "gpio.h"



 uint8_t  Verify_Data[8*1024]; //У��ʱ���������,����С��һ�� Programming Page Size �Ĵ�С ����8k


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) 
{
  SystemInit(); 				// ϵͳ��ʼ��
	HAL_Init();
  SystemClock_Config();	// ����ϵͳʱ�ӣ���Ƶ480MHz
	MX_GPIO_Init();
	MX_QUADSPI_Init();

	if ( QSPI_W25Qxx_Init() != QSPI_W25Qxx_OK )  // ��ʼ��W25Q64
	{
		return 1;
	}
	
	return 0;
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int UnInit (unsigned long fnc) 
{ 
	if ( QSPI_W25Qxx_Init() != QSPI_W25Qxx_OK )  // ��ʼ��W25Q64
	{
		return 1;
	}
	return 0;
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseChip (void) 
{	
	if ( QSPI_W25Qxx_ChipErase() != QSPI_W25Qxx_OK )  // ��Ƭ����
	{
		return 1;
	}
	
	return 0;  
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) 
{	
	adr = adr - W25Qxx_Mem_Addr; 
	
	if ( QSPI_W25Qxx_BlockErase_64K(adr) != QSPI_W25Qxx_OK )  // ������Ӧ����
	{
		return 1;
	}
	
	return 0;  	
}


/*
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat)
{
	return (1); /* Always Force Erase */
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) 
{

	QSPI_W25Qxx_WriteBuffer(buf, adr -W25Qxx_Mem_Addr,sz);	

	return (0);                      
}

/*  
 *  Verify Flash Contents
 *    Parameter:      adr:  Start Address
 *                    sz:   Size (in bytes)
 *                    buf:  Data
 *    Return Value:   (adr+sz) - OK, Failed Address
 */

/*
   ��adr��ַ��ȡ��szָ���Ĵ�С��ȵ����ݵ���Ϊ Verify_Data �Ļ������� 
   ѭ������ buf �������е�ÿ���ֽڣ��������� Verify_Data �������е���Ӧ�ֽڽ��бȽϡ�
 */
unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf)
{
	unsigned long i;

	QSPI_W25Qxx_ReadBuffer(Verify_Data, adr - W25Qxx_Mem_Addr, sz);	

	for (i = 0; i < sz; i++)
	{
		if (Verify_Data[i] != buf[i])
		{
			return (adr + i); // Verification Failed (return address)
		}
	}
	
	return (adr + sz); // Done successfully
}
