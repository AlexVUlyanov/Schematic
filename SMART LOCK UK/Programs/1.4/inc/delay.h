
////////////////////////////////

#define F_CPU_16�

////////////////////////////////

#ifdef F_CPU_16�

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x*4-2)     
#define pdelay_us(usd)  asm("PUSH CC\n" "PUSHW X\n" "LDW X, #" STR(usd) "\n" "Lable:\n" "NOP\n" "DECW X\n"  "JRNE Lable\n" "POPW X\n" "POP CC\n")
#define CLK_Div CLK_SYSCLKDiv_1

#endif

#ifdef F_CPU_8�

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x*2-1)    
#define pdelay_us(usd)  asm("LDW X, #" STR(usd) "\n" "Lable:\n" "NOP\n" "DECW X\n"  "JRNE Lable\n")
#define CLK_Div CLK_SYSCLKDiv_2

#endif

#ifdef F_CPU_4�

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)    
#define pdelay_us(usd)  asm("LDW X, #" STR(usd) "\n" "Lable:\n" "DECW X\n"  "JRNE Lable\n")
#define CLK_Div CLK_SYSCLKDiv_4
  
#endif


void delay_ms(uint16_t n_ms) ; // ������������ ������ TIM4
void SomeDelay(); // 0.5 ��� ��� 16���
void delay_10us(uint16_t n_10us); //������������ ������ TIM4 ������� 25���
 