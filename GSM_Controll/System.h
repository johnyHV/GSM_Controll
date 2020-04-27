/**
 * @file System.h
 *
 * @brief Kniznica pre ukladanie tel.c do pamete a samotna logika aplikacie
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <EEPROM.h>
#include "HW_cfg.h"
#include "GSM_A6.h"

#define SYSTEM_MAX_NUMBER         20                  // maximalny pocet tel.c
#define SYSTEM_NUMBER_LENGTH      12                  // dlzka tel.c

#define SYSTEM_RELE_ON            true                // Vypnutie rele
#define SYSTEM_RELE_OFF           false               // Zapnutie rele
#define SYSTEM_RELE_TIMEOUT       1000                // TIMEOUT pre rele

#define SYSTEM_EEPROM_ADD         1                   // zapis cislo do EEPROM
#define SYSTEM_EEPROM_DEL         2                   // vymaz cislo z EEPROM
#define SYSTEM_EEPROM_ENABLE      3                   // cislo zapnute v EEPROM
#define SYSTEM_EEPROM_DISABLE     4                   // cislo vypnute v EEPROM
#define SYSTEM_EEPROM_NOTFOUND    5                   // nenajdene

#define SYSTEM_SMS_OPERATION_LEN  2                   // dlzka znakov v SMS ktore urcuju ci sa cislo zapne, vypne, a atd. Aj s bielym znakom!

void SystemInit();                                    // inicializacia kniznice
void SystemReleControl(String);                       // funkcia zapne/vypne rele na zaklade cisla volajuceho

void SystemReleOnOff(bool);                           // zmeni stav rele

int SystemGetAddress(int);                            // ziska adresu pamete na zaklade indexu
uint8_t SystemCheckEnableNumber(int);                 // iba skontroluje ci je cislo aktivne v danej adrese pameti
bool SystemNumberAdd(SMSmessage *);                   // pridanie cisla z SMS spravy
uint8_t SystemEepromCheckNumber(String);              // skontroluje ci je take cislo v EEPROM pameti, a vrati v akom stave je v pameti ulozene. Enable, Disable, Notfoudn
bool SystemEepromWriteNumber(String, uint8_t, int);   // zapise cislo do EEPROm pamete. vrati status o uspesnosti zapisania cisla
uint8_t SystemEepromReadNumber(int, String*);         // vycita cislo z EEPROM pamete. vrati status cisla v pameti. Enale, Disable...
String SystemEeepromNumberList();                     // vypise zoznam cisiel. Vrati zoznam cisiel v stringu
int SystemEepromGetAddressNumber(String);             // ziska adresu pamete, na ktorej je ulozene konkretne cislo

bool SystemNumberList(SMSmessage *);                  // vypise zoznam cisiel v pameti

#endif
