#include "stm8l15x_conf.h"
#include <stdio.h>
#include "KT_mfrc522.h"
#include "delay.h"

#define RX_BUFFER_SIZE 7

char rx_buffer[RX_BUFFER_SIZE];
unsigned int rx_wr_index;

uint8_t WORK = FALSE;

uint8_t CardID[5];
char szBuff[100];
uint8_t CLOSE;

uint8_t Watch[6];
  
RTC_InitTypeDef   RTC_InitStr;
RTC_TimeTypeDef   RTC_TimeStr;
RTC_DateTypeDef   RTC_DateStr;


void ON_LEDA (void) // Транзистор открыт
{
  GPIO_SetBits (GPIOD,GPIO_Pin_6);
}

void OFF_LEDA (void) // Транзистор закрыт
{
  GPIO_ResetBits (GPIOD,GPIO_Pin_6);
}

void ON_LEDB (void) // Сигнализирует об открытом транзисторе
{
  GPIO_SetBits (GPIOD,GPIO_Pin_7);
}

void OFF_LEDB (void) // Сигнализирует об закрытом транзисторе
{
  GPIO_ResetBits (GPIOD,GPIO_Pin_7);
}

void ON_PB3 (void) // Сигнализирует об типе Управления 
{
   GPIO_SetBits (GPIOB,GPIO_Pin_3);
}

void OFF_PB3 (void) // Сигнализирует об типе Управления 
{
  GPIO_ResetBits (GPIOB,GPIO_Pin_3);
}

void UART_SendByte(uint8_t uData) {
	USART_SendData8(USART1,uData);
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE)== RESET);     
}

void UART_SendStr(char *pData) {
	while(*pData != '\0') {
          UART_SendByte(*pData++);                     
    }
	UART_SendByte((uint8_t)0x0d);
	UART_SendByte((uint8_t)0x0d);
	UART_SendByte((uint8_t)0x0a);
}

u8 chek_rfid_1 (uchar *CardID) // Проверка доверительной карты 1
{
  uint8_t kDATA[5];
  kDATA[0] = 0x34;
  kDATA[1] = 0x1D;
  kDATA[2] = 0x02;
  kDATA[3] = 0xEA;
  kDATA[4] = 0xC1;
  
  if(CardID[0] == kDATA[0] && CardID[1] == kDATA[1] && CardID[2] == kDATA[2]&& CardID[3] == kDATA[3]&& CardID[4] == kDATA[4])
    {
        return 1;
                      } 
      
      else
           return 0;
}

u8 chek_rfid_2 (uchar *CardID) // Проверка доверительной карты 2
{
  uint8_t kDATA[5];
  kDATA[0] = 0x61;
  kDATA[1] = 0xDD;
  kDATA[2] = 0xB8;
  kDATA[3] = 0x45;
  kDATA[4] = 0x41;
  
  if(CardID[0] == kDATA[0] && CardID[1] == kDATA[1] && CardID[2] == kDATA[2]&& CardID[3] == kDATA[3]&& CardID[4] == kDATA[4])
  {
        return 1;
              } 
      
      else
           return 0;
}

void INIT (void) // Инициализация
{
  GPIO_Init (GPIOD,GPIO_Pin_2,GPIO_Mode_Out_PP_Low_Fast);
  GPIO_WriteBit(GPIOD,GPIO_Pin_2,RESET);
   
   
  CLK_SYSCLKSourceConfig (CLK_SYSCLKSource_HSI); // Тактирование от внутреннего 16 МГц
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // Предделитель частоты 1
  CLK_ClockSecuritySystemEnable(); // защита тактирования от смены такктового генератора
  
  CLK_LSEConfig(CLK_LSE_ON);
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);
     
   /* Select LSE (32.768 KHz) as RTC clock source */
   CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);
   CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1,ENABLE);
  
  
  ////////////////////////////SPI//////////////////////////////////////////
   GPIO_Init (GPIOB,GPIO_Pin_7,GPIO_Mode_In_PU_No_IT); // PB7 - MISO - input MASTER
   GPIO_Init (GPIOB,GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Fast); // PB6 - MOSI - out Master
   GPIO_Init (GPIOB,GPIO_Pin_5,GPIO_Mode_Out_PP_Low_Fast); // PB5 - SCK - out Master 
  ////////////////////////////////////////////////////////////////////////
   
   GPIO_Init (GPIOD,GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Fast); // out Transistor
   GPIO_Init (GPIOD,GPIO_Pin_7,GPIO_Mode_Out_PP_Low_Fast); // STATUS
   GPIO_Init (GPIOB,GPIO_Pin_3,GPIO_Mode_Out_PP_Low_Fast); // CONTROL
  
  USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx));
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  USART_Cmd (USART1,ENABLE); 
  
  MFRC522_Init();
  
  sprintf(szBuff, "WORK_OK");
  UART_SendStr(szBuff);  
  
  OFF_LEDA ();
}

void Calendar_Init(void)

{
RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
RTC_InitStr.RTC_AsynchPrediv = 0x7F;
RTC_InitStr.RTC_SynchPrediv = 0x00FF;
RTC_Init(&RTC_InitStr);
 
   RTC_DateStructInit(&RTC_DateStr);
   RTC_DateStr.RTC_WeekDay = RTC_Weekday_Tuesday;
   RTC_DateStr.RTC_Date = 0x30;
   RTC_DateStr.RTC_Month = RTC_Month_January;
   RTC_DateStr.RTC_Year =0x18; // 2018 год
   RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);
 
   RTC_TimeStructInit(&RTC_TimeStr);
   RTC_TimeStr.RTC_Hours=0x0C;
   RTC_TimeStr.RTC_Minutes=0x1C;
   RTC_TimeStr.RTC_Seconds=00;
   RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);

}

void Time_Show(void)
{
  /* Wait until the calendar is synchronized */
  while (RTC_WaitForSynchro() != SUCCESS);
  
  /* Get the current Time*/
   RTC_GetTime(RTC_Format_BCD, &RTC_TimeStr);
 Watch[0] = RTC_TimeStr.RTC_Hours;
 Watch[1] = RTC_TimeStr.RTC_Minutes; 
 Watch[2] = RTC_TimeStr.RTC_Seconds;
 Watch[3] = RTC_DateStr.RTC_Date;
 Watch[4] = RTC_DateStr.RTC_Month;
 Watch[5] = RTC_DateStr.RTC_Year;
 sprintf(szBuff, "%02X:%02X:%02X/%02X.%02X.%02X", Watch[0],Watch[1], Watch[2],Watch[3],Watch[4],Watch[5]);
	      UART_SendStr(szBuff);
}

void FSM_LOCK_SYSTEM (void) {
typedef enum {StInit,COMPEAR_1,COMPEAR_2,OPEN,CLOSE,SEND} State_Type;
static State_Type  state = StInit;
switch(state){
           case StInit:
            if (MFRC522_Request(PICC_REQIDL,CardID) == MI_OK ) { 
            if (MFRC522_Anticoll(CardID) == MI_OK) {
              sprintf(szBuff, "SERIAL : %02X %02X %02X %02X %02X", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
	      UART_SendStr(szBuff);             
              state = COMPEAR_1;    
              break;
            case COMPEAR_1:
                        if (chek_rfid_1(CardID) == 1)
                        {
                         state = OPEN;
                         sprintf(szBuff, "Ulianov A.V.");
	                 UART_SendStr(szBuff);                                            
                         Time_Show();
                          break;
                        }
                          else 
                            if (chek_rfid_2(CardID) == 1)
                        {
                         state = OPEN;
                         sprintf(szBuff, "Kopitov S.M.");
	                 UART_SendStr(szBuff);                                
                         Time_Show();
                          break;
                        }
                        else
                             state = CLOSE;
                        break;
            case OPEN :
                         OFF_LEDA();
                         //ON_LEDA();
                         ON_LEDB();
                         sprintf(szBuff, "OPEN");
	                  UART_SendStr(szBuff);                      
                         for (unsigned long i=0; i <600000;i++)
                         {
                           SomeDelay();
                         }
                         OFF_LEDA();
                         OFF_LEDB();
                        sprintf(szBuff, "CLOSE");
	                UART_SendStr(szBuff);                                            
                         state = StInit;
                         break;
               case CLOSE :
                          ON_LEDA();
                         //OFF_LEDA();
                         OFF_LEDB();
                         for (unsigned long i=0; i <600000;i++)
                         {
                           SomeDelay();
                         }
                        sprintf(szBuff, "CLOSE");
	                UART_SendStr(szBuff);  
                        Time_Show();                                            
                         state = StInit;
                         break;
            }
            }
}
}
void PC_CONTROL (void)
{
  if (rx_buffer[0]==0x44) // D -DATA SET
  {
 ON_PB3();
 RTC_TimeStr.RTC_Hours   = rx_buffer[1];
 RTC_TimeStr.RTC_Minutes = rx_buffer[2];
 RTC_TimeStr.RTC_Seconds = rx_buffer[3];
 RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
   
 RTC_DateStr.RTC_Date = rx_buffer[4];
 RTC_DateStr.RTC_Month = rx_buffer[5];
 RTC_DateStr.RTC_Year = rx_buffer[6];
 RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);
 OFF_LEDB ();
  }
       if ((rx_buffer[0]==0x41)&&(rx_buffer[1]==0x4F)) // A O - Acces Open
  {
           OFF_LEDA();
            //ON_LEDA();    // Открыть замоk
           ON_LEDB();  
            
                         sprintf(szBuff, "OPEN");
	                  UART_SendStr(szBuff);
                           Time_Show();
                           
                         /*for (u8 i=0; i<300000;i++)
                         {
                           SomeDelay();
                         }
                        OFF_LEDA(); 
                         
                       sprintf(szBuff, "CLOSE");
	               UART_SendStr(szBuff);
           */
                      rx_buffer[0] = 0;
                      rx_buffer[1] = 0;
                       
                          
     }
  if ((rx_buffer[0]==0x41)&&(rx_buffer[1]==0x43)) // A C Acces CLOSE
  {
                         ON_LEDA();
                          //OFF_LEDA(); // Закрыть замок
                         OFF_LEDB();
                         
                         sprintf(szBuff, "CLOSE");
	                 UART_SendStr(szBuff);  
                         Time_Show();
                          
                          rx_buffer[0] = 0;
                          rx_buffer[1] = 0;
                          //OFF_LEDB();                     
       }

  if ((rx_buffer[0]==0x53)&&(rx_buffer[1]==0x4E)) // SN- RIAD RFID SN
  { 
           //ON_LEDB();
            ON_PB3();
           if (MFRC522_Request(PICC_REQIDL,CardID) == MI_OK ) { 
            if (MFRC522_Anticoll(CardID) == MI_OK) {
              sprintf(szBuff, "SERIAL : %02X %02X %02X %02X %02X", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
	      UART_SendStr(szBuff);              
                          rx_buffer[0] = 0;
                          rx_buffer[1] = 0;
                          //OFF_LEDB();
                          OFF_PB3();
            }
           }                                                    
       }
  
      if ((rx_buffer[0]==0x53)&&(rx_buffer[1]==0x54)) // S T Show Time
      {
       Time_Show();
       
  rx_buffer[0] = 0;
  rx_buffer[1] = 0;
      }        
         
}



void main( void )

{
WORK = FALSE ;
INIT ();   
Calendar_Init();   
enableInterrupts();

ON_LEDA();

while (1)
    
  {  
  if (rx_buffer[0]== 0x57) // W
  {
    WORK = FALSE ;
  }
  
  if (WORK ==FALSE)
  {
   FSM_LOCK_SYSTEM();
    ON_PB3();
  }
  
 if (rx_buffer[0]== 0x50)
 { 
   WORK = TRUE ;
 }
 
 if (WORK ==TRUE)
  {
   PC_CONTROL();
   OFF_PB3();
  }

}

}


INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
  rx_buffer [rx_wr_index++] = USART_ReceiveData8 (USART1);
  
 if ( rx_wr_index == RX_BUFFER_SIZE)
  {
  rx_wr_index = 0;
  }
          
  }
   
