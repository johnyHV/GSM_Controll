/**
 * @file System.cpp
 *
 * @brief Kniznica pre ukladanie tel.c do pamete a samotna logika aplikacie
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#include "System.h"

/**
 *
 * @info Inicializacia rele a vstupov
 * @param none
 * @return none
 */
void SystemInit() {
  pinMode(RELE_A, OUTPUT);
  pinMode(RELE_B, OUTPUT);
  pinMode(INPUT_A, INPUT);
  
  SystemReleOnOff(SYSTEM_RELE_OFF);

  Serial.println(F("Number list: "));
  Serial.println(SystemEeepromNumberList());
}

/**
 *
 * @info kontrola, ci dane cislo moze ovladat RELE
 * @param String - tel.n
 * @return none
 */
void SystemReleControl(String num) {
  wdt_reset();
  Serial.print(F("SystemRele: "));
  Serial.println(num);
  
 if (SystemEepromCheckNumber(num) == SYSTEM_EEPROM_ENABLE) {
  Serial.println(F("ENABLE rele!"));
  SystemReleOnOff(SYSTEM_RELE_ON);
  delay(SYSTEM_RELE_TIMEOUT);
  Serial.println(F("DISABLE rele!"));
  SystemReleOnOff(SYSTEM_RELE_OFF);
 } else {
  Serial.println(F("Invalid tel.n!"));
 }
 wdt_reset();
}

/**
 *
 * @info Zapinanie, vypinanie RELE
 * @param bool - status
 * @return none
 */
void SystemReleOnOff(bool stat) {
  
  if (stat == SYSTEM_RELE_ON) {
    digitalWrite(RELE_A,HIGH);
    digitalWrite(RELE_B,LOW);
  } else if (stat == SYSTEM_RELE_OFF) {
    digitalWrite(RELE_A,LOW);
    digitalWrite(RELE_B,HIGH);
  }
}

/**
 *
 * @info kontrola, ci dane cislo moze ovladat RELE
 * @param none
 * @return none
 */
uint8_t SystemCheckEnableNumber(int addr) {
  wdt_reset();
  int stat = EEPROM.read(addr);
  int ret = SYSTEM_EEPROM_DISABLE;
  
  if (stat == SYSTEM_EEPROM_ENABLE) {
    ret = SYSTEM_EEPROM_ENABLE;
  } else if (stat == SYSTEM_EEPROM_DISABLE) {
    ret = SYSTEM_EEPROM_DISABLE;
  } else {
    ret = SYSTEM_EEPROM_DISABLE;
  }

  return ret;
}

/**
 *
 * @info ziska adresu pamete na zaklade indexu
 * @param int - Index
 * @return int - address
 */
int SystemGetAddress(int index){
  wdt_reset();
  int ret = SYSTEM_EEPROM_NOTFOUND;
  
  if (index > SYSTEM_MAX_NUMBER)
    ret = SYSTEM_EEPROM_NOTFOUND;
    
  switch (index) {
    case 0:
      ret = SYSTEM_E_NUM_MASTER;
      break;
    case 1:
      ret = SYSTEM_E_NUM1_ADD;
      break;
    case 2:
      ret = SYSTEM_E_NUM2_ADD;
      break;
    case 3:
      ret = SYSTEM_E_NUM3_ADD;
      break;
    case 4:
      ret = SYSTEM_E_NUM4_ADD;
      break;
    case 5:
      ret = SYSTEM_E_NUM5_ADD;
      break;
    case 6:
      ret = SYSTEM_E_NUM6_ADD;
      break;
    case 7:
      ret = SYSTEM_E_NUM7_ADD;
      break;
    case 8:
      ret = SYSTEM_E_NUM8_ADD;
      break;
    case 9:
      ret = SYSTEM_E_NUM9_ADD;
      break;
    case 10:
      ret = SYSTEM_E_NUM10_ADD;
      break;
    case 11:
      ret = SYSTEM_E_NUM11_ADD;
      break;
    case 12:
      ret = SYSTEM_E_NUM12_ADD;
      break;
    case 13:
      ret = SYSTEM_E_NUM13_ADD;
      break;
    case 14:
      ret = SYSTEM_E_NUM14_ADD;
      break;
    case 15:
      ret = SYSTEM_E_NUM15_ADD;
      break;
    case 16:
      ret = SYSTEM_E_NUM16_ADD;
      break;
    case 17:
      ret = SYSTEM_E_NUM17_ADD;
      break;
    case 18:
      ret = SYSTEM_E_NUM18_ADD;
      break;
    case 19:
      ret = SYSTEM_E_NUM19_ADD;
      break;
    case 20:
      ret = SYSTEM_E_NUM20_ADD;
      break;
  }
  return ret;
}

/**
 *
 * @info  pridanie cisla z SMS spravy
 * @param SMSmessage - sms message
 * @return bool - status
 */
bool SystemNumberAdd(SMSmessage *sms) {
  wdt_reset();
  String master = "";
  SystemEepromReadNumber(SYSTEM_E_NUM_MASTER, &master);

  if (sms->number.equals(master)) {
    Serial.println(F("Configure SMS from master"));

    /* kontrola ci je to dostatocne dlhe cislo */
    if (sms->message.length() < SYSTEM_NUMBER_LENGTH + SYSTEM_SMS_OPERATION_LEN) {
        return false;
    }

    /* vytiahnutie cisla */
    Serial.print(F("Number: "));
    String num = "";
    for (int i=SYSTEM_SMS_OPERATION_LEN ;i < (SYSTEM_NUMBER_LENGTH + SYSTEM_SMS_OPERATION_LEN); i++) {
      num += sms->message[i];
    }
    Serial.println(num);
    
    /* kontrola ci chcem pridat, alebo zmazat cislo */
    if ((sms->message[0] == 'A') || (sms->message[0] == 'a')) {
      /* pridat cislo */
      Serial.println(F("Add number"));
      int adresa = 0;

      for (int i = 1;i <= SYSTEM_MAX_NUMBER; i++) {
        if (SystemCheckEnableNumber(SystemGetAddress(i)) == SYSTEM_EEPROM_DISABLE) {
          adresa = SystemGetAddress(i);
          break;
        }
      }

      /*Serial.print("Address: ");
      Serial.println(adresa);*/
      if (adresa != 0) {
        SystemEepromWriteNumber(num, SYSTEM_EEPROM_ADD, adresa);
        return true;
      }
      
    } else if ((sms->message[0] == 'D') || (sms->message[0] == 'd')) {
      /* zmazat cislo */
      Serial.println(F("Delete number"));
      uint8_t stat = 0;
      int adresa = 0;
      stat = SystemEepromCheckNumber(num);

      if (stat != SYSTEM_EEPROM_NOTFOUND) {
        adresa = SystemEepromGetAddressNumber(num);
        SystemEepromWriteNumber(num, SYSTEM_EEPROM_DEL, adresa);
      }
      
      return true;
    }
  }
  return false;
}

/**
 *
 * @info skontroluje ci je take cislo v EEPROM pameti, a vrati v akom stave je v pameti ulozene. Enable, Disable, NotFoudn
 * @param String - tel.n
 * @return uint8_t - index
 */
uint8_t SystemEepromCheckNumber(String i_num) {
  wdt_reset();
  Serial.print("Locate num: ");
  Serial.println(i_num);
  bool ret = false;
  int addr = 0;
  int stat = 0;
  
  for (int i=0;i<SYSTEM_MAX_NUMBER;i++) {
    String num = "";
    addr = SystemGetAddress(i);
    stat = SystemEepromReadNumber(addr, &num);
    
    if (stat == SYSTEM_EEPROM_ENABLE) {
      if (i_num.equals(num)) {
        /*Serial.print("Locate: ");
        Serial.print(addr);
        Serial.print(" - ");
        Serial.println(num);*/
        return stat;
      }
    }
  }
  return SYSTEM_EEPROM_NOTFOUND;
}

/**
 *
 * @info zapise cislo do EEPROm pamete. vrati status o uspesnosti zapisania cisla
 * @param String - tel.n
 * @param uint8_t - operation ADD/DEL
 * @param int - EEPROM addr
 * @return uint8_t - status
 */
bool SystemEepromWriteNumber(String num, uint8_t stat, int addr) {
  wdt_reset();
/*
  Serial.print("Write: ");
  Serial.println(num);
  Serial.print("stat: ");
  Serial.println(stat);
  Serial.print("Addr: ");
  Serial.println(addr);
*/
  /* nemoznost vymazat MASTER cislo */
  if ((addr == SYSTEM_E_NUM_MASTER) && (stat == SYSTEM_EEPROM_DEL)) {
    return false;
  }

  /* kontrola dlzky cisla */
  if (num.length() < SYSTEM_NUMBER_LENGTH) {
    Serial.println(F("Number is too short"));
    return false;
  }

  /* kontrola operacie */
  if (stat == SYSTEM_EEPROM_ADD ) {
    EEPROM.write(addr,SYSTEM_EEPROM_ENABLE);
    addr++;
    for (int i=0;i<SYSTEM_NUMBER_LENGTH;i++,addr++) {
      EEPROM.write(addr,num[i]);
    }
    
    return true;
  } else if (stat == SYSTEM_EEPROM_DEL ) {
    EEPROM.write(addr,SYSTEM_EEPROM_DISABLE);
    addr++;
    for (int i=0;i<SYSTEM_NUMBER_LENGTH;i++,addr++) {
      EEPROM.write(addr,255);
    }

    return true;
  }

  return false;
}

/**
 *
 * @info vycita cislo z EEPROM pamete. vrati status cisla v pameti. Enale, Disable...
 * @param int - EEPROM addr
 * @param String* - tel.n
 * @return uint8_t - index
 */
uint8_t SystemEepromReadNumber(int addr, String *num) {
  wdt_reset();
  String number = "";
  int adresa = addr;
  int ret = SYSTEM_EEPROM_DISABLE;

  uint8_t stat = EEPROM.read(adresa);

  if (stat == SYSTEM_EEPROM_ENABLE) {
    adresa++;
    for (int i=adresa;i < (adresa + SYSTEM_NUMBER_LENGTH);i++) {
      number += (char) EEPROM.read(i);
    }
    ret = SYSTEM_EEPROM_ENABLE;
  }
  *num = number;
/*
  Serial.println("Read:");
  Serial.print("stat: ");
  Serial.println(stat);
  Serial.print("Addr: ");
  Serial.println(addr);
  Serial.print("Num: ");
  Serial.println(*num);
*/
  return ret;
}

/**
 *
 * @info vypise zoznam cisiel. Vrati zoznam cisiel v stringu
 * @param none
 * @return String - tel.n list
 */
String SystemEeepromNumberList() {
  wdt_reset();
  String output = "";

  int addr = 0;
  int stat = 0;
  for (int i = 0;i <= SYSTEM_MAX_NUMBER; i++) {
    String num = "";
    addr = SystemGetAddress(i);
    stat = SystemEepromReadNumber(addr, &num);
    
    if (stat == SYSTEM_EEPROM_DISABLE) {
      output += "D ";
    } else if (stat == SYSTEM_EEPROM_ENABLE) {
      output += "A ";
    }

    output += num;
    output += '\n';
  }

  return output;
}

/**
 *
 * @info ziska adresu pamete, na ktorej je ulozene konkretne cislo
 * @param String - tel.n
 * @return int - EEPROM addr
 */
int SystemEepromGetAddressNumber(String i_num) {
  wdt_reset();
  Serial.print(F("Locate Address tel.n: "));
  Serial.println(i_num);
  int addr = 0;
  int stat = 0;
  
  for (int i=0;i<SYSTEM_MAX_NUMBER;i++) {
    String num = "";
    addr = SystemGetAddress(i);
    stat = SystemEepromReadNumber(addr, &num);
    
    if (stat == SYSTEM_EEPROM_ENABLE) {
      if (i_num.equals(num)) {
        /*Serial.print("Locate: ");
        Serial.print(addr);
        Serial.print(" - ");
        Serial.println(num);*/
        return addr;
      }
    }
  }
  return SYSTEM_EEPROM_NOTFOUND;
}

/**
 *
 * @info zisti, ci prisla poziadavka pre zoznam cisiel v pameti
 * @param SMSmessage - SMS
 * @return bool - status
 */
bool SystemNumberList(SMSmessage *sms) {
  wdt_reset();
  int pos = sms->message.indexOf("zoznam");
  int c_pos = sms->message.indexOf("Zoznam");

  if ((pos >= 0) || (c_pos >=0)) {
    String master = "";
    SystemEepromReadNumber(SYSTEM_E_NUM_MASTER, &master);

    if (sms->number.equals(master)) {
      Serial.println(F("Request number list from master"));
      String text = SystemEeepromNumberList();
      A6_SendSMS(master, text);
      return true;
    }
  }

  return false;
}
