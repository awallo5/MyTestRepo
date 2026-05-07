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

#define START_SCREEN_DELAY 144000000 // ~ 2 seconds, Prescaler = 1

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
    GAME1,
    GAME2,
    GAME3,
    GAME4,
    GAME5
} FSM_state;


// --------------------------------------------------------
// COLOR LOGIC FUNCTIONS
// --------------------------------------------------------
void wipe_LEDs();


// --------------------------------------------------------
// START
// --------------------------------------------------------
void initialize_Start_Screen(Graphics_Context *g_sContext_p);
void initialize_Start_Screen_Timer();


// --------------------------------------------------------
// MENU
// --------------------------------------------------------
void initialize_Menu_Screen(Graphics_Context *g_sContext_p);


// --------------------------------------------------------
// INSTRUCTIONS
// --------------------------------------------------------
void initialize_Instructions_Screen(Graphics_Context *g_sContext_p);


// --------------------------------------------------------
// GAME FSM + SCREEN (4 games total)
// --------------------------------------------------------

void initialize_GAME1_Screen(Graphics_Context *g_sContext_p);
void run_GAME1_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state);

void initialize_GAME2_Screen(Graphics_Context *g_sContext_p);
void run_GAME2_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state);

void initialize_GAME3_Screen(Graphics_Context *g_sContext_p);
void run_GAME3_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state);

void initialize_GAME4_Screen(Graphics_Context *g_sContext_p);
void run_GAME4_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state);

void initialize_GAME5_Screen(Graphics_Context *g_sContext_p);
void run_GAME5_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state);



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
// START
// --------------------------------------------------------

void initialize_Start_Screen(Graphics_Context *g_sContext_p){

    Graphics_clearDisplay(g_sContext_p);

    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"SPRING 2026 PROJECT 3", AUTO_STRING_LENGTH, 64, 12, false);
    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 64, 24, false);
    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"By: Andrew Wallo V", AUTO_STRING_LENGTH, 64, 36, false);

}


// 2-second start screen timer
void initialize_Start_Screen_Timer(){

    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_1_BASE, START_SCREEN_DELAY);
    Timer32_enableInterrupt(TIMER32_1_BASE);
    Interrupt_enableInterrupt(INT_T32_INT2);
    Timer32_startTimer(TIMER32_1_BASE, 1);
}

// --------------------------------------------------------
// MENU
// --------------------------------------------------------
void initialize_Menu_Screen(Graphics_Context *g_sContext_p){

   Graphics_clearDisplay(g_sContext_p);

   Graphics_drawStringCentered(g_sContext_p, (int8_t *)"PROJECT 3", AUTO_STRING_LENGTH, 64, 12, false);
   Graphics_drawString(g_sContext_p, (int8_t *)"B1: Start", AUTO_STRING_LENGTH, 4, 24, false);
   Graphics_drawString(g_sContext_p, (int8_t *)"B2: Instructions", AUTO_STRING_LENGTH, 4, 36, false);
}


// --------------------------------------------------------
// INSTRUCTIONS
// --------------------------------------------------------
void initialize_Instructions_Screen(Graphics_Context *g_sContext_p){

    Graphics_clearDisplay(g_sContext_p);

    Graphics_drawStringCentered(g_sContext_p, (int8_t *)"PROJECT 3", AUTO_STRING_LENGTH, 64, 10, false);

    Graphics_drawString(g_sContext_p, (int8_t *)"Use LB2 to navigate", AUTO_STRING_LENGTH, 4, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"through the game", AUTO_STRING_LENGTH, 4, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"screens. Read the", AUTO_STRING_LENGTH, 4, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"instructions on each", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"screen to know what", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"you need to do. Each", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"interrupt should", AUTO_STRING_LENGTH, 4, 10 * 8, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"only work on that", AUTO_STRING_LENGTH, 4, 10 * 9, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"screen.", AUTO_STRING_LENGTH, 4, 10 * 10, false);

    Graphics_drawString(g_sContext_p, (int8_t *)"Press BB2 to return.", AUTO_STRING_LENGTH, 4, 10 * 11, false);

}

// --------------------------------------------------------
// GAME1 (Buttons With Interrupts)
// --------------------------------------------------------
void initialize_GAME1_Screen(Graphics_Context *g_sContext_p){
    Graphics_clearDisplay(g_sContext_p);
    Graphics_drawString(g_sContext_p, (int8_t *)"Project 3: Buttons", AUTO_STRING_LENGTH, 0, 10, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"Press various buttons", AUTO_STRING_LENGTH, 0, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"to toggle different", AUTO_STRING_LENGTH, 0, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"LEDs", AUTO_STRING_LENGTH, 0, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"LB1: LL1", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"BB1: BLR", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"BB2: BLG", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"JSB: BLB", AUTO_STRING_LENGTH, 4, 10 * 8, false);
}

void run_GAME1_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state){


    if(buttons->LB1tapped){
        wipe_LEDs();
        Toggle_LL1();
        } else

    if(buttons->BB1tapped){
        wipe_LEDs();
        Toggle_BLR();
        } else

    if(buttons->BB2tapped){
        wipe_LEDs();
        Toggle_BLG();
        } else

    if(buttons->JSBtapped){
        wipe_LEDs();
        Toggle_BLB();
        } else

    // EXIT CONDITION
    if(buttons->LB2tapped){

        wipe_LEDs(); // exiting game requires reset of LEDs
        initialize_GAME2_Screen(g_sContext_p);
        *state = GAME2;
    }
}

// --------------------------------------------------------
// GAME2 (Hardware Timer)
// --------------------------------------------------------
void initialize_GAME2_Screen(Graphics_Context *g_sContext_p){
    Graphics_clearDisplay(g_sContext_p);
    Graphics_drawString(g_sContext_p, (int8_t *)"Project 3: HW Timer", AUTO_STRING_LENGTH, 0, 10, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 8, false);
}

void run_GAME2_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state){
    if(buttons->LB2tapped){
        initialize_GAME3_Screen(g_sContext_p);
        *state = GAME3;
    }
}

// --------------------------------------------------------
// GAME3 (UART)
// --------------------------------------------------------
void initialize_GAME3_Screen(Graphics_Context *g_sContext_p){
    Graphics_clearDisplay(g_sContext_p);
    Graphics_drawString(g_sContext_p, (int8_t *)"Project 3: UART", AUTO_STRING_LENGTH, 0, 10, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 8, false);
}

void run_GAME3_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state){
    if(buttons->LB2tapped){
        initialize_GAME4_Screen(g_sContext_p);
        *state = GAME4;
    }
}

// --------------------------------------------------------
// GAME4 (Joystick)
// --------------------------------------------------------
void initialize_GAME4_Screen(Graphics_Context *g_sContext_p){
    Graphics_clearDisplay(g_sContext_p);
    Graphics_drawString(g_sContext_p, (int8_t *)"Project 3: Joystick", AUTO_STRING_LENGTH, 0, 10, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 8, false);
}

void run_GAME4_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state){
    if(buttons->LB2tapped){
        initialize_GAME5_Screen(g_sContext_p);
        *state = GAME5;
    }
}

// --------------------------------------------------------
// GAME5 (Color Mixer)
// --------------------------------------------------------
void initialize_GAME5_Screen(Graphics_Context *g_sContext_p){
    Graphics_clearDisplay(g_sContext_p);
    Graphics_drawString(g_sContext_p, (int8_t *)"Project 3: ColorMixer", AUTO_STRING_LENGTH, 0, 10, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 2, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 3, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 0, 10 * 4, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 5, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 6, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 7, false);
    Graphics_drawString(g_sContext_p, (int8_t *)"", AUTO_STRING_LENGTH, 4, 10 * 8, false);
}

void run_GAME5_FSM(buttons_t *buttons, Graphics_Context *g_sContext_p, volatile FSM_state *state){
    if(buttons->LB2tapped){
        initialize_GAME1_Screen(g_sContext_p);
        *state = GAME1;
    }
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
            initialize_Menu_Screen(g_sContext_p);
            state = MENU;
        }
        break;

    case MENU:
        if(buttons.BB2tapped){
            initialize_Instructions_Screen(g_sContext_p);
        state = INSTRUCTIONS;
        } else if(buttons.BB1tapped){
            initialize_GAME1_Screen(g_sContext_p);
            state = GAME1;
        }
        break;

    case INSTRUCTIONS:
        if(buttons.BB2tapped){
            initialize_Menu_Screen(g_sContext_p);
            state = MENU;
        }
        break;

        // GAME LOGIC MOVED TO INDIVIDUAL FSM modules
    case GAME1:
       run_GAME1_FSM(&buttons, g_sContext_p, &state);
       break;

    case GAME2:
        run_GAME2_FSM(&buttons, g_sContext_p, &state);
        break;

    case GAME3:
        run_GAME3_FSM(&buttons, g_sContext_p, &state);
        break;

    case GAME4:
        run_GAME4_FSM(&buttons, g_sContext_p, &state);
        break;

    case GAME5:
        run_GAME5_FSM(&buttons, g_sContext_p, &state);
        break;

    }

    /*   // TEST

    if (buttons.LB1tapped)
        Toggle_LL1();

     */
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
    initialize_Start_Screen_Timer();

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
