// Don't use #define in C++ programs. Make these typed const members

#define conc1                   0.5
#define conc2                   5
#define conc3                   10
#define LockOutDefault          5
#define DOOR_SENSE		p5
#define SYRINGE_SENSE		p6
#define LEVEL_SENSE		P7
#define ON_BUTTON		P8
#define OFF_BUTTON 		p9
#define NO_BUTTON		P10
#define ENTER_BUTTON 		p11
#define UP_BUTTON		p12
#define DOWN_BUTTON		p13
#define BOLUS_BUTTON		p14
#define LOAD_BUTTON		p15
//Ticker
Ticker pca_lockout_timer;
Ticker pca_cont_lockout_timer;

//GPIO_LED
DigitalOut LED_YELLOW(LED1);
DigitalOut LED_RED(LED2);
DigitalOut LED_TEST(LED3);
DigitalOut Intrpt(p25);

// make the pin names symbolic
//Interrupt pins
    InterruptIn Door_closed(DOOR_SENSE);
    InterruptIn Syringe_placed(SYRINGE_SENSE);
    InterruptIn Level_high(LEVEL_SENSE);
    InterruptIn on(ON_BUTTON);
    InterruptIn off(OFF_BUTTON);
    InterruptIn no(NO_BUTTON);
    InterruptIn enter(ENTER_BUTTON);
    InterruptIn up(UP_BUTTON);
    InterruptIn down(DOWN_BUTTON);
    InterruptIn bolus(BOLUS_BUTTON);
    InterruptIn load(LOAD_BUTTON);
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

// If you are going to program in C++, the machine should include the transition functions,
// since the machine pointer is essentially just *this.
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
	// document these fields
    float   drug_concentration;
	// why is this an int instead of an enum?
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
// put these in the machine class
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
/*The function start_motor guarantees the delivery of right dosage of the medicine to the patient.
structure user_inputs contains the dosage to be delivered which is used by the motor function to 
determine the duration for the motor to be run */
void stop_motor();
