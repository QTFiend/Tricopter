/**********************************************************
;WE Bots
;Main

**********************************************************/
/*NOTES
 Max Heli Accel (2/4/8/16g):  4m = (1/2)a*(.5)^2  --> a = 32m/s^2 = 3.26g.. round to 8g
 Max Heli Gyro (250/500/1000/2000 deg/s): 3960deg/5s = 792dps.............. round to 1000dps
 * Make sure all blades spin in lifting direction with positive values
 * Change time constant depending on sampling rate (see filter.pdf)
 * Are we using a camera? path planning?
 * What does the accel/gyro return? Convert to distances and angles
 * Add AccXdeg, AccXdegEff and GyroXdeg into Interrupt files
 * MAKE PWM
 * create stdio, math, stdlib.h files?
 * check directions of all acc/dir, and whether they should be x or y
 * delay: (http://www.programmingsimplified.com/c/dos.h/delay)
 * */

#include <TIME.H>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Micros_h_File.h"
#include "Init_Declarations.h"
#include "Configutation_Bits_File.h"


/*********************************************************
 * PORT DESIGNATIONS
 * Motor Signals: RB2 - RB7
 * Accel/gyro: RB8(SCL), RB9(SDA), RA4(INT)
 * LEDs: RA2, RA3
 * Programming: RB0(PGD1), RB1(PGC1), VSS, VDD, MCLR
 * Test pins: AVDD, RA0, RA1, RB13, VSS
 * Bluetooth: AVDD, VSS, RB11, RB12
 * Master power: RB15
 * BAT low: RB14   batLow=high=1?
 * Motor Power: RB10
 ********************************************************
 *           X
            1,2
             |
             |
      Y<-----O------
            / \
        5,6/   \3,4 
 */

//TRIS DEFINE STATEMENTS
#define TA00     TRISAbits.TRISA0 //
#define TA01     TRISAbits.TRISA1 //
#define TA02     TRISAbits.TRISA2 //
#define TA03     TRISAbits.TRISA3 //
#define TA04     TRISAbits.TRISA4 //
#define TB00     TRISBbits.TRISB0 //
#define TB01     TRISBbits.TRISB1 //
#define TB02     TRISBbits.TRISB2 //
#define TB03     TRISBbits.TRISB3 //
#define TB04     TRISBbits.TRISB4 //
#define TB05     TRISBbits.TRISB5 //
#define TB06     TRISBbits.TRISB6 //
#define TB07     TRISBbits.TRISB7 //
#define TB08     TRISBbits.TRISB8 //
#define TB09     TRISBbits.TRISB9 //
#define TB10    TRISBbits.TRISB10 //
#define TB11    TRISBbits.TRISB11 //
#define TB12    TRISBbits.TRISB12 //
#define TB13    TRISBbits.TRISB13 //
#define TB14    TRISBbits.TRISB14 //
#define TB15    TRISBbits.TRISB15 //

//LAT DEFINE STATEMENTS
#define LA00     LATAbits.LATA0 //
#define LA01     LATAbits.LATA1 //
#define LA02     LATAbits.LATA2 //
#define LA03     LATAbits.LATA3 //
#define LA04     LATAbits.LATA4 //
#define LB00     LATBbits.LATB0 //
#define LB01     LATBbits.LATB1 //
#define LB02     LATBbits.LATB2 //
#define LB03     LATBbits.LATB3 //
#define LB04     LATBbits.LATB4 //
#define LB05     LATBbits.LATB5 //
#define LB06     LATBbits.LATB6 //
#define LB07     LATBbits.LATB7 //
#define LB08     LATBbits.LATB8 //
#define LB09     LATBbits.LATB9 //
#define LB10    LATBbits.LATB10 //
#define LB11    LATBbits.LATB11 //
#define LB12    LATBbits.LATB12 //
#define LB13    LATBbits.LATB13 //
#define LB14    LATBbits.LATB14 //
#define LB15    LATBbits.LATB15 //
#define numGoalsRow 2; //Number of set goals
#define numGoalsColumn 6;
//VARIABLE DEFINITIONS
typedef struct {int xPos, yPos, zPos, xAng, yAng, zAng;}SpaceAbs;
SpaceAbs spaceAbs;

typedef struct {int xPosD, yPosD, zPosD, xAngD, yAngD, zAngD;}SpaceDes;
SpaceDes spaceDes;

typedef struct {int xPosR, yPosR, zPosR, xAngR, yAngR, zAngR;}SpaceRel;
SpaceRel spaceRel;

typedef struct {int xPosIn, yPosIn, zPosIn, xAngIn, yAngIn, zAngIn;}Input;
Input input;

typedef struct {int xPosCal, yPosCal, zPosCal, xAngCal, yAngCal, zAngCal;}SpaceCal;
SpaceCal spaceCal;

typedef struct {int Xacc, Yacc, Zacc;}Acc_Linear;
Acc_Linear acc_Linear;

typedef struct {int Xvel, Yvel, Zvel;}Vel_Linear;
Vel_Linear vel_Linear;

typedef struct {int Xpos, Ypos, Zpos;}Pos_Linear;
Pos_Linear pos_Linear;

typedef struct {int Xaddpos, Yaddpos, Zaddpos;}Addpos_Linear;
Addpos_Linear addpos_Linear;

typedef struct {int Xaddvel, Yaddvel, Zaddvel;}Addvel_Linear;
Addvel_Linear addvel_Linear;

typedef struct {int Xvel, Yvel, Zvel;}Vel_Rotation;
Vel_Rotation vel_Rotation;

typedef struct {int Xpos, Ypos, Zpos;}Pos_Rotation;
Pos_Rotation pos_Rotation;

typedef struct {int Xaddpos, Yaddpos, Zaddpos;}Addpos_Rotation;
Addpos_Rotation addpos_Rotation;

unsigned int motSpeed[6];
unsigned int batLow;
int ms;
int distToGround;
int motor1;
int motor2;
int motor3;
int motor4;
int motor5;
int motor6;
int dt=0;
int dt1=0;
int dt2=0;
int clockinit;

int motorStable[6] = {0,0,0,0,0,0};
int AccXdeg, AccYdeg, AccZdeg, AccXdegEff, AccYdegEff, AccZdegEff;
int GyroXdeg, GyroYdeg, GyroZdeg;
int Pi = 3.141592654;

//SET GOALS
int goal[2][6];
//volatile unsigned int goal[0][6]; //set in calibrate method

//volatile unsigned int goal1[6] = {};

//DECLARE FUNCTIONS
void initialize(void);
void test(void);
void calibrate(void);
void go(void);
int camSensor(void);
void flatten(void);
void land(void);
void ascend(int height, int speed);
void descend(void);
void incAll(void);
void decAll(void);
void setMot(void);
void retMot(void);
void I2CData_to_Pos (void);
void I2CData_to_Orientation(void);
void stall(int stalltime);
void pwmtest (void);
//void delay(int);



/************************************************/
//CALIBRATION CONSTANTS
int testing;        //1=testing 0=actual !!! was 1
int calibrating;    //1=calibrating 0=actual !!!was 1
int halfSpeed;     //motor half speed !!was 30
//theoretical flat offsets of 32768
//Accel sensitivity: 4096 bits per g
//Gyro Sensitivity:  32.8 bits per dps
//Complementary Filter using small angle (<30deg) approximation
//Option 1: Let Acc set upon start, need a perfectly flat table
int AccX0;
int AccY0;
int AccZ0;
/*Option 2: Let Acc calibrate itself
 * int AccX0 = 32768;
 * int AccY0 = 32768;
 * int AccZ0 = 32768;
 */
//int AccSens = 4096;
//Gyro values set themselves on startup
int GyroX0;
int GyroY0;
int GyroZ0;
//int GyroSens = 32.768;
//PD Control: non:specific
//int Kp = 5; //Constant Kp
//int Kd = 3; //Constant Kd
/* PD Control: specific
 * int KpAccX;
 * int KpAccY;
 * int KpAccZ;
 * int KpGyroX;
 * int KpGyroY;
 * int KpGyroZ;
 * int KdAccX;
 * int KdAccY;
 * int KdAccZ;
 * int KdGyroX;
 * int KdGyroY;
 * int KdGyroZ; */
//int GyroCoeff = .98;

//Z-Spin offsets between each motor of a pair
int motor12Offset;
int motor34Offset;
int motor56Offset;
//Speeds of each motor pair to hover, offsets between pairs of motors
int motor12Hover;
int motor34Hover;
int motor56Hover;
int AccST;          //Acc/Gyro SensitivityTime (seconds), to prevent vibrating !!! was 1
int pathDiv;        //Acceptable divergence from path
/************************************************/


//Main: Initialize, Testing, Start
int main()
{
    goal [1][0]= 5; //position 1
    goal [1][1]= 3; //position 2
    goal [1][2]= -2; //position 3
    goal [1][3]= 0; //position 4
    goal [1][4]= 6; //position 5
    goal [1][5]= -10; //position 5
//    int AccCoeff = 1-GyroCoeff;
//Initialize registers and ports
    //Pressing button gives power to micro and starts it
    initialize();
    
    //do nothing for certain time (in seconds) so people can get away
    stall(3);
    calibrate();

    //set LED brightness relative to the motor speed
    pwmtest();
    /*
    if (testing) {
       int i=0;
        for (i=0; i < 6; i++) {
            motors[i] = halfSpeed;
            //Sleep(4000); This puts the micro to sleep - instead count the number of times the interrupt happens
            motors[i] = 0;
           // Sleep(4000);
        }
    } else{
        go();
    }
    */
    I2CData_to_Pos();
    I2CData_to_Orientation();
    return 0;
}


//Set buffers etc.
void initialize(void){
  //WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
     //   ubBufferIndex = 0;
     //   ubLastBufferIndex = 0;

	init_Osc();
	init_PWM();								// PWM Setup
	init_ADC();								// ADC Setup
	init_DAC_Comparators();
        init_T1();
        init_I2C();

    //initialize the old accel/gyro value
    AccXOLD=AccX;
    AccYOLD=AccY;
    AccZOLD=AccZ;
    TemperatureOLD=Temperature;
    GyroXOLD=GyroX;
    GyroYOLD=GyroY;
    GyroZOLD=GyroZ;
    
	//TB04 = 0;                 //Set RB4 as output
        //LB04 = 1; high            //Set RB4 as high

        // PTCONbits.PTEN = 1;                   // Enable the PWM
//	ADCONbits.ADON = 1;                 // Enable the ADC

     //   uiHeaderZeroCount = 0;
     //	IEC0bits.T1IE = 1;
//	uc_Main_Case_Index = 0;
/*
  //P1DIR = 0xFF; //(nov 13th)0xFF;           // All P1.x outputs
  P1OUT = 0;                                // All P1.x reset
  P2DIR = 0xFF;                             // All P2.x outputs
  P2OUT = 0;                                // All P2.x reset
  P1SEL = BIT1 + BIT2;                      //(nov 13th) add bit 0/ACLK                 // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
  P3DIR = 0xFF;                             // All P3.x outputs
  //P3REN = 0x00;                           // Set all Pulldown resistors to prevent random button pushes
  P3OUT = 0;                                // All P3.x reset

//BUTTON INITIALIZATION
  P1OUT |= 8;
  P1REN |= 8;
  P1DIR &= ~8;

//Clock Initialization
  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32kHz/9600 = 3.41
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = UCBRS1 + UCBRS0;               // Modulation UCBRSx = 3
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

//TIMER INITIALIZATION >>Timer resets cause interrupt
  TA0CCR0 = 62500 - 1;                      // A period of 62,500 cycles is 0 to 62,499.
  TA0CCTL0 = CCIE;                          // Enable interrupts for CCR0.
  TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;  // SMCLK, div 8, up mode,


//ENABLE INTERRUPTS (IN 'LOW POWER MODE 3')
  IFG2 &= ~UCA0RXIFG;                       //Clear the flag first to prevent immediate interrupt
  IE2 |= UCA0RXIE;// + UCA0TXIE;            // Enable USCI_A0 RX TX or timer interrupt....CLEAR FLAGS FIRST?
  _enable_interrupt();                      // Enable interrupts
  //__bis_SR_register(LPM3_bits + GIE);     // Enter LPM3 w/ int until Byte RXed
  

*/
}

//Set Acc/Gyro Reference Values

void calibrate(void){

    //Is it being calibrated with a camera or laser? Set to a perfect straight line?
    //Option 1: Let Accelerometer find its own level, ACCURACY?
    //Would you get an accelerometer value if stationary on ground?
    AccX0 = AccX;
    AccY0 = AccY;
    AccZ0 = AccZ;    
    GyroX0 = GyroX;
    GyroY0 = GyroY;
    GyroZ0 = GyroZ;
    //goal[0][0]= xPos;
    //goal[0][1]= yPos;
    //goal[0][2]= zPos;
    //goal[0][3]= xAng;
    //goal[0][4]= yAng;
    //goal[0][5]= zAng;
    
    //find axis perpendicular to earth ground with tricopter nonmoving
    //z accn= gravity, no x or y accn
    //z accn= sqrt[AccX*(g/LSB)^2 + AccY^2 + AccZ^2]
    //acc_Linear.Zacc= AccX*

    /*
    int Xabs = 0;
    int Yabs = 0;
    int Zabs = 0;
    //in degrees
    int aXabs = 0;
    int aYabs = 0;
    int aZabs = 0;
    //in feet
    int Xref = 0;
    int Yref = 0;
    int Zref = 0;
    //in degrees
    int aXref = 0; //tilt down
    int aYref = 0; //tilt left
    int aZref = 0;
*/
}

//Flight Sequence
/*void go(void) {
    //STEP ONE: Start sequence (w or w/o button) to start
    //ALT: Just use fact that button turns on power
    //while(buttonHiLo);

    //STEP TWO: Wait 3 seconds for clearance
    //Sleep(3000);

    //STEP THREE: Save start position and orientation
    if(calibrating){
        calibrate();
        //store in memory
    }*/
    /*STEP FOUR: Rise to 2 inches
        i.   slowly increase each motor "simultaneously"
        ii.  if acc/gyr is not level, increase other motors correspondingly
                -parabolic velocity graph
        iii. stop and hover at 2 inches
     */
  /*  ascend(.2, 2); //rise to 0.2' at a speed of 2 (1-5)
    stabilize(.2);

    //STEP FIVE: Wait 5 seconds to stabilize
    //Sleep(5000);

    //STEP SIX: Go to first destination point, stop 5 seconds, repeat for next goals
    //for (int i(0); i < numGoals; i++) {   //cycle through goals, first "goal" is calibration step
      //  for (int j = 0; j < 6; j++) {
        //    if(i = 0){
          //goal[0][i];
            }else{
            //Take 6 inputs from accelerometer and gyro into input[]
            //Create 6 coordinate system
            //Does gyro and accelerometer measure relative or absolute values?? Relative?
            spaceRel[j] = input[j];
            spaceAbs[j] = spaceAbs[j] + spaceRel[j];
            }
        }

        //Desired position/orientation
       // goal[][];
        //??calculate the forces and torques on the helicopter based on previous values
        //use PID to establish how much to to pulse motor
        setMot(1,)
        //tell motor 1 what to do
        TB02 = 0;
        //tell motor 2 what to do
        TB03 = 0;
        //tell motor 3 what to do
        TB04 = 0;
        //tell motor 4 what to do
        TB05 = 0;
        //tell motor 5 what to do
        TB06 = 0;
        //tell motor 6 what to do
        TB07 = 0;
        //tell Red LED what to do
        //RA3
        //tell Green LED what to do
        //RA2

        if (batLow) {
            land();
        }
        //do something if battery low (land) or very low (shut off)
*/
        /* Future stuff */
        //Vision System
        //Object Detection
 /*   }

    //STEP SEVEN: Land safely
    land();
}
*/
//Read Camera to detect ground, obstacles, USB
/*
int camSensor(void){
    //read camera sensor
    return distToGround;
}
*/
/*
//Hover in place, PD Control
void stabilize(int height){
    //STEP ONE: Stabilize in X and Y
    //Use a Complementary Filter: Gyro = angVel, Acc+Gyro = angle
    int enterLoop = 0;
    int stabilized = 0;
    while(((xPos-xPosOld) != 0) && ((yPos-yPosOld) != 0) && (enterLoop == 1) && (stabilized == 0){

        //Angle Read from Accelerometer
        AccXdeg = (AccX-AccX0)*180/(AccSens*9.81*Pi); //angle in degrees
        AccYdeg = (AccY-AccY0)*180/(AccSens*9.81*Pi); //angle in degrees
        //AccZdeg = (AccZ-AccZ0)*180/(AccSens*9.81*Pi); //angle in degrees
        GyroXdeg =(GyroX-GyroX0)/GyroSens; //velocity in degrees/second
        GyroYdeg =(GyroY-GyroY0)/GyroSens; //velocity in degrees/second
        //GyroZdeg =(GyroZ-GyroZ0)/GyroSens; //velocity in degrees/second

        //Initial AccXdegEff value
        if(enterLoop == 0){
            AccXdegEff = AccXdeg;
            AccYdegEff = AccYdeg;
            //AccZdegEff = AccZdeg;
        }
        //Effective Angles: Filter quick Acc readings
        AccXdegEff = GyroCoeff * (AccXdegEff - GyroYdeg * dt) + AccCoeff * (AccXdeg);
        AccYdegEff = GyroCoeff * (AccYdegEff - GyroXdeg * dt) + AccCoeff * (AccYdeg);
        //AccZdegEff = GyroCoeff * (AccZdeg + GyroZdeg * dt) + AccCoeff * (AccZdeg);

        //Simplification: Have Motors12 controlX, have Motors34 control Y
        Motor1 = motor12Hover + Kp * AccXdegEff - Kd * GyroYdeg;
        Motor2 = Motor1;
        Motor3 = motor34Hover + Kp * AccYdegEff - Kd * GyroXdeg;
        Motor4 = Motor3;
        //Have all motors control Z
        while(zPos < height){
            ascend(height);
        }
        while(zPos > height){
            descend(height);
        }
        enterLoop = 1;
    }//blink red

    //STEP TWO: Stabilize in Z
    //Ideally, only have to change Motor12Offset for all turning.
    while((GyroZ != GyroZ0)&& stabilized == 0){
        motor12Offset = Kd * (GyroZ0-GyroZ);
        Motor1 += motor12Offset;
        Motor2 -= motor12Offset;
    }
    //blink green //blinks green if fully stabilized

    //RECORD STABLE VALUES
    if (stabilized == 0) {
        for (int k(0); k < 6; k++) {
            motorStable[k] = motors[k];
        }
        stabilized = 1;
    }else{
        for (int k(0); k < 6; k++) {
            motorStable[k] = motors[k];
        }
    }

}
*/
/*
void land(void)
{
    //check speed
    //check acceleration
    //check orientation
    //space[] = checkPos();
    flatten();
    descend(-1); //Descend to ground

}
*/
//Ascend using PD Control
/*
void ascend(int height, int speed)
{     //speed is a number 1-5, 5 being fastest
    while(zPos < height)
    {
        Motor1 += speed;
        Motor2 += speed;
        Motor3 += speed;
        Motor4 += speed;
        Motor5 += speed;
        Motor6 += speed;
        delay (1sec)
    }
}
*/
//**************Descend using PD Control
/*
void descend(int height, int speed){ //descends at constant speed to height
    while(zPos > height)
    {
        Motor1 += speed;
        Motor2 += speed;
        Motor3 += speed;
        Motor4 += speed;
        Motor5 += speed;
        Motor6 += speed;
        delay(1000)
    }
}
 */
/*
void incAll(int speed){
    if(retMot(1)<speed){
        M1++;
    }
    if(retMot(2)<speed){
        M2++;
    }
    if(retMot(3)<speed){
        M3++;
    }
    if(retMot(4)<speed){
        M4++;
    }
    if(retMot(5)<speed){
        M5++;
    }
    if(retMot(6)<speed){
        M6++;
    }
}
*/
/*
void decAll(int speed){
    if(retMot(1)>speed){
        M1--;
    }
    if(retMot(2)>speed){
        M2--;
    }
    if(retMot(3)>speed){
        M3--;
    }
    if(retMot(4)>speed){
        M4--;
    }
    if(retMot(5)>speed){
        M5--;
    }
    if(retMot(6)>speed){
        M6--;
    }
}
 */
/*
void SetMotorSpeed(int motNum, motSpeed){
   // motSpeed[motNum-1] = motSpeed; //Arithmetic to determine what is sent to motor
}
*/
/*
int retMot(int motNum){
   // return motSpeed[motNum-1];
}
*/
/*void delay(int delTime){
    for(int z(0); z<delTime; z++){
        //delay
    }
}*/

//3D Motion Tracking
//Initialize all at 0 on start
void I2CData_to_Pos(void)
{
    if (clockinit==0)
    {
        dt1=clock()/1000000;
        clockinit=1;
    }
    dt2=clock()/1000000;
    dt=(dt2-dt1);
    dt1=dt2;

    //AFS_SEL=2;
    //get the acceleration in m/s^2
    acc_Linear.Xacc= ((AccX/4096)-8)*9.80665;
    acc_Linear.Yacc= ((AccY/4096)-8)*9.80665; 
    acc_Linear.Zacc= ((AccZ/4096)-8)*9.80665; 
    
    //get the added velocity
    addvel_Linear.Xaddvel= (acc_Linear.Xacc)*dt;
    addvel_Linear.Yaddvel= (acc_Linear.Yacc)*dt;
    addvel_Linear.Zaddvel= (acc_Linear.Zacc)*dt;
    
    //get velocity in m/s
    vel_Linear.Xvel= (vel_Linear.Xvel+addvel_Linear.Xaddvel);
    vel_Linear.Yvel= (vel_Linear.Yvel+addvel_Linear.Yaddvel);
    vel_Linear.Zvel= (vel_Linear.Zvel+addvel_Linear.Zaddvel);

    //get the added position
    addpos_Linear.Xaddpos= (vel_Linear.Xvel)*dt;
    addpos_Linear.Yaddpos= (vel_Linear.Yvel)*dt;
    addpos_Linear.Zaddpos= (vel_Linear.Zvel)*dt;

    pos_Linear.Xpos=(pos_Linear.Xpos+addpos_Linear.Xaddpos);
    pos_Linear.Ypos=(pos_Linear.Ypos+addpos_Linear.Yaddpos);
    pos_Linear.Zpos=(pos_Linear.Zpos+addpos_Linear.Zaddpos);




    
    /*
    //in feet
    int Xabs;
    int Yabs;
    int Zabs;
    //in degrees

    //in feet
    int Xref;
    int Yref;
    int Zref;
    //in degrees*
    int aXref = AccXdeg; //tilt down
    int aYref = AccYdeg; //tilt left
    int aZref = aZref + GyroZdeg * dt; //CCW rotation

*/
}

void I2CData_to_Orientation(void)
{
   /*may need to update dt b/w acc and gyro
        if (clockinit==0)
    {
        dt1=clock()/1000000;
        clockinit=1;
    }
    dt2=clock()/1000000;
    dt=(dt2-dt1);
    dt1=dt2;
    */

    //FS_SEL=2;
    //get the angular velocity in degrees/s
    vel_Rotation.Xvel= ((GyroX/32.8)-1000);
    vel_Rotation.Yvel= ((GyroY/32.8)-1000);
    vel_Rotation.Zvel= ((GyroZ/32.8)-1000);

    addpos_Rotation.Xaddpos= (vel_Rotation.Xvel)*dt;
    addpos_Rotation.Yaddpos= (vel_Rotation.Yvel)*dt;
    addpos_Rotation.Zaddpos= (vel_Rotation.Zvel)*dt;

    pos_Rotation.Xpos=(pos_Rotation.Xpos+addpos_Rotation.Xaddpos);
    pos_Rotation.Ypos=(pos_Rotation.Ypos+addpos_Rotation.Yaddpos);
    pos_Rotation.Zpos=(pos_Rotation.Zpos+addpos_Rotation.Zaddpos);
}

void stall (int stalltime)
{
 int clockwait1=clock();
    int clockwait2=clock();
    while ((clockwait2-stalltime*1000)<clockwait1)
    {
       motor1=0;
       motor2=0;
       motor3=0;
       motor4=0;
       motor5=0;
       motor6=0;
       clockwait2=clock();
    }
}

void pwmtest(void)
{
    int i=0;
    for (i=0; i<100; i++)
    {
        motor1 =i;
        motor2 =i;
        stall(.2);
    }
}