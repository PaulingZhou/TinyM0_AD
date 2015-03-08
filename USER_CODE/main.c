		/****************************************Copyright (c)****************************************************
		**                            Guangzhou ZHIYUAN electronics Co.,LTD.
		**
		**                                 http://www.embedtools.com
		**
		**--------------File Info---------------------------------------------------------------------------------
		** File name:           main.c
		** Last modified Date:  2010-02-04
		** Last Version:        V1.0
		** Descriptions:        The main() function example template
		**
		**--------------------------------------------------------------------------------------------------------
		** Created by:          Lanwuqiang
		** Created date:        2010-02-05
		** Version:             V1.0
		** Descriptions:        ����û�Ӧ�ó���
		**
		**--------------------------------------------------------------------------------------------------------
		** Modified by:         ZhangNingbo
		** Modified date:       2010-02-25
		** Version:             V1.0
		** Descriptions:        AD����ʾ������
		**
		**--------------------------------------------------------------------------------------------------------
		** Modified by:        
		** Modified date:      
		** Version:            
		** Descriptions:       
		**
		** Rechecked by:
		*********************************************************************************************************/
		#include "..\config.h"

		/*********************************************************************************************************
		�궨��
		*********************************************************************************************************/
		#define UART_BPS    115200                                              /*  ����ͨ�Ų�����              */
		char    GcRcvBuf2[20],GcRcvBuf3[20];                                                   /*  AD�ɼ���������              */

		/*********************************************************************************************************
		** Function name:       myDelay
		** Descriptions:        �����ʱ
		** input parameters:    ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void myDelay (INT32U ulTime)
		{
		INT32U i;

		i = 0;
		while (ulTime--) {
		   for (i = 0; i < 5000; i++);
		}
		}
		/*********************************************************************************************************
		** Function name:       ADCInit
		** Descriptions:        ADC��ʼ��
		** input parameters:    ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void ADCInit ( void )
		{

		PDRUNCFG      &= ~(0x01 << 4);                                      /*  ADCģ���ϵ�                  */
		SYSAHBCLKCTRL |=  (0x01 << 13);                                     /*  ʹ��ADCģ��ʱ��              */

		// IOCON_PIO0_11 &= ~0xBF;                                             /*  ����PIO0_11Ϊģ������ģʽ    */
		// IOCON_PIO0_11 |=  0x02;                                             /*  PIO0_11ģ������ͨ��0         */
		// IOCON_PIO1_0 &= ~0xBF;
		// IOCON_PIO1_0 |= 0x02;
		IOCON_PIO1_1 &= ~0xBF;
		IOCON_PIO1_1 |= 0x02;				//PIO1_1ģ������ͨ��2
		IOCON_PIO1_2 &= ~0xBF;
		IOCON_PIO1_2 |= 0x02;				//PIO1_2ģ������ͨ��3

		AD0CR = ( 0x0C << 0 ) |                                             /*  SEL=12,ѡ��ADC2��3��1100��               */
				((FAHBCLK / 1000000 - 1 ) << 8) |                           /*  ת��ʱ��1MHz                 */
				( 1 << 16 ) |                                               /*  BURST=1,ʹ��Burstģʽ        */
				( 0 << 17 ) |                                               /*  ʹ��11 clocksת��            */
				( 0 << 24 ) |                                               /*  ADCת��ֹͣ                  */
				( 0 << 27 );                                                /*  ֱ������ADCת������λ��Ч    */

		}
		/*********************************************************************************************************
		** Function name:       uartInit
		** Descriptions:        ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������Ϊ115200
		** input parameters:    ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void uartInit (void)
		{
		INT16U usFdiv;

		IOCON_PIO1_6  &= ~0x07;
		IOCON_PIO1_6  |= (1 << 0);                                          /*  ����P1.6ΪRXD               */
		IOCON_PIO1_7  &= ~0x07;
		IOCON_PIO1_7  |= (1 << 0);                                          /*  ����P1.7ΪTXD               */
		SYSAHBCLKCTRL |= (1 << 12);                                         /*  ��UART���ܲ���ʱ��        */
		UARTCLKDIV     = 0x01;                                              /*  UARTʱ�ӷ�Ƶ                */

		U0LCR  = 0x83;                                                      /*  �������ò�����              */
		usFdiv = (FAHBCLK / UARTCLKDIV / 16) / UART_BPS;                    /*  ���ò�����                  */
		U0DLM  = usFdiv / 256;
		U0DLL  = usFdiv % 256; 
		U0LCR  = 0x03;                                                      /*  ����������                  */
		U0FCR  = 0x07;
		}

		/*********************************************************************************************************
		** Function name:       uartSendByte
		** Descriptions:        �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
		** input parameters:    ucDat:   Ҫ���͵�����
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void uartSendByte (INT8U ucDat)
		{
		U0THR = ucDat;                                                      /*  д������                    */
		while ((U0LSR & 0x40) == 0){                                        /*  �ȴ����ݷ������            */
		}
		}

		/*********************************************************************************************************
		** Function name:       uartSendStr
		** Descriptions:        �򴮿ڷ����ַ���
		** input parameters:    puiStr:   Ҫ���͵��ַ���ָ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void uartSendStr (INT8U *pucStr)
		{
		while (1){
			if (*pucStr == '\0') {
				break;                                                      /*  �������������˳�            */
			}
			uartSendByte (*pucStr++);
		}
		}

		/*********************************************************************************************************
		** Function name:       pcDispChar
		** Descriptions:        ��PC��������ʾ�ַ�
		** input parameters:    x:        ��ʾ�ַ��ĺ�����
		**                      y:        ��ʾ�ַ���������
		**                      ucChr:    ��ʾ���ַ�������Ϊff
		**                      ucColor:  ��ʾ��״̬������ǰ��ɫ������ɫ����˸λ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void pcDispChar (INT8U x, INT8U y, INT8U ucChr, INT8U ucColor)
		{
		uartSendByte(0xff);                                                 /*  ��ʼ�ַ�                    */
		uartSendByte(x);
		uartSendByte(y);
		uartSendByte(ucChr);
		uartSendByte(ucColor);
		}

		/*********************************************************************************************************
		** Function name:       iSendStr
		** Descriptions:        ����λ�������ַ���
		** input parameters:    x:        ��ʾ�ַ��ĺ�����
		**                      y:        ��ʾ�ַ���������
		**                      ucColor:  ��ʾ��״̬������ǰ��ɫ������ɫ����˸λ��
		**                                ��DOS�ַ���ʾһ����0��3,ǰ��ɫ��4��6������ɫ��7����˸λ
		**                      pcStr:   Ҫ���͵��ַ�������'\0'����
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		void iSendStr (INT8U x, INT8U y, INT8U ucColor, char *pcStr)
		{
		while (1) {
			if (*pcStr == '\0') {                                           /*  �����ַ�                    */
				break;
			}
			pcDispChar(x++, y, *pcStr++, ucColor);
			if (x >= 80) {
				x = 0;
				y++;
			}
		}
		}
		/*********************************************************************************************************
		** Function name:       main
		** Descriptions:        AD�ɼ��������̣��轫JP8�е�VIN��P0.11�̽ӣ�JP3�е�TXD0��P1.7��RXD��P1.6�̽ӡ�
		**                      ͬʱ��PC���Ĵ��������ӵ�������UART0����Easyarm���ڵ���������۲���������
		**                      ע�⣺����AD�ο���ѹ��3.3V����ֱ�Ӳ���P0.11�ɼ���ѹ����ѹ���ø���3.3V��
		** input parameters:    ��
		** output parameters:   ��
		** Returned value:      ��
		*********************************************************************************************************/
		int main (void)
		{
		INT32U  i;
		INT32U  ulADCData2,ulADCData3; 
		INT32U  ulADCBuf2,ulADCBuf3;

		targetInit();                                                       /*  ��ʼ��Ŀ��壬����ɾ��      */

		uartInit();                                                         /*  ���ڳ�ʼ��                  */
		ADCInit();                                                          /*  ADCģ���ʼ��               */
		while (1) {
			ulADCData2 = 0;
			ulADCData3 = 0;
			
			// for(i = 0; i < 10; i++) {
				while((AD0DR2 & 0x80000000) == 0){
				}
				ulADCBuf2 = AD0DR2;                                          /*  ��ȡ����Ĵ���              */
				ulADCBuf2 = (ulADCBuf2 >> 6) & 0x3ff;
				myDelay(50);
				// ulADCData1 += ulADCBuf1;
				while((AD0DR3 & 0x80000000) == 0){                          /*  ��ȡAD0DR1��Done            */
				}
				ulADCBuf3 = AD0DR3;                                          /*  ��ȡ����Ĵ���              */
				ulADCBuf3 = (ulADCBuf3 >> 6) & 0x3ff;
				// ulADCData2 += ulADCBuf2;
			// }
			// ulADCData1 = ulADCBuf1 / 10;                                     /*  ����10�ν����ǲ�����        */
			// while((AD0DR0 & 0x80000000)==0){
				// ulADCData1 = (AD0DR0>>6) & 0x3FF;
			ulADCData2 = (ulADCBuf2 * 3300) / 1024;
			ulADCData3 = (ulADCBuf3 * 3300) /1024;
			sprintf(GcRcvBuf2,"VIN2= %4d mv",ulADCData2);
			sprintf(GcRcvBuf3,"VIN3 = %4d mv",ulADCData3);/*  �����ݷ��͵����ڽ�����ʾ    */
			iSendStr(0, 0, 0x30, GcRcvBuf2);	
			iSendStr(0,1,0x30,GcRcvBuf3);	
			// }
			myDelay(50);
		}
		}

		/*********************************************************************************************************
		End Of File
		*********************************************************************************************************/
