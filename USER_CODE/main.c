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
** Descriptions:        添加用户应用程序
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         ZhangNingbo
** Modified date:       2010-02-25
** Version:             V1.0
** Descriptions:        AD采样示例程序
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
  宏定义
*********************************************************************************************************/
#define UART_BPS    115200                                              /*  串口通信波特率              */
char    GcRcvBuf[20];                                                   /*  AD采集到的数据              */

/*********************************************************************************************************
** Function name:       myDelay
** Descriptions:        软件延时
** input parameters:    无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        ADC初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ADCInit ( void )
{

    PDRUNCFG      &= ~(0x01 << 4);                                      /*  ADC模块上电                  */
    SYSAHBCLKCTRL |=  (0x01 << 13);                                     /*  使能ADC模块时钟              */

    IOCON_PIO0_11 &= ~0xBF;                                             /*  配置PIO0_11为模拟输入模式    */
    IOCON_PIO0_11 |=  0x02;                                             /*  PIO0_11模拟输入通道0         */

    AD0CR = ( 0x01 << 0 ) |                                             /*  SEL=1,选择ADC0               */
            ((FAHBCLK / 1000000 - 1 ) << 8) |                           /*  转换时钟1MHz                 */
            ( 1 << 16 ) |                                               /*  BURST=1,使用Burst模式        */
            ( 0 << 17 ) |                                               /*  使用11 clocks转换            */
            ( 0 << 24 ) |                                               /*  ADC转换停止                  */
            ( 0 << 27 );                                                /*  直接启动ADC转换，此位无效    */

}
/*********************************************************************************************************
** Function name:       uartInit
** Descriptions:        串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为115200
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartInit (void)
{
    INT16U usFdiv;

    IOCON_PIO1_6  &= ~0x07;
    IOCON_PIO1_6  |= (1 << 0);                                          /*  配置P1.6为RXD               */
    IOCON_PIO1_7  &= ~0x07;
    IOCON_PIO1_7  |= (1 << 0);                                          /*  配置P1.7为TXD               */
    SYSAHBCLKCTRL |= (1 << 12);                                         /*  打开UART功能部件时钟        */
    UARTCLKDIV     = 0x01;                                              /*  UART时钟分频                */

    U0LCR  = 0x83;                                                      /*  允许设置波特率              */
    usFdiv = (FAHBCLK / UARTCLKDIV / 16) / UART_BPS;                    /*  设置波特率                  */
    U0DLM  = usFdiv / 256;
    U0DLL  = usFdiv % 256; 
    U0LCR  = 0x03;                                                      /*  锁定波特率                  */
    U0FCR  = 0x07;
}

/*********************************************************************************************************
** Function name:       uartSendByte
** Descriptions:        向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    ucDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartSendByte (INT8U ucDat)
{
    U0THR = ucDat;                                                      /*  写入数据                    */
    while ((U0LSR & 0x40) == 0){                                        /*  等待数据发送完毕            */
	}
}

/*********************************************************************************************************
** Function name:       uartSendStr
** Descriptions:        向串口发送字符串
** input parameters:    puiStr:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uartSendStr (INT8U *pucStr)
{
    while (1){
        if (*pucStr == '\0') {
			break;                                                      /*  遇到结束符，退出            */
        }
        uartSendByte (*pucStr++);
    }
}

/*********************************************************************************************************
** Function name:       pcDispChar
** Descriptions:        向PC机发送显示字符
** input parameters:    x:        显示字符的横坐标
**                      y:        显示字符的纵坐标
**                      ucChr:    显示的字符，不能为ff
**                      ucColor:  显示的状态，包括前景色、背景色、闪烁位。
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void pcDispChar (INT8U x, INT8U y, INT8U ucChr, INT8U ucColor)
{
    uartSendByte(0xff);                                                 /*  起始字符                    */
    uartSendByte(x);
    uartSendByte(y);
    uartSendByte(ucChr);
    uartSendByte(ucColor);
}

/*********************************************************************************************************
** Function name:       iSendStr
** Descriptions:        向上位机发送字符串
** input parameters:    x:        显示字符的横坐标
**                      y:        显示字符的纵坐标
**                      ucColor:  显示的状态，包括前景色、背景色、闪烁位。
**                                与DOS字符显示一样：0～3,前景色，4～6，背景色，7，闪烁位
**                      pcStr:   要发送的字符串，以'\0'结束
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void iSendStr (INT8U x, INT8U y, INT8U ucColor, char *pcStr)
{
    while (1) {
        if (*pcStr == '\0') {                                           /*  结束字符                    */
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
** Descriptions:        AD采集数据例程，需将JP8中的VIN与P0.11短接，JP3中的TXD0与P1.7，RXD与P1.6短接。
**                      同时将PC机的串口线连接到开发板UART0，打开Easyarm串口调试软件，观察采样结果。
**                      注意：由于AD参考电压是3.3V，若直接采用P0.11采集电压，电压不得高于3.3V。
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int main (void)
{
    INT32U  i;
    INT32U  ulADCData; 
    INT32U  ulADCBuf;

    targetInit();                                                       /*  初始化目标板，切勿删除      */

    uartInit();                                                         /*  串口初始化                  */
    ADCInit();                                                          /*  ADC模块初始化               */
    while (1) {
        ulADCData = 0;
        for(i = 0; i < 10; i++) {
            while((AD0DR0 & 0x80000000) == 0){                          /*  读取AD0DR0的Done            */
			}
            ulADCBuf = AD0DR0;                                          /*  读取结果寄存器              */
            ulADCBuf = (ulADCBuf >> 6) & 0x3ff;
            ulADCData += ulADCBuf;
        }
        ulADCData = ulADCData / 10;                                     /*  采样10次进行虑波处理        */
        ulADCData = (ulADCData * 3300) / 1024;
        sprintf(GcRcvBuf,"VIN0 = %4d mv",ulADCData);                      /*  将数据发送到串口进行显示    */
        iSendStr(0, 0, 0x30, GcRcvBuf);                                 /*  将数据发送到串口显示        */
        myDelay(50);
    }
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
