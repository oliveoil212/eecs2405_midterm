#include "mbed.h"
#include "mbed_events.h"
#include "uLCD_4DGL.h"

Timer t;
using namespace std::chrono;
InterruptIn buttom_up(D13);
InterruptIn buttom_down(D12);
InterruptIn buttom_select(D11);
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut Aout(PA_4); // D7 pin
AnalogIn Ain(A0);
Thread printfThread(osPriorityLow);
Thread eventThread(osPriorityNormal);
EventQueue printfQueue;
EventQueue eventQueue;
Thread g, s;
Ticker counter;
int cnt = 0;
long int  period;
int freq = 1;
int selected = 0;
int selection =4;
bool sampling = 0;
float out;
void lcd_update(){
    switch(selection){
        case 1: 
            // uLCD.textbackground_color(WHITE);
            uLCD.locate(0,2);
            uLCD.printf("-");
            uLCD.locate(0,4);
            uLCD.printf(" ");
            uLCD.locate(0,6);
            uLCD.printf(" ");
            uLCD.locate(0,8);
            uLCD.printf(" ");
            break;
        case 2: 
            // uLCD.textbackground_color(WHITE);
            uLCD.locate(0,2);
            uLCD.printf(" ");
            uLCD.locate(0,4);
            uLCD.printf("-");
            uLCD.locate(0,6);
            uLCD.printf(" ");
            uLCD.locate(0,8);
            uLCD.printf(" ");
            break;
        case 3: 
            // uLCD.textbackground_color(WHITE);
            uLCD.locate(0,2);
            uLCD.printf(" ");
            uLCD.locate(0,4);
            uLCD.printf(" ");
            uLCD.locate(0,6);
            uLCD.printf("-");
            uLCD.locate(0,8);
            uLCD.printf(" ");
            break;
        case 4: 
            // uLCD.textbackground_color(WHITE);
            uLCD.locate(0,2);
            uLCD.printf(" ");
            uLCD.locate(0,4);
            uLCD.printf(" ");
            uLCD.locate(0,6);
            uLCD.printf(" ");
            uLCD.locate(0,8);
            uLCD.printf("-");
            break;
            // uLCD.printf("error");
    }
}
void count_func() {
    cnt++;
}
void bt1_irq(){ // up buttom
    selection++;
    if(selection > 4) selection = 4;
    // printfQueue.call(&lcd_update);
}
void bt2_irq(){   //down bottom
    selection--;
    if(selection < 1) selection = 1;
    // printfQueue.call(&lcd_update);
}
void generator_thread() {

    float i;
    while(1) {
        if (1) {
            if (selection == 1) {        // slew1
                for (i = 0.0f; i < 1; i+=0.000175635f) {
                    Aout = i;
                }

                ThisThread::sleep_for(80ms);  
                for (i = 1.0f; i > 0.0f; i -= 0.000175635f) {
                    Aout = i;
                }
            }
            else if (selection == 2) {        // slew1/2
                for (i = 0.0f; i < 1; i+=0.000345635f) {
                    Aout = i;
                }

                ThisThread::sleep_for(160ms);  
                for (i = 1.0f; i > 0.0f; i -= 0.000345635f) {
                    Aout = i;
                }
            }
            else if (selection == 3) {        // slew 1/4
                for (i = 0.0f; i < 1; i+=0.000690005f) {
                    Aout = i;
                }
                ThisThread::sleep_for(200ms);  
                for (i = 1.0f; i > 0.0f; i -= 0.000690005f) {
                    Aout = i;
                }
            }    
            else if (selection == 4) {        // slew 1/8
                for (i = 0.0f; i < 1; i+=0.001400005f) {
                    Aout = i;
                }
                ThisThread::sleep_for(220ms);  
                for (i = 1.0f; i > 0.0f; i -= 0.001400005f) {
                    Aout = i;
                }
            }    
        }
    }
}
int sample = 500;
float ADCdata[500];
void sample_thread() {
    int i = 0;
    sampling = 1;
    // t.start();
    for (i = 0; i < sample; i++){
        // Aout = Ain;
        ADCdata[i] = Ain;
        // ThisThread::sleep_for(1ms); // sampling rate = 500/s 實際55/s
        ThisThread::sleep_for(1000ms/sample); // sampling rate = 500/s 實際55/s
        // wait_us(100);
    }
    sampling = 0;
    // t.stop();
    // auto ms = chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count();
    for (i = 0; i < sample; i++) {
        printf("%f\r\n", ADCdata[i]);
    }
    // printf ("Timer time: %llu ms\n", ms);
}
void bt3_irq(){ //selected buttom
    printfQueue.call(&sample_thread);
    selected = 1;
}
int main() {
    uLCD.reset();
    uLCD.background_color(0xDB7093);
    uLCD.cls();

    // uLCD.color(0xF0FFF0);
    // uLCD.textbackground_color(0x00FF00);
    // uLCD.printf("108061213_HW2\n");
    // uLCD.printf("cutoff freq 152Hz\n");
    // uLCD.textbackground_color(0xCD853F);

    // uLCD.text_width(2); //3X size text
    // uLCD.text_height(2);
    uLCD.color(GREEN);
    uLCD.locate(1,2);
    uLCD.printf("1");
    uLCD.locate(1,4);
    uLCD.printf("1/2");
    uLCD.locate(1,6);
    uLCD.printf("1/4");
    uLCD.locate(1,8);
    uLCD.printf("1/8");
    printfThread.start(callback(&printfQueue, &EventQueue::dispatch_forever));
    // normal priority thread for other events
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));

    // debounce.start();
    // buttom_1.fall(&bt1_irq);
    buttom_down.fall(&bt2_irq);
    buttom_up.fall(&bt1_irq);
    buttom_select.fall(&bt3_irq);
    g.start(generator_thread);
    while(selected != 1) {
        //  uLCD.locate(0,3);
        //         uLCD.printf("%d",selection);
        lcd_update();

        // ThisThread::sleep_for(1000ms); // sampling rate = 500/s 實際55/s
    }
    // uLCD.locate(0,10);
    // uLCD.printf(" start!");
    // printfQueue.call(&sample_thread);
    // sample_thread();
    // uLCD.locate(0,10);
    // uLCD.printf(" done!");

}


