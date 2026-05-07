/* TI includes */
#include <LcdDriver/Crystalfontz128x128_ST7735.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

/* Standard includes */
#include <stdio.h>

/* HAL includes */
#include "HAL/Button.h"
#include "HAL/LED.h"
#include "HAL/Timer.h"

/* Application Header */


// --------------------------------------------------------
// MACROS
// --------------------------------------------------------

#define START_SCREEN_DELAY 96000000 // 2 seconds, Prescaler = 1

                         // FUNCTION PROTOTYPES //

// --------------------------------------------------------
// GIVEN TEMPLATE FUNCTIONS
// --------------------------------------------------------

void initialize();
void initGraphics(Graphics_Context *g_sContext_p);
void main_loop(Graphics_Context *g_sContext_p);
void sleep();


// --------------------------------------------------------
// STRUCTURES
// --------------------------------------------------------

// FSM structure
typedef enum {
    START,
    MENU,
    INSTRUCTIONS,
    GAME
} FSM_state;


// --------------------------------------------------------
// COLOR LOGIC FUNCTIONS
// --------------------------------------------------------
void wipe_LEDs();


// --------------------------------------------------------
// START SCREEN
// --------------------------------------------------------
void initialize_Start_Screen(Graphics_Context *g_sContext_p);
void initialize_Start_Screen_Timer();


// --------------------------------------------------------
// MENU SCREEN
// --------------------------------------------------------
void initialize_Menu_Screen(Graphics_Context *g_sContext_p);



// --------------------------------------------------------
// MAIN
// --------------------------------------------------------

int main(void)
{

    initialize();
    
    /* Graphics context. Works in the same as it did in the previous projects.
     You may choose to make a HAL struct and store the Graphics context in there
     instead. You may also choose to use the GFX struct from Guess the Color. */
    Graphics_Context g_sContext;
    initGraphics(&g_sContext);



    while (1)
    {
        sleep();

        main_loop(&g_sContext);
    }

}



// --------------------------------------------------------
// START SCREEN
// --------------------------------------------------------

void initialize_Start_Screen(Graphics_Context *g_sContext_p){

    Graphics_clearDisplay(g_sContext_p);

    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"SPRING 2026 PROJECT 3", AUTO_STRING_LENGTH, 64, 12, false);
    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 64, 12, false);
    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"By: Andrew Wallo V", AUTO_STRING_LENGTH, 74, 12, false);

}



// --------------------------------------------------------
// MENU SCEEN
// --------------------------------------------------------
void initialize_Menu_Screen(Graphics_Context *g_sContext_p){

}

// 2-second start screen timer
void initialize_Start_Screen_Timer(){

    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, START_SCREEN_DELAY);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(INT_T32_INT2);
    Timer32_startTimer(TIMER32_0_BASE, 1);
}
// --------------------------------------------------------
// COLOR LOGIC FUNCTIONS
// --------------------------------------------------------

// Turns off all lights
void wipe_LEDs(){

    TurnOff_LLR();
    TurnOff_LLG();
    TurnOff_LLB();
    TurnOff_BLR();
    TurnOff_BLG();
    TurnOff_BLB();
    TurnOff_LL1();
}


// --------------------------------------------------------
// GIVEN TEMPLATE FUNCTIONS
// --------------------------------------------------------


void sleep()
{
    // The Launchpad Green LED is used to signify the processor is in low-power
    // mode. From the human perspective, it should seem the processor is always
    // asleep except for fractions of second here and there.

    TurnOn_LLG();
    // Enters the Low Power Mode 0 - the processor is asleep and only responds to
    // interrupts
    PCM_gotoLPM0();
    TurnOff_LLG();
}

void main_loop(Graphics_Context *g_sContext_p)
{

    static volatile FSM_state state = START;
    buttons_t buttons = updateButtons();

    switch(state){

    case START:
        if(evaluateStartScreen() == false){
            Graphics_clearDisplay(g_sContext_p);
            initialize_Menu_Screen(g_sContext_p);
            state = MENU;
        }
    case MENU:

        break;

    }

    // TEST
    if (buttons.LB1tapped)
        Toggle_LL1();
}

// Initialization part is always device dependent and therefore does not change
// much with HAL
void initialize()
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);
    InitSystemTiming();

    initLEDs();
    initButtons();

    // add any other initializations here

}

void initGraphics(Graphics_Context *g_sContext_p)
{
    // initializing the display
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // setting up the graphics
    Graphics_initContext(g_sContext_p, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(g_sContext_p, &g_sFontFixed6x8);

    // set colors
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(g_sContext_p, GRAPHICS_COLOR_BLACK);

    // apply changes by clearing background
    Graphics_clearDisplay(g_sContext_p);

    // Show Start Screen Graphics
    initialize_Start_Screen(g_sContext_p);
}
