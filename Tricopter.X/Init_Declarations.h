/**********************************************************
;Asterik Research Electonics
;Variable Initalization Header Templet

**********************************************************/


#ifndef __INIT_DECLARATIONS_H__
#define __INIT_DECLARATIONS_H__

unsigned char uc_test_it;
unsigned char uc_test_it2;
unsigned char   ubBufferIndex;
unsigned char   ubLastBufferIndex;
unsigned char   ubProcessByte;
unsigned char   ubDecipherErrorByte;
unsigned char   ubCRCIndex;
unsigned char   ubDecipherIndex;
unsigned char   ucData2bCRCd;

unsigned int    uiSelectWhichBuffer;
unsigned int    uiHichClockCount;
unsigned int    uiLowClockCount;
unsigned int    uiCountsBetweenGoodClks;
unsigned int    uiHeaderZeroCount;

unsigned int    uiTEstCounts;
unsigned int    uiCRCCheck;

unsigned int   *uiDecipherBufferAddress;
unsigned int   uiHighClkBuffer[255];
unsigned int   uiLowClkBuffer[255];
unsigned int   uiDataBuffer[130];
unsigned int   uiDataBuffer2[130];


unsigned int    uiTempDataStore;
unsigned int    uiRxHeader;             //10bits
unsigned int    uiCountryCode;          //10bits
unsigned int    uiStatusAndApplication; // 1 bit for status and 1 bit of application
unsigned int    uiCRC;                  // 16 bit
unsigned long   ulExtraData;            //24 bits
unsigned long long    ullNationalCode;         // 38 bits

int ADC_RSLT0=0;
int ADC_RSLT1=0;




unsigned int c_Tmr1_Tim = C_TMR1_TIME;

unsigned int ui_Table_Page_Number;
unsigned int ui_Table_Offset_Number;
unsigned int ui_Table_Offset_Number_Working;
unsigned int ui_Table_Offset_Number_End;



unsigned char uc_100uS_Timer;
unsigned char uc_Main_Case_Index;

extern void     init_PWM(void);
extern void     init_ADC(void);
extern void	init_Osc(void);
extern void	init_T1(void);
extern void     init_I2C(void);
//how do I add the assembly file I2C_data
extern void	init_DAC_Comparators(void);

int AccX;
int AccY;
int AccZ;
int Temperature;
int GyroX;
int GyroY;
int GyroZ;

int AccX1;
int AccX2;
int AccY1;
int AccY2;
int AccZ1;
int AccZ2;
int Temperature1;
int Temperature2;
int GyroX1;
int GyroX2;
int GyroY1;
int GyroY2;
int GyroZ1;
int GyroZ2;
int PWMperiod;
int PWMzerocheck;
int PWMmotor1check;
int PWMmotor2check;
int PWMmotor3check;
int PWMmotor4check;
int PWMmotor5check;
int PWMmotor6check;
int PWMmotor1;
int PWMmotor2;
int PWMmotor3;
int PWMmotor4;
int PWMmotor5;
int PWMmotor6;
int motor1=0;
int motor2=0;
int motor3=0;
int motor4=0;
int motor5=0;
int motor6=0;
int AccXOLD=0;
int AccYOLD=0;
int AccZOLD=0;
int TemperatureOLD=0;
int GyroXOLD=0;
int GyroYOLD=0;
int GyroZOLD=0;

#endif
