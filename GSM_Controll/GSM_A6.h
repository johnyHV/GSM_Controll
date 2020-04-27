/**
 * @file GSM_A6.h
 *
 * @brief kniznica pre ovladanie GSM modemu A6
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#ifndef __GSM_A6_H_
#define __GSM_A6_H_

#include "Variable.h"
#include "HW_cfg.h"

#define A6_OK 0                       // status pre OK
#define A6_NOTOK 1                    // status pre NOT OK
#define A6_TIMEOUT 2                  // status pre timeout
#define A6_FAILURE 3                  // status pre failure

#define A6_CMD_TIMEOUT 2000           // standardny timeout pre GSM A6

enum A6_call_direction {
    DIR_OUTGOING = 0,
    DIR_INCOMING = 1
};

enum A6_call_state {
    CALL_ACTIVE = 0,
    CALL_HELD = 1,
    CALL_DIALING = 2,
    CALL_ALERTING = 3,
    CALL_INCOMING = 4,
    CALL_WAITING = 5,
    CALL_RELEASE = 7
};

enum A6_call_mode {
    MODE_VOICE = 0,
    MODE_DATA = 1,
    MODE_FAX = 2,
    MODE_VOICE_THEN_DATA_VMODE = 3,
    MODE_VOICE_AND_DATA_VMODE = 4,
    MODE_VOICE_AND_FAX_VMODE = 5,
    MODE_VOICE_THEN_DATA_DMODE = 6,
    MODE_VOICE_AND_DATA_DMODE = 7,
    MODE_VOICE_AND_FAX_FMODE = 8,
    MODE_UNKNOWN = 9
};

struct SMSmessage {
    String type;      // typ spravy. READ, UNREAD....
    String number;    // cislo odosielatela
    String date;      // datum prijatia SMS
    String message;   // sprava
};

extern String A6_IncommingDataBuffer;                                               // globalny buffer pre prijimanie dat z GSM pocas behu loop-u
extern bool A6_CallGetNumber;                                                       // 

void A6_powerCycle();                                                               // reset GSM modulu
void A6_powerOff();                                                                 // vypnutie GSM modulu
void A6_powerOn();                                                                  // zapnutie GSM modulu

bool A6_Init();                                                                     // initializacia GSM modulu

void A6_SendSMS(String, String);                                                    // odoslanie SMS
SMSmessage A6_ReadSMS(int);                                                         // vycitanie SMS z pamete 
byte A6_DeleteSMS(int);                                                             // vymazanie SMS z pamete
byte A6_DeleteAllSMS();                                                             // asi vymazanie vsetkych sms z pamete https://www.developershome.com/sms/cmgdCommand.asp
bool A6_SmsIncommingCheck(SMSmessage*);                                             // kontrola ci neprisla SMS pocas behu programu loop-u

void A6_CallEnd();                                                                  // ukoncenie prichadzajuceho hovoru
bool A6_CallIncommingCheck(String*);                                                // kontrola ci niekto prave nevola
bool A6_ResetModem();                                                               // reset modemu ?

void A6_ReadIncommingData();                                                        // Globalna funkcia pre live prijimanie dat pocas behu loop-u
String A6_read();                                                                   // citanie dat teraz a hned
void A6_ClearBuffer();                                                              // vymazanie buffra
byte A6_waitFor(String, String, int);                                               // cakanie na odpoved prvu, alebo druhu, do timeout
byte A6_waitFor(const char *, const char *, int , String *);                        // cakanie na prvu alebo druhu odpoved s timeout, a vrati co prijal
byte A6_command(String, String, String, int, int);                                  // odoslanie prikaz, s cakanim na prvu, alebo druhu odpoved, s timeoutom, poctom opakovani
byte A6_command(const char *, const char *, const char *, int , int , String *);    // odoslanie prikaz, s cakanim na prvu, alebo druhu odpoved, s timeoutom, poctom opakovani, a vrati to co prislo

void updateSerial();                                                                // serial redirect z medzi USB a modemo

#endif

// https://wiki.dfrobot.com/Gravity__UART_A6_GSM_&_GPRS_Module_SKU__TEL0113
// https://github.com/skorokithakis/A6lib
// Call - ATD0900123456
// End call - ATH
// Start incoming call ATA
// At+clip=1
// get number during ring at+clcc

// AT&F0  Reset to factory settings
// ATE0  disable Echo
// AT+CMEE=2 enable better error messages

// https://www.developershome.com/sms/cmgdCommand.asp
// https://www.developershome.com/sms/cmgrCommand.asp
// https://www.developershome.com/sms/readSmsByAtCommands.asp
// https://www.developershome.com/sms/cpmsCommand.asp
// AT+CMGR=1 vycitanie prvej SMS
// AT+CMGL="ALL" vycitanie vsetkych SMS 
// AT+CMGF=1 set sms to text mode
// AT+CNMI=1,2,0,0,0 AT Command to receive a live SMS
// +CMT: "+421900123456",,"2020/04/10,19:57:08+02"
