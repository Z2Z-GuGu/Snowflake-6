
#include "debug.h"
#include "ch32v003_touch.h"



#define ADD_N_NOPS( n ) asm volatile( ".rept " #n "\nc.nop\n.endr" );

uint8_t tim_flag = 0;

uint8_t uart_data = 0x55;
uint8_t uart_data_buf[10] = {0};
uint8_t uart_flag = 0;

void SF_GPIO_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void TIM1_INT_Init( u16 arr, u16 psc)
{

    NVIC_InitTypeDef NVIC_InitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );

    NVIC_InitStructure.NVIC_IRQChannel =TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}

void TIM2_INT_Init( u16 arr, u16 psc)
{

    NVIC_InitTypeDef NVIC_InitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    // USART1 TX-->D.5   RX-->D.6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // only uart rx irq
    // NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    // only uart rx dma
    DMA_Cmd(DMA1_Channel5, ENABLE); // USART1 Rx

    USART_Cmd(USART1, ENABLE);
}

void start_one_DMA()
{
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel5,10);
    DMA_Cmd(DMA1_Channel5, ENABLE);
}

void DMA_INIT(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart_data_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 10;

    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
}

void ADC_IO_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    // ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t Get_ADC_Val(uint8_t ch)
{
    uint16_t val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);

    return val;
}

#define io_send_1() GPIOD->BSHR=GPIO_Pin_5
#define io_send_0() GPIOD->BCR=GPIO_Pin_5
// ws2812 ctrl
void ws2812_send_bit(uint8_t data_bit)
{
    if(data_bit == 0){
        // ADD_N_NOPS(2);
        io_send_0();
        ADD_N_NOPS(40);
        io_send_1();
    }else{
        ADD_N_NOPS(20);
        io_send_0();
        ADD_N_NOPS(12);
        io_send_1();
    }
}

void TIM1_Dead_Time_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_BDTRInitTypeDef     TIM_BDTRInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM1, ENABLE);

    /* TIM1_CH1N */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_6, 1);
    Delay_Ms(10);
    GPIO_WriteBit(GPIOC, GPIO_Pin_6, 0);
    Delay_Ms(10);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

        // while(!(TIM1->INTFR & TIM_IT_Update)){}
        // TIM1->CH1CVR = 44;
        // TIM1->INTFR &= 0xFFFE;
        // while(!(TIM1->INTFR & TIM_IT_Update)){}
        // TIM1->CH1CVR = 16;
        // TIM1->INTFR &= 0xFFFE;

void ws2812_send_Byte(uint8_t ws_byte)
{
    uint8_t i;
    for(i = 0; i < 8; i ++){
        while(!(TIM1->INTFR & TIM_IT_Update)){}
        if (ws_byte & 0x80) {
            TIM1->CH1CVR = 44;
        } else {
            TIM1->CH1CVR = 16;
        }
        TIM1->INTFR &= 0xFFFE;
        ws_byte <<= 1;
    }
}

#define LED_NUM         30
#define PIXEL_SIZE      3

//  {0x00, 0x00, 0x00},
//  { H  ,  S  ,  V  }
uint8_t ws2812_hsv_data[LED_NUM][PIXEL_SIZE] = {
/*              I                   B                   L                   T                   R        */
/* 0 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 1 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 2 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 3 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 4 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 5 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
};

//  {0x00, 0x00, 0x00},
//  { G  ,  R  ,  B  }
uint8_t ws2812_sample_data[LED_NUM][PIXEL_SIZE] = {
/*              I                   B                   L                   T                   R        */
/* 0 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 1 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 2 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 3 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 4 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
/* 5 */ {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
};

// uint8_t ws2812_sample_data[LED_NUM][PIXEL_SIZE] = {
// /*              I                   B                   L                   T                   R        */
// /* 0 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// /* 1 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// /* 2 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// /* 3 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// /* 4 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// /* 5 */ {0x40, 0xff, 0x40}, {0x00, 0x00, 0x00}, {0x48, 0xe0, 0x80}, {0x80, 0xc0, 0xff}, {0x48, 0xe0, 0x80},
// };

void ws2812_send_data(uint8_t *p_wsdata, uint16_t data_len)
{
    uint16_t i;
    uint8_t *p_data = p_wsdata;


    GPIOC->BCR=GPIO_Pin_6;
    GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
    // Delay_Us(300);
    __disable_irq();
    TIM1->CHCTLR1 &= ~TIM_OCPreload_Enable;  // DIS OC1Preload
    TIM1->CH1CVR = 0;          // start with 0%PWM
    TIM1->CHCTLR1 |= TIM_OCPreload_Enable;  // EN OC1Preload
    TIM1->CNT = 0x0001;
    TIM1->CTLR1 |= TIM_CEN;     // start tim1
    GPIOC->CFGLR |= 0x08000000; // to TIMCC1 

    for(i = 0; i < data_len; i++){
        ws2812_send_Byte(*p_data);
        p_data++;
    }

    while(!(TIM1->INTFR & TIM_IT_Update)){}
    // ADD_N_NOPS(7);
    TIM1->INTFR &= 0xFFFE;
    TIM1->CTLR1 &= ~TIM_CEN;    // stop tim1
    TIM1->CH1CVR = 0;          // start with 0%PWM
    TIM1->CNT = 0x0001;
    GPIOC->BCR=GPIO_Pin_6;
    // GPIOC->BSHR=GPIO_Pin_6;
    GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
    __enable_irq();
    // Delay_Us(300);
}

// #include <stdint.h>

void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t *p_save) {
    uint8_t r, g, b;

    if (s == 0) {
        // Achromatic (grey)
        r = g = b = v;
    } else {
        uint8_t region = h / 43; // 0-5 之间的区间
        uint8_t remainder = (h - (region * 43)) * 6; // (0-42) 转换为 (0-255)
        uint8_t p = (v * (255 - s)) >> 8; // v * (1 - s)
        uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8; // v * (1 - s * remainder)
        uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8; // v * (1 - s * (255 - remainder))

        switch (region) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
        }
    }

    p_save[0] = r;
    p_save[1] = g;
    p_save[2] = b;
}

#define Value_MAX 100
#define Value_MIN 30
#define Saturation_MAX 200
#define Saturation_MIN 130
#define Hue_MAX 255
#define Hue_MIN 0
uint8_t Periodic_State = 0;
uint8_t Saturation = 130;   // 饱和度
uint8_t Value      = 0;     // 明度
uint8_t Hue        = 0;     // 色相
uint8_t Sat_Breath_State = 1;      // 
uint8_t Val_Breath_State = 0;      // 


uint8_t sin_val[7] = {0, 62, 120, 169, 207, 231, 240};  // 240*sin(15*i)

int8_t sind(int8_t angle)
{
    if(angle < 0 || angle > 24) return 0;
    if(angle >= 0 || angle <= 6) return sin_val[angle];
    if(angle > 6 || angle <= 12) return sin_val[12-angle];
    if(angle > 12 || angle <= 18) return -sin_val[angle-12];
    if(angle > 18 || angle <= 24) return -sin_val[24-angle];
}

int8_t cosd(int8_t angle)
{
    if(angle < 0 || angle > 24) return 0;
    if(angle >= 0 || angle <= 6) return sin_val[6-angle];
    if(angle > 6 || angle <= 12) return -sin_val[angle-6];
    if(angle > 12 || angle <= 18) return -sin_val[18-angle];
    if(angle > 18 || angle <= 24) return sin_val[angle-18];
}

typedef struct {
    int8_t angle;
    int8_t distance;
    int8_t x_value;
    int8_t y_value;
    int8_t direction;
} led_position_t;

// typedef struct {
//     int8_t h_value;
//     int8_t s_value;
//     int8_t v_value;
// } led_color_t;

led_position_t led_position[LED_NUM] = {0};
// led_color_t led_color[LED_NUM] = {0};

void led_position_init(void)
{
    int i;
    int temp16;
    for(i = 0; i < 6; i++){
        // angle
        led_position[i*5+0].angle = 22-4*i;
        led_position[i*5+1].angle = 22-4*i;
        led_position[i*5+2].angle = 23-4*i;
        led_position[i*5+3].angle = 22-4*i;
        led_position[i*5+4].angle = 21-4*i;
        // distance
        led_position[i*5+0].distance = 10;
        led_position[i*5+1].distance = 20;
        led_position[i*5+2].distance = 24;
        led_position[i*5+3].distance = 30;
        led_position[i*5+4].distance = 24;
        // direction
        led_position[i*5+0].direction = 1;
        led_position[i*5+1].direction = 0;
        led_position[i*5+2].direction = 1;
        led_position[i*5+3].direction = 1;
        led_position[i*5+4].direction = 1;
    }
    for(i = 0; i < LED_NUM; i++){
        temp16 = led_position[i].distance*cosd(led_position[i].angle)/240;
        led_position[i].x_value = temp16;
        temp16 = led_position[i].distance*sind(led_position[i].angle)/240;
        led_position[i].y_value = temp16;
    }
}

#define render_info_element_MAX 4

typedef struct {
    int8_t exist;
    int8_t type;    // 0:point(max=3) 1:ring(max=1)
    int8_t x_value;
    int8_t y_value;
    int8_t h_value;
    int8_t s_value;
    int8_t v_value;
} render_info_element_t;

render_info_element_t render_info_element[render_info_element_MAX] = {0};

void render_info_element_init(void)
{
    render_info_element[0].exist   =    1;
    render_info_element[0].type    =    0;
    render_info_element[0].x_value =    0;
    render_info_element[0].y_value =    0;
    render_info_element[0].h_value =    180;
    render_info_element[0].s_value =    100;
    render_info_element[0].v_value =    100;

    render_info_element[1].exist   =    0;
    render_info_element[1].type    =    0;
    render_info_element[1].x_value =    0;
    render_info_element[1].y_value =    0;
    render_info_element[1].h_value =    0;
    render_info_element[1].s_value =    0;
    render_info_element[1].v_value =    0;

    render_info_element[2].exist   =    0;
    render_info_element[2].type    =    0;
    render_info_element[2].x_value =    0;
    render_info_element[2].y_value =    0;
    render_info_element[2].h_value =    0;
    render_info_element[2].s_value =    0;
    render_info_element[2].v_value =    0;

    render_info_element[3].exist   =    1;
    render_info_element[3].type    =    1;
    render_info_element[3].x_value =    40;
    render_info_element[3].y_value =    40;
    render_info_element[3].h_value =    10;
    render_info_element[3].s_value =    100;
    render_info_element[3].v_value =    10;
}

void pixel_render_element(void)
{
    int i, j;
    int32_t W_param[4] = {0};
    int32_t temp32;
    for(i = 0; i < LED_NUM; i++){
        for(j = 0; j < 3; j++){
            if(render_info_element[j].exist){
                temp32 = led_position[i].x_value - render_info_element[j].x_value;
                W_param[j] = temp32*temp32;
                temp32 = led_position[i].y_value - render_info_element[j].y_value;
                W_param[j] += temp32*temp32;
            } else {
                W_param[j] = 0;
            }
        }
        temp32 = render_info_element[3].x_value - led_position[i].distance;
        W_param[3] = temp32*temp32;
        ws2812_hsv_data[i][0] = render_info_element[0].h_value;
        ws2812_hsv_data[i][1] = ((W_param[0]*render_info_element[0].s_value)+\
                                 (W_param[1]*render_info_element[1].s_value)+\
                                 (W_param[2]*render_info_element[2].s_value)+\
                                 (W_param[3]*render_info_element[3].s_value))/\
                                (W_param[0]+W_param[1]+W_param[2]+W_param[3]);
        ws2812_hsv_data[i][2] = ((W_param[0]*render_info_element[0].v_value)+\
                                 (W_param[1]*render_info_element[1].v_value)+\
                                 (W_param[2]*render_info_element[2].v_value)+\
                                 (W_param[3]*render_info_element[3].v_value))/\
                                (W_param[0]+W_param[1]+W_param[2]+W_param[3]);
    }
}

void pixels_hsv_to_RGB(void)
{
    uint8_t i = 0;
    for(i = 0; i < LED_NUM; i++){
        hsv_to_rgb( ws2812_hsv_data[i][0], \
                    ws2812_hsv_data[i][1], \
                    ws2812_hsv_data[i][2], \
                    &ws2812_sample_data[i][0]);
    }
}



int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    // ADC_IO_Init();
    SF_GPIO_INIT();
//    TIM1_INT_Init(250-1, 48000-1);      // 250ms
//    TIM_Cmd( TIM1, ENABLE );
    TIM2_INT_Init(16-1, 48000-1);       // 16ms
    TIM_Cmd( TIM2, ENABLE );
    DMA_INIT();
    USARTx_CFG();
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    // Delay_Ms(1000);

   
    // TIM1_Dead_Time_Init(58, 0, 16);
    TIM1_Dead_Time_Init(58, 0, 0);
    while(1)
    {
        // test0:delay+gpiowrite
        // Delay_Ms(250);
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 0);
        // Delay_Ms(250);
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1);

        // tesy1:delay+gpiowrite+read
        // Delay_Ms(250);
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0));

        // test2 tim1/2+gpiowrite
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, tim_flag);

        // test3 TIM IRQ time:3.7us
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)); // 1.2/1.3us
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 0); // 0.5us
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1); // 0.8us

        // test4 nop time
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 0); // 0.5us
        // ADD_N_NOPS(100);                     // 4.14us
        // GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1); // 0.8us
        // ADD_N_NOPS(200);                     // 8.28us

        // test5 uart send
        // USART_SendData(USART1, 0x55);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        // {
        //     // waiting for sending finish
        // }
        // Delay_Ms(250);

        // test6 uart r+s        
        // while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        // {
        //     // waiting for receiving finish
        // }
        // uart_data = (USART_ReceiveData(USART1));
        // USART_SendData(USART1, uart_data);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        // {
        //     // waiting for sending finish 
        // }

        // test7 uart interrupt
        // if(uart_flag == 1){
        //     USART_SendData(USART1, uart_data);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        //     {
        //        // waiting for sending finish
        //     }
        //     uart_flag = 0;
        // } 
        // // 外边必须有东西，不然按目前的优化程度直接被优化了，不被执行
        // Delay_Ms(1);

        // test8 uart DMA
        // while(DMA_GetFlagStatus(DMA1_FLAG_TC5) == RESET) // Wait until USART1 RX DMA1 Transfer Complete
        // {
        //     // USART_SendData(USART1, 0x55);
        //     // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // }
        // DMA_ClearFlag(DMA1_FLAG_TC5);
        // USART_SendData(USART1, uart_data_buf[0]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[1]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[2]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[3]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[4]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[5]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[6]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[7]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[8]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // USART_SendData(USART1, uart_data_buf[9]);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}

        // start_one_DMA();

        // test9 uart dma irq
        // if(uart_flag == 1){
        //     uart_flag = 0;
        //     DMA_ClearFlag(DMA1_FLAG_TC5);

        //     USART_SendData(USART1, uart_data_buf[0]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[1]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[2]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[3]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[4]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[5]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[6]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[7]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[8]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        //     USART_SendData(USART1, uart_data_buf[9]);
        //     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}

        //     start_one_DMA();
        // }
        // Delay_Ms(1);

        // test10 ADC read
        // printf("====================================\r\n");
        // printf("adc0 data = %d\r\n", Get_ADC_Val(ADC_Channel_0));
        // printf("adc1 data = %d\r\n", Get_ADC_Val(ADC_Channel_1));
        // printf("adc2 data = %d\r\n", Get_ADC_Val(ADC_Channel_2));
        // printf("adc3 data = %d\r\n", Get_ADC_Val(ADC_Channel_3));
        // printf("adc4 data = %d\r\n", Get_ADC_Val(ADC_Channel_4));
        // printf("adc7 data = %d\r\n", Get_ADC_Val(ADC_Channel_7));
        // Delay_Ms(1000);

        // test11 ws2812
        // int i = 0;
        // uint8_t ws2812_data = 0x55;
        // ws2812_send_bit(ws2812_data&0x01);
        // ws2812_send_bit(ws2812_data&0x02);
        // ws2812_send_bit(ws2812_data&0x04);
        // ws2812_send_bit(ws2812_data&0x08);
        // ws2812_send_bit(ws2812_data&0x10);
        // ws2812_send_bit(ws2812_data&0x20);
        // ws2812_send_bit(ws2812_data&0x40);
        // ws2812_send_bit(ws2812_data&0x80);
        // Delay_Ms(1);

        // test12 PWM
        // USART_SendData(USART1, TIM1->CNT);
        // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
        // Delay_Ms(1);
        // while(TIM1->CNT < 50){}

        // test13 PWM chg
        // while(!(TIM1->INTFR & TIM_IT_Update)){}
        // TIM1->CH1CVR = 44;
        // TIM1->INTFR &= 0xFFFE;
        // while(!(TIM1->INTFR & TIM_IT_Update)){}
        // TIM1->CH1CVR = 16;
        // TIM1->INTFR &= 0xFFFE;

        // test14 gpio out <-> af
        // GPIOC->BSHR=GPIO_Pin_6;     // 1
        // GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
        // Delay_Ms(1);
        // GPIOC->CFGLR |= 0x08000000; // to TIMCC1
        // Delay_Ms(1);
        // GPIOC->BCR=GPIO_Pin_6;      // 0
        // GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
        // Delay_Ms(1);
        // GPIOC->CFGLR |= 0x08000000; // to TIMCC1
        // Delay_Ms(1);

        // test15 ws2812 send
        // ws2812_send_Byte(0x0F);

        // test16 ws send start&stop
        // GPIOC->BCR=GPIO_Pin_6;
        // GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
        // Delay_Us(300);
        // TIM1->CHCTLR1 &= ~TIM_OCPreload_Enable;  // DIS OC1Preload
        // TIM1->CH1CVR = 0;          // start with 0%PWM
        // TIM1->CHCTLR1 |= TIM_OCPreload_Enable;  // EN OC1Preload
        // TIM1->CNT = 0x0001;
        // TIM1->CTLR1 |= TIM_CEN;     // start tim1
        // GPIOC->CFGLR |= 0x08000000; // to TIMCC1
        // ws2812_send_Byte(0x0F);
        // while(!(TIM1->INTFR & TIM_IT_Update)){}
        // // ADD_N_NOPS(7);
        // TIM1->INTFR &= 0xFFFE;
        // TIM1->CTLR1 &= ~TIM_CEN;    // stop tim1
        // TIM1->CH1CVR = 0;          // start with 0%PWM
        // TIM1->CNT = 0x0001;
        // GPIOC->BCR=GPIO_Pin_6;
        // // GPIOC->BSHR=GPIO_Pin_6;
        // GPIOC->CFGLR &= 0xF7FFFFFF; // to GPIO
        // Delay_Us(300);

        // test17 ws2812 send data
        // GPIOC->BSHR=GPIO_Pin_0;     // 1
        // ws2812_send_data(ws2812_sample_data, LED_NUM*PIXEL_SIZE);
        // GPIOC->BCR=GPIO_Pin_0;      // 0

        // test18 hsv to rgb time   // 30~2.35us
//        if(Periodic_State == 1){
//            // 亮度呼吸
//            if(Value >= Value_MAX) Val_Breath_State = 0;
//            else if(Value <= Value_MIN) Val_Breath_State = 1;
//            if(Val_Breath_State) Value += 1;
//            else Value -= 1;
//            // 饱和呼吸
//            if(Saturation >= Saturation_MAX) Sat_Breath_State = 0;
//            else if(Saturation <= Saturation_MIN) Sat_Breath_State = 1;
//            if(Sat_Breath_State) Saturation += 1;
//            else Saturation -= 1;
//            // 色相轮转
//            if(Hue == Hue_MAX) Hue = Hue_MIN;
//            else Hue++;
//            uint8_t i = 0;
//            for(i = 0; i < LED_NUM; i++){
//                // if((i+0)%5 == 0) // only innerlight
//                // if((i+1)%5 == 0) // only rightlight
//                // if((i+2)%5 == 0) // only toplight
//                // if((i+3)%5 == 0) // only leftlight
//                if((i+4)%5 == 0) // only backlight
//                // if((i+3)%5 == 0 || (i+1)%5 == 0 || (i+0)%5 == 0 || (i+2)%5 == 0)
//                    hsv_to_rgb(Hue+(8*i), Saturation, Value, &ws2812_sample_data[i][0]);
//                else
//                    hsv_to_rgb(0, 0, 0, &ws2812_sample_data[i][0]);
//            }
//            ws2812_send_data(ws2812_sample_data, LED_NUM*PIXEL_SIZE);
//            // Delay_Us(1000);
//            Periodic_State = 0;
//        }
//         Delay_Us(1);

        // test19 运算时间
        // float  *  = 1.7us
        // float  /  = 2.0us
        // uint32 *  = 41ns
        // uint32 /  = 41ns

        // uint32_t i;
        // uint16_t a,b,c;
        // GPIOC->BSHR=GPIO_Pin_0;     // 1
        // for(i = 0; i < 1000; i++){
        //     c+=a+b;
        // }
        // GPIOC->BCR=GPIO_Pin_0;      // 0
        // printf("c = %d", c);

         // test20 rander
        if(Periodic_State == 1){
          GPIO_WriteBit(GPIOC, GPIO_Pin_1, 1);
          pixel_render_element();
          pixels_hsv_to_RGB();
          ws2812_send_data(ws2812_sample_data, LED_NUM*PIXEL_SIZE);
          GPIO_WriteBit(GPIOC, GPIO_Pin_1, 0);
          Periodic_State = 0;
        }
        Delay_Us(1);
    }
}

// void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
// void TIM1_UP_IRQHandler(void)
// {
//     if(TIM_GetITStatus(TIM1, TIM_IT_Update)==SET)
//     {
//         tim_flag = !tim_flag;
//     }
//     TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
// }

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查TIM2中断是否发生。
    {
        // tim_flag = !tim_flag;
         GPIO_WriteBit(GPIOC, GPIO_Pin_1, !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1));
        Periodic_State = 1;
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);      //清除TIM2的中断挂起位。
    }
}

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uart_data = USART_ReceiveData(USART1);
        uart_flag = 1;
    }
}

void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void)
{
    if( DMA_GetITStatus(DMA1_IT_TC5) != RESET )
    {
        uart_flag = 1;
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
}

