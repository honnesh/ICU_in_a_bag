#include "mbed.h"
#include "pca.h"

Serial pc(USBTX, USBRX);

/* ------------- ISRs ---------------------------------- */
void DoorClosed()
{
    sensorstatus.doorclosed=true;
    if(smart_pca->CurrentState != ERROR_DOOR_STATE)
        (smart_pca->CurrentState)(smart_pca,DOOR_CLOSED);
}

void DoorOpen()
{
    sensorstatus.doorclosed=false;
    (smart_pca->CurrentState)(smart_pca,DOOR_OPEN);
}

void SyringePlaced()
{
    sensorstatus.syringeplaced=true;
    pc.printf("SYRINGE INSERTED\n\r");
    (smart_pca->CurrentState)(smart_pca,SYRINGE_PRESENT);
}

void SyringeAbsent()
{
    sensorstatus.syringeplaced=false;  
    pc.printf("SYRINGE REMOVED\n\r"); 
    (smart_pca->CurrentState)(smart_pca,SYRINGE_ABSENT);
}

void LevelHigh()
{
    sensorstatus.levelhigh=true;
    pc.printf("LEVEL WENT LOW\n\r");
    (smart_pca->CurrentState)(smart_pca,LEVEL_HIGH);
}

void LevelLow()
{
    sensorstatus.levelhigh=false;
    pc.printf("LEVEL WENT HIGH\n\r");
    (smart_pca->CurrentState)(smart_pca,LEVEL_LOW);
}

void On()
{
    //pc.printf("ON button interrupt detected\n\r");
    (smart_pca->CurrentState)(smart_pca,ON);
}

void Off()
{
    (smart_pca->CurrentState)(smart_pca,OFF);
}

void No()
{
    (smart_pca->CurrentState)(smart_pca,NO);
}

void Enter()
{
    (smart_pca->CurrentState)(smart_pca,ENTER);
}

void Up()
{
    //pc.printf("UP\n\r");
    (smart_pca->CurrentState)(smart_pca,UP);
}

void Down()
{
    //pc.printf("DOWN\n\r");
    (smart_pca->CurrentState)(smart_pca,DOWN);
}

void Bolus()
{
    (smart_pca->CurrentState)(smart_pca,BOLUS);
}

void Load()
{
    (smart_pca->CurrentState)(smart_pca,LOAD);
}

void PCA_lockout()
{
     pca_lockout_timer.detach();
    (smart_pca->CurrentState)(smart_pca,TIMEOUT);
}

void PCA_cont_lockout()
{
     pca_cont_lockout_timer.detach();
    (smart_pca->CurrentState)(smart_pca,TIMEOUT);
}
/* ------------- end ISRs ------------------------------ */

/* ------------- MOTOR FUNCTIONS ----------------------- */
void start_motor(struct user_requirements user_inputs)
{
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
    LED_RED = 0;
    LED_YELLOW = 0;
}
    
/* ------------- end Motor Functions ------------------ */

/* ------------- STATE MACHINE ------------------------- */
void OFFSTATE(TStateMachine *state, Event input)
{
    pc.printf("\n\r");
    switch(input)
    {
        case ON:        if(sensorstatus.doorclosed)
                            pc.printf("DOOR CLOSE\n\r");
                            //Display(1)- pc.printf("DOOR OPEN\n");
                        else if(!sensorstatus.syringeplaced)
                            pc.printf("SYRINGE ABSENT\n\r");
                            //Display(2)- Syringe Absent!!
                        else if(!sensorstatus.levelhigh)
                            pc.printf("LEVEL LOW\n\r");
                            //Display(3)- Level low!!
                        else
                        {
                            //Display(4)- 
                            pc.printf("Welcome to the actual SMART PCA\n\r");
                            //Display(5)- Select concentration menu with 1st option highlighted
                            pc.printf("*CONC1\tCONC2\tCONC3\n\r");
                            state->CurrentState=CONC1;
                        }
                        break;
        default:        break;              
    }               
}

void CONC1(TStateMachine    *state, Event input)
{
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
                                pc.printf(" CONC1\t CONC2\t*CONC3\n\r");
                                state->CurrentState=CONC3;
                                break;
        case DOWN:              //Display(6) - Highlight 2nd option
                                 pc.printf(" CONC1\t*CONC2\t CONC3\n\r");
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

void CONC2(TStateMachine    *state, Event input)
{

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
                                
                                pc.printf("* CONC1\t CONC2\t CONC3\n\r");
                                state->CurrentState=CONC1;
                                break;
        case DOWN:              //Display(7) - Highlight 3rd option
                                pc.printf(" CONC1\t CONC2\t* CONC3\n\r");
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
                                pc.printf(" CONC1\t* CONC2\t CONC3\n\r");
                                state->CurrentState=CONC2;
                                break;
        case DOWN:              //Display(5) - Select concentration menu with 1st option highlighted
                                pc.printf("*CONC1\t CONC2\t CONC3\n\r");
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
    pc.printf("MODE_SELECT_PCA\n\r");
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
    pc.printf("MODE_SELECT_CONT\n\r");
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
    pc.printf("MODE_SELECT_CONT\n\r");
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
    pc.printf("PCA_DOSAGE\n\r");
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
    pc.printf("PCA_LOCKOUT\n\r");
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
    pc.printf("PCA_IDLE\n\r");
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
    pc.printf("PCA_READY\n\r");
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case BOLUS:             //Display() -  Administering Bolus
                                load.rise(NULL);
                                bolus.rise(NULL);
                                start_motor(user_input); //time to be calculated after dosage is entered
                                pca_lockout_timer.attach(&PCA_lockout,user_input.pca_lockout*60);
                                bolus.rise(&Bolus);
                                load.rise(&Load);
                                state->CurrentState=PCA_IDLE;
                                break;
        case LOAD:              //Display() - Loading Dosage
                                load.rise(NULL);
                                bolus.rise(NULL);
                                start_motor(user_input); //time to be calculated after dosage is entered
                                pca_lockout_timer.attach(&PCA_lockout,user_input.pca_lockout*60);
                                bolus.rise(&Bolus);
                                load.rise(&Load);
                                state->CurrentState=PCA_IDLE;
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
    pc.printf("CONT_RATE\n\r");
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
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case DOOR_CLOSED:       //Dislay() - Motor Running
                                start_motor(user_input);
                                //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                //state->CurrentState=CONT_DELIVERY;
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
    switch(input)
    {
        case OFF:           //Display(0) - Blank Screen
                            state->CurrentState=OFFSTATE;
                            break;
        case DOOR_CLOSED:   if(sensorstatus.syringeplaced && sensorstatus.levelhigh)
                            {
                                pc.printf("DOOR CLOSED\n\r");
                                state->CurrentState=state->PreviousState;
                            }
                            break;
        case DOOR_OPEN:     while(!sensorstatus.doorclosed);
                            break;
    }
}

void ERROR_SYRINGE_STATE(TStateMachine  *state, Event input)
{
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case SYRINGE_PRESENT:   if(sensorstatus.levelhigh)
                                {
                                    pc.printf("SYRINGE PLACED\n\r");
                                    state->CurrentState=state->PreviousState;
                                }
                                break;                              
    }
}

void ERROR_LEVEL_STATE  (TStateMachine  *state, Event input)
{
    switch(input)
    {
        case OFF:               //Display(0) - Blank Screen
                                state->CurrentState=OFFSTATE;
                                break;
        case LEVEL_HIGH:        sensorstatus.levelhigh=true;
                                if(sensorstatus.syringeplaced)
                                {
                                    pc.printf("LEVEL HIGH NOW\n\r");
                                    state->CurrentState=state->PreviousState;
                                }
                                break;                              
    }
}
/* -------------END OF STATE MACHINE ------------------------- */
// Function Call to initiallize a State Machine

int main()
{
    
    wait(2);
  
    DigitalIn d_c(p5);
    DigitalIn s_p(p6);
    DigitalIn l_h(p7);
    

    
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
    
    InterruptIn Door_closed(p5);
    InterruptIn Syringe_placed(p6);
    InterruptIn Level_high(p7);
    
   // pc.printf("Hello\n\r");

    not_so_smart_pca.CurrentState=&OFFSTATE;
    not_so_smart_pca.PreviousState=&OFFSTATE;
    smart_pca=&not_so_smart_pca;
    //smart_pca=(*TStateMachine)malloc(1000);
    //pc.printf("Hello2:%s",smart_pca);

    //smart_pca->CurrentState = OFFSTATE;
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
    
    LED_YELLOW = 1;
    LED_RED = 1;
    wait(2);
    LED_YELLOW = 0;
    LED_RED = 0;
    Intrpt = 1;
    pc.printf("Hello\n\r");
    
    while(1);
}
