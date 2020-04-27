#include <Arduino.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

#include "HW_cfg.h"
#include "Variable.h"
#include "GSM_A6.h"
#include "System.h"

void setup() {
  
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200 );
  Serial.println(F("MCU start!"));
  Serial.println(SW_VERSION);
  Serial.println(F("Initializing..."));

  SystemInit();
  A6_Init();
  A6_DeleteAllSMS();

  /* tu zapiseme do EEPROM MASTER cislo, ktore moze pridavat/editovat/mazat dalsie tel.c */
  /* MASTER cislo zapiseme iba raz, pri prvej inicializacii */
  /*
  SystemEepromWriteNumber("42190123456",SYSTEM_EEPROM_ADD,SYSTEM_E_NUM_MASTER);
  String numb = "";
  Serial.println(SystemEepromReadNumber(SYSTEM_E_NUM_MASTER,&numb));
  Serial.println(numb);
  */

  wdt_enable (WDTO_8S);
  wdt_reset();

  // Test WDG. odkomentovat a otestovat
  // while(1);
}

void loop() {  
  /* kontrola dat z A6 */
  A6_ReadIncommingData();
  wdt_reset();

  /* kontrola prichadzajuceho hovoru */
  if (A6_CallIncommingCheck(&IncommingCallNumber) == A6_OK) {
    wdt_reset();
    SystemReleControl(IncommingCallNumber);
    IncommingCallNumber = "";
  }
  wdt_reset();

   
  if (A6_SmsIncommingCheck(&IncommingSms) == A6_OK) {
    wdt_reset();
     /* kontrola prichadzajucej SMS pre zoznam cisiel*/
    if (SystemNumberList(&IncommingSms) == true) {
      wdt_reset();
      IncommingSms.type = "";
      IncommingSms.number = "";
      IncommingSms.date = "";
      IncommingSms.message = "";

      A6_DeleteAllSMS();
    }
    wdt_reset();

    wdt_reset();
    /* kontrola prichadzajucej konfiguracnej SMS */
    if (SystemNumberAdd(&IncommingSms) == true) {
      wdt_reset();
      IncommingSms.type = "";
      IncommingSms.number = "";
      IncommingSms.date = "";
      IncommingSms.message = "";

      A6_DeleteAllSMS();
    }
    wdt_reset();
  }
  wdt_reset();
  
  /*while(1) {
    updateSerial();
  }*/
}
