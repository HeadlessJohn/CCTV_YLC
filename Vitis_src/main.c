#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "sleep.h"
#include "xinterrupt_wrap.h"
#include "xil_exception.h"
#include "xuartlite.h"

#define STEP_MOTOR_BASEADDR  XPAR_STEP_MOTOR_0_BASEADDR
#define ADC_BASEADDR         XPAR_XSYSMON_0_BASEADDR
#define ULTRA_SONIC_BASEADDR XPAR_MYIP_ULTRA_SONIC_0_BASEADDR
#define INTC_BASEADDR        XPAR_MICROBLAZE_0_AXI_INTC_BASEADDR
#define BT_UART1_BASEADDR     XPAR_AXI_UARTLITE_1_BASEADDR
#define UART0_BASEADDR        XPAR_XUARTLITE_0_BASEADDR

#define POSITIVE_EDGE 1
#define NEGATIVE_EDGE -1

// define recv buffer size
#define BUFFER_SIZE        100

// ADC
#define ADC_DATA             (*adc_4)

// Ultra sonic
#define DISTANCE             (*ultra_sonic)
#define DETECT_DISTANCE 20    //cm

// Step motor
#define STEP_TIME       4000
#define DEG_RIGHT_90    3072
#define DEG_CENTER      2048
#define DEG_LEFT_90     1024

#define SEARCHING_TIME  7000 // 7s 마다 반대로 회전

// Step motor struct
typedef struct {
    volatile int target_deg; // 0~4095
    volatile u32 fast_mode;  // 0:8step, 1:4step
    volatile u32 set_center; // set current deg as 2048
    volatile u32 step_time;  // hold time per step
    volatile u32 motor_on;   // motor on/off
    volatile u32 cur_deg;    // current deg
}Step;

// Base address 
volatile Step * step_motor            = (Step*) STEP_MOTOR_BASEADDR;
volatile unsigned int * adc_4         = (volatile unsigned int *) (ADC_BASEADDR +0x27c);
volatile unsigned int * ultra_sonic   = (volatile unsigned int *)  ULTRA_SONIC_BASEADDR;
UINTPTR uart0_baseaddr                = (UINTPTR) UART0_BASEADDR;
UINTPTR bt_uart1_baseaddr             = (UINTPTR) BT_UART1_BASEADDR;
UINTPTR intc_baseaddr                 = (UINTPTR) INTC_BASEADDR;

XIntc intc;
XUartLite bt_uart1;
XUartLite uart0;

void init_step_motor();
void init_timer_interrupt();
void timer_ms();
void ultrasonic_searching(u8 searching_enable);
void step_motor_rot(u8 on_off);

void uart0_send_handler();
void uart0_recv_handler();
void bt_uart1_send_handler();
void bt_uart1_recv_handler();
void init_uart0();
void init_uart1();
void bt_recv_command();
void joystick_control(u8 manual_control_enable, u8 _tick_1ms);
int edge_detector(u8 current_clk_source, u8 index);

volatile u32 cnt_ms = 0;
volatile u8 detected_flag = 0; 
volatile u8 timer_enable = 0;
volatile u8 bt_recv_done_flag = 0;
volatile u8 searching_off_flag = 0;
volatile u8 tick_1ms = 0;
volatile u8 motor_rot_flag = 0;
// buffer
u8 Uart0_ReceiveBuffer   [BUFFER_SIZE];
u8 BT_Uart1_ReceiveBuffer[BUFFER_SIZE];

int main(){   
    init_platform();
    init_uart0();
    init_timer_interrupt();
    init_uart1();
    init_step_motor();
    
    print("Initializing procedure complete\n\r");

/////////////////////////////////////////////////////////////////////////////////////////////////
    while(1){
        joystick_control(searching_off_flag, tick_1ms);
        bt_recv_command();                                    // 블루투스 커맨드
        ultrasonic_searching(1);                              // 1넣으면 감시
        step_motor_rot( detected_flag + searching_off_flag ); // 1넣으면 멈춤
    }
/////////////////////////////////////////////////////////////////////////////////////////////////

    cleanup_platform();
    return 0;
}

void timer_ms(){
    if (timer_enable){
        cnt_ms++;
    }
    else {
        cnt_ms = 0;
    }
    tick_1ms ^= 0b00000001;
}

void init_step_motor(){
    usleep(10);
    //SET CENTER
    step_motor->target_deg = DEG_CENTER;
    usleep(10);    
    step_motor->set_center = TRUE;
    usleep(10);
    step_motor->set_center = FALSE;

    step_motor->fast_mode  = FALSE;      // SET FAST MODE 
    step_motor->step_time  = STEP_TIME; // SET STEP TIME
    step_motor->motor_on   = 1;
    usleep(10);
    // print("step motor initialized\n center deg at 2048\n");
}

void init_timer_interrupt(){

    XIntc_Initialize(&intc, intc_baseaddr);
    XIntc_Connect(&intc, 0, timer_ms, (void *)0);
	XIntc_Start(&intc, XIN_REAL_MODE);
	XIntc_Enable(&intc, 0);
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);
	Xil_ExceptionEnable();
    XIntc_SimulateIntr(&intc, 0);

    // XIntc_Initialize (&intc, intc_baseaddr);
    // XIntc_Connect(&intc, 0, timer_ms, (void*)intc_baseaddr);
    // XIntc_Enable(&intc, 0);
    // XIntc_Start(&intc, XIN_REAL_MODE);

    // Xil_ExceptionInit();
    // Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);
    // Xil_ExceptionEnable();
}

void ultrasonic_searching(u8 searching_enable){
    static u8 prev_detected_flag = 0;

    if (searching_enable == 1) {
        if (DISTANCE <= DETECT_DISTANCE) {
            detected_flag = 1;
        }
        else {
            detected_flag = 0;
        }
    }

    if (( prev_detected_flag == 0) && (detected_flag == 1) ) {
        XUartLite_Send(&bt_uart1, "!!ENEMY DETECTED!!\n\r", 19);
    }

    prev_detected_flag = detected_flag;
}

void step_motor_rot(u8 on_off){
    static u8 prev_motor_rot_flag = 0;

    if (on_off == 0){
        motor_rot_flag = 1;
        timer_enable = 1;
        step_motor->motor_on = 1;
        if (cnt_ms < SEARCHING_TIME){
            step_motor->target_deg = DEG_LEFT_90;
        }
        else if(cnt_ms < 2*SEARCHING_TIME){
            step_motor->target_deg = DEG_RIGHT_90;
        }
        else {
            timer_enable = 0;
        }
    }
    else {
        motor_rot_flag = 0;
    }
    // if (edge_detector(motor_rot_flag, 0) == NEGATIVE_EDGE){
    //     step_motor->motor_on = 0; // 모터를 끄고
    //     step_motor->target_deg = step_motor->cur_deg; // 현재각도에서 멈춤
    // }
    if (( prev_motor_rot_flag == 1) && (motor_rot_flag == 0) ) {
        step_motor->motor_on = 0; // 모터를 끄고
        step_motor->target_deg = step_motor->cur_deg; // 현재각도에서 멈춤
    }

    prev_motor_rot_flag = motor_rot_flag;
}



void init_uart0(){
    //init uart0
    XUartLite_Config *Uart0_CfgPtr;
	Uart0_CfgPtr = XUartLite_LookupConfig(uart0_baseaddr);
	XUartLite_Initialize(&uart0, uart0_baseaddr);
	XSetupInterruptSystem(&uart0, &XUartLite_InterruptHandler, Uart0_CfgPtr->IntrId, Uart0_CfgPtr->IntrParent, XINTERRUPT_DEFAULT_PRIORITY);
	XUartLite_SetSendHandler(&uart0, uart0_send_handler, &uart0);
	XUartLite_SetRecvHandler(&uart0, uart0_recv_handler, &uart0);
	XUartLite_EnableInterrupt(&uart0);
    return;
}

void init_uart1(){
    //init bt_uart1
    XUartLite_Config *BT_Uart1_CfgPtr;
	BT_Uart1_CfgPtr = XUartLite_LookupConfig(bt_uart1_baseaddr);
	XUartLite_Initialize(&bt_uart1, bt_uart1_baseaddr);
	XSetupInterruptSystem(&bt_uart1, &XUartLite_InterruptHandler, BT_Uart1_CfgPtr->IntrId, BT_Uart1_CfgPtr->IntrParent, XINTERRUPT_DEFAULT_PRIORITY);
	XUartLite_SetSendHandler(&bt_uart1, bt_uart1_send_handler, &bt_uart1);
	XUartLite_SetRecvHandler(&bt_uart1, bt_uart1_recv_handler, &bt_uart1);
	XUartLite_EnableInterrupt(&bt_uart1);
    return;
}

void uart0_send_handler(){
    return;
}


void uart0_recv_handler(){   
    static u16 recv_cnt = 0;
    u8 data;
    XUartLite_Recv(&uart0, &data, 1);
    if ( data == ('\n') | data == ('\r') ){
        Uart0_ReceiveBuffer[recv_cnt] = '\0'; // make end of string
        XUartLite_Send(&uart0, Uart0_ReceiveBuffer, recv_cnt);
        recv_cnt = 0;
    }
    else {
        Uart0_ReceiveBuffer[recv_cnt++] = data;
    }
    return;
}

void bt_uart1_send_handler(){
	return;
}

        // XUartLite_Send(&bt_uart1, BT_Uart1_ReceiveBuffer, recv_cnt);
        // XUartLite_Send(&uart0, BT_Uart1_ReceiveBuffer, recv_cnt);

void bt_uart1_recv_handler(){   
    static u16 recv_cnt = 0;              //받는 데이터 숫자 카운트를 위한 변수
    u8 data;                              //받은 데이터 1문자를 담는 임시 변수
    XUartLite_Recv(&bt_uart1, &data, 1);
    if ( data == ('\n') | data == ('\r') ){
        BT_Uart1_ReceiveBuffer[recv_cnt] = '\0'; // make end of string
        recv_cnt = 0;
        bt_recv_done_flag = 1;
    }
    else {
        BT_Uart1_ReceiveBuffer[recv_cnt++] = data;
    }
    return;
}

void bt_recv_command(){
    if (bt_recv_done_flag == 1) {
        bt_recv_done_flag = 0;
        if (strcmp(BT_Uart1_ReceiveBuffer, "AUTO") == 0 ){
            // on
            XUartLite_Send(&bt_uart1, "AUTO MODE\n", 10);
            searching_off_flag = 0;
        }
        else if (strcmp(BT_Uart1_ReceiveBuffer, "MANUAL") == 0){
            //off
            XUartLite_Send(&bt_uart1, "MANUAL MODE\n", 12);
            searching_off_flag = 1;
        }
        memset(BT_Uart1_ReceiveBuffer, 0, BUFFER_SIZE);
    }
}

void joystick_control(u8 manual_control_enable, u8 _tick_1ms){
    static u8 prev_tick = 0;
    u8 current_tick = _tick_1ms;

    if ( (prev_tick == 0) && (current_tick == 1) ){
        if(manual_control_enable == 1) {   
            step_motor->motor_on = 1;         
            if      (ADC_DATA > 60000) {
                step_motor->target_deg += 4;
            } 
            else if (ADC_DATA > 50000) {
                step_motor->target_deg += 3;
            }
            else if (ADC_DATA > 40000) {
                step_motor->target_deg += 2;
            }
            else if (ADC_DATA > 30000) {
                step_motor->target_deg += 1;
            }
            else if (ADC_DATA > 20000) {
                //
            }
            else if (ADC_DATA > 18000) {
                //
            }
            else if (ADC_DATA > 15000) {
                step_motor->target_deg -= 1;
            }
            else if (ADC_DATA > 10000) {
                step_motor->target_deg -= 2;
            }
            else if (ADC_DATA > 5000) {
                step_motor->target_deg -= 3;
            }
            else {
                step_motor->target_deg -= 4;
            }
            xil_printf("%d\t %d\t %d\n", step_motor->target_deg, step_motor->cur_deg, ADC_DATA);

        }
    }    
    if (step_motor->target_deg <= 20) step_motor->target_deg = 20;
    else if (step_motor->target_deg >= 4070) step_motor->target_deg = 4070;

    prev_tick = _tick_1ms;
}

int edge_detector(u8 current_clk_source, u8 index){
    static u8 prev_clk_source[10] = {0,};
    u8 result = 0;

    if ((prev_clk_source[index] == 0) && (current_clk_source == 1)){
        result = 1; // posedge
    }
    else if ((prev_clk_source[index] == 1) && (current_clk_source == 0)){
        result = -1; // negedge
    }
    else{
        result = 0; 
    }
    prev_clk_source[index] = current_clk_source;

    return result;
}