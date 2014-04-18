// Don't use #define in C++ programs. Make these typed const members

#define conc1                   0.5
#define conc2                   5
#define conc3                   10
#define LockOutDefault          5

//Ticker
Ticker pca_lockout_timer;
Ticker pca_cont_lockout_timer;

//GPIO_LED
DigitalOut LED_YELLOW(LED1);
DigitalOut LED_RED(LED2);
DigitalOut LED_TEST(LED3);
DigitalOut Intrpt(p25);

//Interrupt pins
    InterruptIn Door_closed(p5);
    InterruptIn Syringe_placed(p6);
    InterruptIn Level_high(p7);
    InterruptIn on(p8);
    InterruptIn off(p9);
    InterruptIn no(p10);
    InterruptIn enter(p11);
    InterruptIn up(p12);
    InterruptIn down(p13);
    InterruptIn bolus(p14);
    InterruptIn load(p15);
    //InterruptIn timeout(p16);

//Events
typedef enum
{
    ON,
    OFF,                        
    NO,                     
    ENTER,                      
    UP,                         
    DOWN,                       
    BOLUS,                      
    LOAD,       
    TIMEOUT,
    RETURN, 
    DOOR_CLOSED,
    DOOR_OPEN,  
    SYRINGE_PRESENT,
    SYRINGE_ABSENT,
    LEVEL_HIGH,
    LEVEL_LOW
}Event;

typedef struct StateMachine TStateMachine;
typedef void (*StateProc)(TStateMachine *sm, Event input);
TStateMachine *smart_pca;

struct StateMachine
{
    StateProc CurrentState;
    StateProc PreviousState;
};

struct status
{
    bool doorclosed;
    bool syringeplaced;
    bool levelhigh;
    
}sensorstatus;

struct user_requirements
{
    float   drug_concentration;
    int     mode_selected;
    float   pca_dosage;
    int     pca_lockout;
    float   cont_rate;
    float   pca_cont_rate;
    float   pca_cont_bolus_dosage;
    int     pca_cont_lockout;
    bool    bolus;
}user_input;

TStateMachine not_so_smart_pca;

void OFFSTATE(TStateMachine *state, Event input);
void CONC1(TStateMachine    *state, Event input);
void CONC2(TStateMachine    *state, Event input);
void CONC3(TStateMachine    *state, Event input);
void MODE_SELECT_PCA(TStateMachine  *state, Event input);
void MODE_SELECT_CONT(TStateMachine *state, Event input);
void MODE_SELECT_PCA_CONT(TStateMachine *state, Event input);
void PCA_DOSAGE(TStateMachine   *state, Event input);
void PCA_LOCKOUT(TStateMachine  *state, Event input);
void PCA_IDLE(TStateMachine *state, Event input);
void PCA_READY(TStateMachine    *state, Event input);
void CONT_RATE(TStateMachine    *state, Event input);
void CONT_READY(TStateMachine   *state, Event input);
void PCA_CONT_RATE(TStateMachine    *state, Event input);
void PCA_CONT_BOLUS_DOSAGE(TStateMachine    *state, Event input);
void PCA_CONT_LOCKOUT(TStateMachine *state, Event input);
void PCA_CONT_READY(TStateMachine   *state, Event input);
void PCA_CONT_BOLUS(TStateMachine   *state, Event input);
void PCA_CONT_LOCKOUT_WAIT(TStateMachine    *state, Event input);
void ERROR_DOOR_STATE(TStateMachine *state, Event input);
void ERROR_SYRINGE_STATE(TStateMachine  *state, Event input);
void ERROR_LEVEL_STATE  (TStateMachine  *state, Event input);
void start_motor(struct user_requirements user_inputs);
void stop_motor();
