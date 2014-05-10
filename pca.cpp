#include "mbed.h"
#include "pca.h"

#define _(x)


Serial pc(USBTX, USBRX);

// 
/* ------------- ISRs ---------------------------------- */
void DoorClosed()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is DOOR_CLOSED iff CurrentState != ERROR_DOOR_STATE)

    sensorstatus.doorclosed=true;
    if(smart_pca->CurrentState != ERROR_DOOR_STATE)
        (smart_pca->CurrentState)(smart_pca,DOOR_CLOSED);
}

void DoorOpen()
{
    _(requires falling edge on interrupt pin)
    _(ensures Event is DOOR_OPEN)
    
    sensorstatus.doorclosed=false;
    (smart_pca->CurrentState)(smart_pca,DOOR_OPEN);
}

void SyringePlaced()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is SYRINGE_PRESENT)    
    
    sensorstatus.syringeplaced=true;
    (smart_pca->CurrentState)(smart_pca,SYRINGE_PRESENT);
}

void SyringeAbsent()
{
    _(requires falling edge on interrupt pin)
    _(ensures Event is SYRINGE_ABSENT)
    
    sensorstatus.syringeplaced=false;  
    (smart_pca->CurrentState)(smart_pca,SYRINGE_ABSENT);
}

void LevelHigh()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is LEVEL_HIGH)
    
    sensorstatus.levelhigh=true;
    (smart_pca->CurrentState)(smart_pca,LEVEL_HIGH);
}

void LevelLow()
{
    _(requires falling edge on interrupt pin)
    _(ensures Event is LEVEL_LOW)
    
    sensorstatus.levelhigh=false;
    (smart_pca->CurrentState)(smart_pca,LEVEL_LOW);
}

void On()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is ON)
    
    (smart_pca->CurrentState)(smart_pca,ON);
}

void Off()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is OFF)
    
    (smart_pca->CurrentState)(smart_pca,OFF);
}

void No()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is NO)    
    
    (smart_pca->CurrentState)(smart_pca,NO);
}

void Enter()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is ENTER)
    
    (smart_pca->CurrentState)(smart_pca,ENTER);
}

void Up()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is UP)
    
    (smart_pca->CurrentState)(smart_pca,UP);
}

void Down()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is DOWN)
    
    (smart_pca->CurrentState)(smart_pca,DOWN);
}

void Bolus()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is BOLUS)
    
    (smart_pca->CurrentState)(smart_pca,BOLUS);
}

void Load()
{
    _(requires rising edge on interrupt pin)
    _(ensures Event is LOAD)
    
    (smart_pca->CurrentState)(smart_pca,LOAD);
}

void PCA_lockout()
{
    _(requires timeout of pca_lockout_timer)
    _(ensures Event is TIMEOUT)
    
     pca_lockout_timer.detach();
     bolus.rise(&Bolus);        //Enable load Interrupt
    (smart_pca->CurrentState)(smart_pca,TIMEOUT);
}

void PCA_cont_lockout()
{
    _(requires timeout of pca_cont_lockout_timer)
    _(ensures Event is TIMEOUT)    
    
     pca_cont_lockout_timer.detach();
    (smart_pca->CurrentState)(smart_pca,TIMEOUT);
}
/* ------------- end ISRs ------------------------------ */

/* ------------- MOTOR FUNCTIONS ----------------------- */
void start_motor(struct user_requirements user_inputs)
{
	/* This code is a stub in place of actual motor code.
	The actual function is intended to extract the dosage and other parameters from the
	structure user_inputs passed to it and run till the dosage is delivered.
	drug_concentration-->used by the motor to calculate the volume of the drug to be delivered;
    	mode_selected--> reserved for future use;
    	pca_dosage--> dosage to be delivered when in PCA mode, pca_dosage in conjunction with drug_concentration will determine
    	the volume of the medicine to be delivered;
    	pca_lockout--> reserved for future use;
    	cont_rate--> dosage in CONTINUOUS mode;
    	pca_cont_rate--> dosage in PCA + CONT mode;
    	pca_cont_bolus_dosage--> additional dosage on bolus in PCA + CONT mode;
    	pca_cont_lockout--> reserved for future use;
    	bolus--> reserved for future use;
    	The motor could use feedback from the flow sensor to continuously to check if the required quantity is deliverd
    	or could precisely determine the number of turns of the motor and map it to the volume delivered*/
    switch(user_inputs.mode_selected)
    {
        case 1: LED_YELLOW = 1;
                wait(10);
                LED_YELLOW = 0;
                break;
        case 2: LED_RED = 1;
                wait(10);
                LED_RED = 0;
                break;
        case 3: LED_RED = 1;
                LED_YELLOW = 1;
                wait(20);
                LED_RED = 0;
                LED_YELLOW = 0;
                break;
        default: LED_RED = 0;
                 LED_YELLOW = 0;
                 break;
    }
}
    
void stop_motor()
{
    /*Stub function for the actual stop_motor code.
    The stop_motor function is required to halt the motor to bring it to a stable state of rest*/
    LED_RED = 0;
    LED_YELLOW = 0;
}
    
/* ------------- end Motor Functions ------------------ */

// I appreciate you're trying to document your methods. However, this usually isn't useful for a state
// machine, since they just mirror what you could say in the code (since the code isn't usually doing
// anything clever to implement the spec). Moreover, a client isn't going to read these specs (or the code) anyway.

// What would be useful for your state machine would be the relevant invariants capturing how this machine
// is delivering medecine to the patient. Please talk to me if you need help with this.

/* ------------- STATE MACHINE ------------------------- */
void OFFSTATE(TStateMachine *state, Event input)
{
    _(ensures CurrentState == CONC1 iff Event == ON && door open && Syringe placed && level high)

    switch(input)
    {
        case ON:        if(sensorstatus.doorclosed)
                            //Display(1)- "DOOR OPEN\n";
                        else if(!sensorstatus.syringeplaced)
                            //Display(2)- Syringe Absent!!
                        else if(!sensorstatus.levelhigh)
                            //Display(3)- Level low!!
                        else
                        {
                            //Display(5)- Select concentration menu with 1st option highlighted
                            state->CurrentState=CONC1;
                        }
                        break;
        default:        break;              
    }               
}

void CONC1(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA && user_input.drug_concentration == conc1 iff Event == ENTER)
    _(ensures CurrentState == CONC3 iff Event == UP)
    _(ensures CurrentState == CONC2 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == CONC1 iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == CONC1 iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case ENTER:             user_input.drug_concentration=conc1;
                                user_input.mode_selected=0;
                                // Display(8) - Select mode menu with PCA mode highlighted
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case UP:                //Display(7) - Highlight 3rd option
                                state->CurrentState=CONC3;
                                break;
        case DOWN:              //Display(6) - Highlight 2nd option
                                state->CurrentState=CONC2;
                                break;
        case DOOR_CLOSED:       state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=CONC1;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=CONC1;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;
    }
}

void CONC2(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA && user_input.drug_concentration == conc2 iff Event == ENTER)
    _(ensures CurrentState == CONC1 iff Event == UP)
    _(ensures CurrentState == CONC3 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == CONC2 iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == CONC2 iff Event == LEVEL_LOW)

    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case ENTER:             user_input.drug_concentration=conc2;
                                user_input.mode_selected=0;
                                //Display(8) - Select mode menu with PCA mode highlighted
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case UP:                //Display(5) - Select concentration menu with 1st option highlighted

                                state->CurrentState=CONC1;
                                break;
        case DOWN:              //Display(7) - Highlight 3rd option
                                state->CurrentState=CONC3;
                                break;
        case DOOR_CLOSED:       state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=CONC2;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=CONC2;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;
    }
}

void CONC3(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA && user_input.drug_concentration == conc3 iff Event == ENTER)
    _(ensures CurrentState == CONC2 iff Event == UP)
    _(ensures CurrentState == CONC1 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == CONC3 iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == CONC3 iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case ENTER:             user_input.drug_concentration=conc3;
                                user_input.mode_selected=0;
                                //Display(8) - Select mode menu with PCA mode highlighted
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case UP:                //Display(7) - Highlight 2nd option
                                state->CurrentState=CONC2;
                                break;
        case DOWN:              //Display(5) - Select concentration menu with 1st option highlighted
                                state->CurrentState=CONC1;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=CONC3;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=CONC3;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;
    }
}

void MODE_SELECT_PCA(TStateMachine  *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == CONC1 iff Event == NO)
    _(ensures CurrentState == PCA_DOSAGE && user_input.mode_selected == 1 iff Event == ENTER)
    _(ensures CurrentState == MODE_SELECT_PCA_CONT iff Event == UP)
    _(ensures CurrentState == MODE_SELECT_CONT iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == MODE_SELECT_PCA iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == MODE_SELECT_PCA iff Event == LEVEL_LOW)

    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(5) - Select concentration menu with 1st option highlighted
                                state->CurrentState=CONC1;
                                break;
        case ENTER:             user_input.mode_selected=1;
                                //Display(11) - PCA mode dosage options
                                user_input.pca_dosage=0;
                                state->CurrentState=PCA_DOSAGE;
                                break;
        case UP:                //Display(10) - Highlight PCA+Continuous mode
                                state->CurrentState=MODE_SELECT_PCA_CONT;
                                break;
        case DOWN:              //Display(9) - Highlight Continuous mode
                                state->CurrentState=MODE_SELECT_CONT;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=MODE_SELECT_PCA;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=MODE_SELECT_PCA;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break; 
    }
}

void MODE_SELECT_CONT(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == CONC1 iff Event == NO)
    _(ensures CurrentState == CONT_RATE && user_input.mode_selected == 2 iff Event == ENTER)
    _(ensures CurrentState == MODE_SELECT_PCA iff Event == UP)
    _(ensures CurrentState == MODE_SELECT_PCA_CONT iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == MODE_SELECT_CONT iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == MODE_SELECT_CONT iff Event == LEVEL_LOW)

    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(5) - Select concentration menu with 1st option highlighted
                                state->CurrentState=CONC1;
                                break;
        case ENTER:             user_input.mode_selected=2;
                                //Display() - Continuous mode rate options
                                user_input.cont_rate=0;
                                state->CurrentState=CONT_RATE;
                                break;
        case UP:                //Display(8) - Highlight PCA mode
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case DOWN:              //Display(10) - Highlight PCA+Continuous mode
                                state->CurrentState=MODE_SELECT_PCA_CONT;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=MODE_SELECT_CONT;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=MODE_SELECT_CONT;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break; 
    }
}

void MODE_SELECT_PCA_CONT(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == CONC1 iff Event == NO)
    _(ensures CurrentState == PCA_CONT_RATE && user_input.mode_selected == 3 iff Event == ENTER)
    _(ensures CurrentState == MODE_SELECT_CONT iff Event == UP)
    _(ensures CurrentState == MODE_SELECT_PCA iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == MODE_SELECT_PCA_CONT iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == MODE_SELECT_PCA_CONT iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(5) - Select concentration menu with 1st option highlighted
                                state->CurrentState=CONC1;
                                break;
        case ENTER:             user_input.mode_selected=3;
                                //Display() - PCA+Continuous mode rate option
                                user_input.pca_cont_rate=0;
                                state->CurrentState=PCA_CONT_RATE;
                                break;
        case UP:                //Display(9) - Highlight Continuous mode
                                state->CurrentState=MODE_SELECT_CONT;
                                break;
        case DOWN:              //Display(8) - Highlight PCA mode
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=MODE_SELECT_PCA_CONT;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=MODE_SELECT_PCA_CONT;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;
    }
}

void PCA_DOSAGE(TStateMachine   *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA iff Event == NO)
    _(ensures CurrentState == PCA_LOCKOUT && (0 < user_input.pca_dosage < 100) iff Event == ENTER)
    _(ensures CurrentState == PCA_DOSAGE && user_input.pca_dosage == \old user_input.pca_dosage + 0.1 iff Event == UP)
    _(ensures CurrentState == PCA_DOSAGE && user_input.pca_dosage == \old user_input.pca_dosage - 0.1 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_DOSAGE iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_DOSAGE iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:                //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(8) - Highlight PCA mode
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case ENTER:             //Display() - Display PCA lockout options
                                user_input.pca_lockout=LockOutDefault;
                                state->CurrentState=PCA_LOCKOUT;
                                break;
        case UP:                user_input.pca_dosage = user_input.pca_dosage + 0.1;
                                if(user_input.pca_dosage > 100.0)
                                    user_input.pca_dosage=100.0;
                                //Display() - new dosage value;
                                state->CurrentState=PCA_DOSAGE;
                                break;
        case DOWN:              user_input.pca_dosage = user_input.pca_dosage - 0.1;
                                if(user_input.pca_dosage < 0.0)
                                    user_input.pca_dosage=0.0;
                                //Display() - new dosage value;
                                state->CurrentState=PCA_DOSAGE;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_DOSAGE;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=PCA_DOSAGE;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                
    }
}

void PCA_LOCKOUT(TStateMachine  *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == PCA_DOSAGE iff Event == NO)
    _(ensures CurrentState == PCA_READY && (0 < user_input.pca_lockout < 100) iff Event == ENTER)
    _(ensures CurrentState == PCA_LOCKOUT && user_input.pca_lockout == \old user_input.pca_lockout + 1 iff Event == UP)
    _(ensures CurrentState == PCA_LOCKOUT && user_input.pca_lockout == \old user_input.pca_lockout - 1 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_LOCKOUT iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_LOCKOUT iff Event == LEVEL_LOW)
   
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display() - new dosage value
                                state->CurrentState=PCA_DOSAGE;
                                break;
        case ENTER:             state->CurrentState=PCA_READY;
                                break;
        case UP:                user_input.pca_lockout++;
                                if(user_input.pca_lockout > 100)
                                user_input.pca_lockout=100;
                                //Display() - new lockout value;
                                state->CurrentState=PCA_LOCKOUT;
                                break;
        case DOWN:              user_input.pca_lockout--;
                                if(user_input.pca_lockout < 0)
                                    user_input.pca_lockout=0;
                                //Display() - new lockout value;
                                state->CurrentState=PCA_LOCKOUT;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_LOCKOUT;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=PCA_LOCKOUT;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                      
    }
}

void PCA_IDLE(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures CurrentState == \old CurrentState iff Event == LOAD)
    _(ensures CurrentState == PCA_READY iff Event == TIMEOUT)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_IDLE iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_IDLE iff Event == LEVEL_LOW)

    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;  
        case LOAD:              //Display() - Loading Dosage
                                //state->CurrentState=PCA_RUNNING;
                                start_motor(user_input); //time to be calculated after dosage is entered
                                pca_lockout_timer.detach();
                                pca_lockout_timer.attach(&PCA_lockout,user_input.pca_lockout*60);
                                break;
        case TIMEOUT:           //Display() - Ready
                                state->CurrentState=PCA_READY;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                stop_motor();
                                state->PreviousState=PCA_IDLE;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                stop_motor();
                                state->PreviousState=PCA_IDLE;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                      
    }
}

void PCA_READY(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures CurrentState == PCA_IDLE iff Event == LOAD || Event == BOLUS)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_READY iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_READY iff Event == LEVEL_LOW)

    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case BOLUS:             //Display() -  Administering Bolus
                                load.rise(NULL);        //Disable load Interrupt
                                bolus.rise(NULL);       //Disable bolus Interrupt
                                start_motor(user_input); //time to be calculated after dosage is entered
                                pca_lockout_timer.attach(&PCA_lockout,user_input.pca_lockout*60);
                                state->CurrentState=PCA_IDLE;
                                load.rise(&Load);       //Enable load Interrupt
                                break;
        case LOAD:              //Display() - Loading Dosage
                                load.rise(NULL);
                                bolus.rise(NULL);
                                start_motor(user_input); //time to be calculated after dosage is entered
                                pca_lockout_timer.attach(&PCA_lockout,user_input.pca_lockout*60);
                                state->CurrentState=PCA_IDLE;
                                load.rise(&Load);                                
                                break;
        case SYRINGE_ABSENT:    stop_motor();
                                //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_READY;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         stop_motor();
                                //Display(3) - Level low!!
                                state->PreviousState=PCA_READY;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                              
    }
}

void CONT_RATE(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA iff Event == NO)
    _(ensures CurrentState == CONT_READY && 0<=user_input.cont_rate<=100 iff Event == ENTER)
    _(ensures CurrentState == CONT_RATE && user_input.cont_rate == \old user_input.cont_rate + 0.5 iff Event == UP)
    _(ensures CurrentState == CONT_RATE && user_input.cont_rate == \old user_input.cont_rate - 0.5 iff Event == DOWN)    
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == CONT_RATE iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == CONT_RATE iff Event == LEVEL_LOW)
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(8) - Highlight PCA mode
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case ENTER:             state->CurrentState=CONT_READY;
                                //Display() - close the door                            
                                break;
        case UP:                user_input.cont_rate = user_input.cont_rate + 0.5;
                                if(user_input.cont_rate > 100.0)
                                    user_input.cont_rate=100.0;
                                //Display() - new continous rate value;
                                state->CurrentState=CONT_RATE;
                                break;
        case DOWN:              user_input.cont_rate = user_input.cont_rate - 0.5;
                                if(user_input.cont_rate < 0.0)
                                    user_input.cont_rate=0.0;
                                //Display() - new continous rate value;
                                state->CurrentState=CONT_RATE;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=CONT_RATE;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=CONT_RATE;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                                                                              
    }
}

void CONT_READY(TStateMachine   *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures CurrentState == OFFSTATE iff Event == DOOR_CLOSED && motor finished running)
    _(ensures CurrentState == ERROR_DOOR_STATE && PreviousState == CONT_READY iff Event == DOOR_OPEN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == CONT_READY iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == CONT_READY iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case DOOR_CLOSED:       //Dislay() - Motor Running
                                start_motor(user_input);
                                //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case DOOR_OPEN:         //Display() - Door Open
                                stop_motor();
                                state->PreviousState=CONT_READY;
                                state->CurrentState=ERROR_DOOR_STATE;
                                break;                              
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                stop_motor();
                                state->PreviousState=CONT_READY;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                stop_motor();
                                state->PreviousState=CONT_READY;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                          
    }
}

void PCA_CONT_RATE(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == MODE_SELECT_PCA iff Event == NO)
    _(ensures CurrentState == PCA_CONT_BOLUS_DOSAGE && 0<=user_input.cont_rate<=100 iff Event == ENTER)
    _(ensures CurrentState == PCA_CONT_RATE && user_input.cont_rate == \old user_input.cont_rate + 0.1 iff Event == UP)
    _(ensures CurrentState == PCA_CONT_RATE && user_input.cont_rate == \old user_input.cont_rate - 0.1 iff Event == DOWN)    
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_CONT_RATE iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_CONT_RATE iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(8) - Highlight PCA mode
                                state->CurrentState=MODE_SELECT_PCA;
                                break;
        case ENTER:             //Display() - Enter Bolus Dosage
                                user_input.pca_cont_bolus_dosage=0;
                                state->CurrentState=PCA_CONT_BOLUS_DOSAGE;
                                break;
        case UP:                user_input.pca_cont_rate = user_input.pca_cont_rate + 0.1;
                                if(user_input.pca_cont_rate > 100.0)
                                    user_input.pca_cont_rate=100.0;
                                //Display() - new pca_continous rate value;
                                state->CurrentState=PCA_CONT_RATE;
                                break;
        case DOWN:              user_input.pca_cont_rate = user_input.pca_cont_rate - 0.1;
                                if(user_input.pca_cont_rate < 0.0)
                                    user_input.pca_cont_rate=0.0;
                                //Display() - new continous rate value;
                                state->CurrentState=PCA_CONT_RATE;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_CONT_RATE;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=PCA_CONT_RATE;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                                                          
    }
}

void PCA_CONT_BOLUS_DOSAGE(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == PCA_CONT_RATE iff Event == NO)
    _(ensures CurrentState == PCA_CONT_LOCKOUT && 0<=user_input.cont_rate<=100 iff Event == ENTER)
    _(ensures CurrentState == PCA_CONT_BOLUS_DOSAGE && user_input.cont_rate == \old user_input.cont_rate + 0.1 iff Event == UP)
    _(ensures CurrentState == PCA_CONT_BOLUS_DOSAGE && user_input.cont_rate == \old user_input.cont_rate - 0.1 iff Event == DOWN)    
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_CONT_BOLUS_DOSAGE iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_CONT_BOLUS_DOSAGE iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display(8) - Highlight Bolus Dosage mode
                                state->CurrentState=PCA_CONT_RATE;
                                break;
        case ENTER:             //Display() - Enter Lockout value
                                user_input.pca_cont_lockout=LockOutDefault;
                                state->CurrentState=PCA_CONT_LOCKOUT;
                                break;
        case UP:                user_input.pca_cont_bolus_dosage = user_input.pca_cont_bolus_dosage + 0.1; 
                                if(user_input.pca_cont_bolus_dosage > 100.0)
                                    user_input.pca_cont_bolus_dosage=100.0;
                                //Display() - new pca_continous_bolus_dosage value;
                                state->CurrentState=PCA_CONT_BOLUS_DOSAGE;
                                break;
        case DOWN:              user_input.pca_cont_bolus_dosage = user_input.pca_cont_bolus_dosage - 0.1;
                                if(user_input.pca_cont_bolus_dosage < 0.0)
                                    user_input.pca_cont_bolus_dosage=0.0;
                                //Display() - new pca cont bolus dosage value;
                                state->CurrentState=PCA_CONT_BOLUS_DOSAGE;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_CONT_BOLUS_DOSAGE;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=PCA_CONT_BOLUS_DOSAGE;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                          
    }
}

void PCA_CONT_LOCKOUT(TStateMachine *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF || Event == DOOR_CLOSED)
    _(ensures CurrentState == PCA_CONT_BOLUS_DOSAGE iff Event == NO)
    _(ensures CurrentState == PCA_CONT_READY && (0 < user_input.pca_cont_lockout < 100) iff Event == ENTER)
    _(ensures CurrentState == PCA_CONT_LOCKOUT && user_input.pca_cont_lockout == \old user_input.pca_cont_lockout + 1 iff Event == UP)
    _(ensures CurrentState == PCA_CONT_LOCKOUT && user_input.pca_cont_lockout == \old user_input.pca_cont_lockout - 1 iff Event == DOWN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_CONT_LOCKOUT iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_CONT_LOCKOUT iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case NO:                //Display() - pca cont bolus dosage
                                state->CurrentState=PCA_CONT_BOLUS_DOSAGE;
                                break;
        case ENTER:             state->CurrentState=PCA_CONT_READY;
                                //Display() - Close the door                                
                                break;
        case UP:                user_input.pca_cont_lockout++;
                                if(user_input.pca_cont_lockout > 100)
                                    user_input.pca_cont_lockout=100;
                                //Display() - new pca_cont_lockout value;
                                state->CurrentState=PCA_CONT_LOCKOUT;
                                break;
        case DOWN:              user_input.pca_cont_lockout--;
                                if(user_input.pca_cont_lockout < 0)
                                    user_input.pca_cont_lockout=0;
                                //Display() - new pca_cont_lockout value;
                                state->CurrentState=PCA_CONT_LOCKOUT;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                state->PreviousState=PCA_CONT_LOCKOUT;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                state->PreviousState=PCA_CONT_LOCKOUT;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                          
    }
}

void PCA_CONT_READY(TStateMachine   *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures start motor then CurrentState == PCA_CONT_BOLUS && eventually CurrentState == OFFSTATE when Event == DOOR_CLOSED)
    _(ensures stop motor then PreviousState == PCA_CONT_READY && CurrentState == ERROR_DOOR_STATE when Event == DOOR_OPEN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_READY iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_READY iff Event == LEVEL_LOW)
   
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case DOOR_CLOSED:       //Display(0) - Blank display
                                user_input.bolus = false;
                                state->CurrentState=PCA_CONT_BOLUS;
                                start_motor(user_input);
                                //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;                               
                                break;
        case DOOR_OPEN:         //Display() - Door Opens
                                stop_motor();
                                state->PreviousState=PCA_CONT_READY;
                                state->CurrentState=ERROR_DOOR_STATE;
                                break;                              
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                stop_motor();       
                                state->PreviousState=PCA_CONT_READY;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                stop_motor();       
                                state->PreviousState=PCA_CONT_READY;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                          
    }
}

void PCA_CONT_BOLUS(TStateMachine   *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures CurrentState == PCA_CONT_LOCKOUT_WAIT && start motor when Event == BOLUS)
    _(ensures stop motor then PreviousState == PCA_CONT_BOLUS && CurrentState == ERROR_DOOR_STATE when Event == DOOR_OPEN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_CONT_BOLUS iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_CONT_BOLUS iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                //stop_motor();
                                state->CurrentState=OFFSTATE;
                                break;
        case BOLUS:             state->CurrentState=PCA_CONT_LOCKOUT_WAIT;
                                user_input.bolus = true;
                                start_motor(user_input);
                                user_input.bolus = false;
                                pca_cont_lockout_timer.attach(&PCA_cont_lockout,user_input.pca_cont_lockout * 60);
                                break;
        case DOOR_OPEN:         //Display() - Door Opens
                                stop_motor();
                                state->PreviousState=PCA_CONT_BOLUS;
                                state->CurrentState=ERROR_DOOR_STATE;
                                break;
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                stop_motor();
                                state->PreviousState=PCA_CONT_BOLUS;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                stop_motor();       
                                state->PreviousState=PCA_CONT_BOLUS;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                                                                          
                        
    }
}

void PCA_CONT_LOCKOUT_WAIT(TStateMachine    *state, Event input)
{
    _(ensures CurrentState == OFFSTATE iff Event == OFF)
    _(ensures CurrentState == PCA_CONT_BOLUS && start motor when Event == TIMEOUT)
    _(ensures stop motor then PreviousState == PCA_CONT_LOCKOUT_WAIT && CurrentState == ERROR_DOOR_STATE when Event == DOOR_OPEN)
    _(ensures CurrentState == ERROR_SYRINGE_STATE && PreviousState == PCA_CONT_LOCKOUT_WAIT iff Event == SYRINGE_ABSENT)
    _(ensures CurrentState == ERROR_LEVEL_STATE && PreviousState == PCA_CONT_LOCKOUT_WAIT iff Event == LEVEL_LOW)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case TIMEOUT:           //Display() - Ready
                                state->CurrentState=PCA_CONT_BOLUS;
                                break;
        case DOOR_OPEN:         //Display() - Door Opens
                                stop_motor();
                                state->PreviousState=PCA_CONT_LOCKOUT_WAIT;
                                state->CurrentState=ERROR_DOOR_STATE;
                                break;                              
        case SYRINGE_ABSENT:    //Display(2) - Syringe Absent!!
                                stop_motor();                               
                                state->PreviousState=PCA_CONT_LOCKOUT_WAIT;
                                state->CurrentState=ERROR_SYRINGE_STATE;
                                break;
        case LEVEL_LOW:         //Display(3) - Level low!!
                                stop_motor();
                                state->PreviousState=PCA_CONT_LOCKOUT_WAIT;
                                state->CurrentState=ERROR_LEVEL_STATE;
                                break;                                      
    }
}

void ERROR_DOOR_STATE(TStateMachine *state, Event input)
{
    _(ensure CurrentState == OFFSTATE iff Event == OFF)
    _(ensure CurrentState == PreviousState iff syringe placed && level high && Door closed)
    
    switch(input)
    {
        case OFF:           //Display(0) - Blank Screen
                            state->CurrentState=OFFSTATE;
                            break;
        case DOOR_CLOSED:   if(sensorstatus.syringeplaced && sensorstatus.levelhigh) {
                                state->CurrentState=state->PreviousState;
                            }
                            break;
        case DOOR_OPEN:     while(!sensorstatus.doorclosed);
                            break;
    }
}

void ERROR_SYRINGE_STATE(TStateMachine  *state, Event input)
{
    _(ensure CurrentState == OFFSTATE iff Event == OFF)
    _(ensure CurrentState == PreviousState iff syringe placed && level high)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_PRESENT:   if(sensorstatus.levelhigh) {
                                    state->CurrentState=state->PreviousState;
                                }
                                break;                              
    }
}

void ERROR_LEVEL_STATE  (TStateMachine  *state, Event input)
{
    _(ensure CurrentState == OFFSTATE iff Event == OFF)
    _(ensure CurrentState == PreviousState iff syringe placed && level high)
    
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case LEVEL_HIGH:        sensorstatus.levelhigh=true;
                                if(sensorstatus.syringeplaced) {
                                    state->CurrentState=state->PreviousState;
                                }
                                break;                              
    }
}
/* -------------END OF STATE MACHINE ------------------------- */


/* In main function:
        1) initiallize the State Machine
        2) Check initial sensor status
        3) Assign Interrupt service routines to pins.
*/ 

int main()
{

	// also, all of this should be encapsulated in an object representing the device

    _(requires door sensor connected to pin 5 (DOOR_SENSE))
    _(requires syringe sensor connected to pin 6 (SYRINGE_SENSE))
    _(requires level sensor connected to pin 7 (LEVEL_SENSE))
    _(requires On button of keypad connected to pin 8 (ON_BUTTON))
    _(requires Off button of keypad connected to pin 9 (OFF_BUTTON))
    _(requires Back button of keypad connected to pin 10 (NO_BUTTON))
    _(requires Enter button of keypad connected to pin 11 (ENTER_BUTTON))
    _(requires Up button of keypad connected to pin 12 (UP_BUTTON))
    _(requires Down button of keypad connected to pin 13 (DOWN_BUTTON))
    _(requires Bolus input connected to pin 14 (BOLUS_BUTTON))
    _(requires load button of keypad connected to pin 15 (LOAD_BUTTON))
    
    _(ensures sensorstatus.doorclosed == d_c)
    _(ensures sensorstatus.syringeplaced == s_p)
    _(ensures sensorstatus.levelhigh == l_h)
    
    _(ensures CurrentState == {PCA_READY,PCA_IDLE} ==> user_input.mode_selected == 1 
            && user_input.drug_concentration {conc1,conc2,conc3} 
            && user_input.pca_dosage > 0 
            && user_input.pca_lockout >= LockOutDefault)

    _(ensures CurrentState == {CONT_READY} ==> user_input.mode_selected == 2 
            && user_input.drug_concentration {conc1,conc2,conc3} 
            && user_input.cont_rate >= 0) 
            
    _(ensures CurrentState == {PCA_CONT_READY,PCA_CONT_BOLUS, PCA_CONT_LOCKOUT_WAIT} ==> user_input.mode_selected == 3 
            && user_input.drug_concentration {conc1,conc2,conc3}
            && user_input.pca_cont_bolus_dosage > 0 
            && user_input.pca_cont_lockout >= LockOutDefault            
            && user_input.pca_cont_rate >= 0)
    
    wait(2); // Wait for micro-controller pins to initialize after power up
  
    // Sensor pins need to configured as  digital inputs for initial check
    DigitalIn d_c(DOOR_SENSE); //Door sensor
    DigitalIn s_p(SYRINGE_SENSE); // Syringe sensor
    DigitalIn l_h(LEVEL_SENSE); // Level sensor
    

    // Initial check of sensor pins
    if(!d_c)
        sensorstatus.doorclosed=false;
    else
        sensorstatus.doorclosed=true;
    
    if(!s_p)
        sensorstatus.syringeplaced=false;
    else
        sensorstatus.syringeplaced=true;
    if(!l_h) 
        sensorstatus.levelhigh=false;
    else
        sensorstatus.levelhigh=true;
    
    // Sensor pins configured as interrupts after initial check
    InterruptIn Door_closed(DOOR_SENSE);
    InterruptIn Syringe_placed(SYRINGE_SENSE);
    InterruptIn Level_high(LEVEL_SENSE);
    
    // Initialize the state machine to OFF STATE
    pca_state.CurrentState=&OFFSTATE;
    pca_state.PreviousState=&OFFSTATE;
    smart_pca=&pca_state;

    // Assign ISRs to the pins of microcontroller
    Door_closed.rise(&DoorClosed);
    Door_closed.fall(&DoorOpen);
    Syringe_placed.rise(&SyringePlaced);
    Syringe_placed.fall(&SyringeAbsent);
    Level_high.rise(&LevelHigh);
    Level_high.fall(&LevelLow);
    on.rise(&On);
    off.rise(&Off);
    no.rise(&No);
    enter.rise(&Enter);
    up.rise(&Up);
    down.rise(&Down);
    bolus.rise(&Bolus);
    load.rise(&Load);
    
    while(1);
}
