/**
 * @file GSM_A6.cpp
 *
 * @brief kniznica pre ovladanie GSM modemu A6
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#include "GSM_A6.h"

String A6_IncommingDataBuffer;
bool A6_CallGetNumber;

/**
 *
 * @info Reboot the module by setting the specified pin HIGH, then LOW. The pin should
 * be connected to a P-MOSFET, not the A6's POWER pin.
 * @param none
 * @return none 
 */
void A6_powerCycle() {
    A6_powerOff();

    delay(2000);

    A6_powerOn();

    // Give the module some time to settle.
    Serial.println(F("Done, waiting for the module to initialize..."));
    delay(20000);
    Serial.println(F("Done."));
}

/**
 *
 * @info Turn the modem power completely off.
 * @param none
 * @return none 
 */
void A6_powerOff() {
    digitalWrite(GSM_A6_PWR, LOW);
}

/**
 *
 * @info Turn the modem power on
 * @param none
 * @return none 
 */
void A6_powerOn() {
    digitalWrite(GSM_A6_PWR, HIGH);
}

/**
 *
 * @info Init modem
 * @param none
 * @return none 
 */
bool A6_Init() {
  //Begin serial communication with Arduino and A6
  A6_IncommingDataBuffer = "";
  A6_CallGetNumber = false;
  mySerial.begin(9600);
  
  pinMode(GSM_A6_INT, INPUT);
  pinMode(GSM_A6_PWR, OUTPUT);
  pinMode(GSM_A6_RST, OUTPUT);
  digitalWrite(GSM_A6_RST, LOW);
  A6_powerOn();

  mySerial.println("AT+CREG?");
  byte hi = A6_waitFor("1,", "5,", 1500);  // 1: registered, home network ; 5: registered, roaming
  while ( hi != A6_OK) {
    mySerial.println("AT+CREG?");
    hi = A6_waitFor("1,", "5,", 1500);
  }

  A6_command("AT+CLIP=1", "OK", "ERROR", A6_CMD_TIMEOUT, 2);            // Enable CLIP for incomming call
  A6_command("AT+CMGF=1", "OK", "ERROR", A6_CMD_TIMEOUT, 2);            // Send SMS to text mode
  A6_command("AT+CNMI=1,2,0,0,0", "OK", "ERROR", A6_CMD_TIMEOUT, 2);    // AT Command to receive a live SMS
  A6_command("AT+CMEE=2", "OK", "ERROR", A6_CMD_TIMEOUT, 2);            // enable better error message
  A6_ClearBuffer();

  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);

  delay(1000);
/*
  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
*/  

}

/**
 *
 * @info Send SMS
 * @param String - tel.num
 * @param String - text
 * @return none 
 */
void A6_SendSMS(String tel,String text) {
  Serial.print(F("Sending SMS: "));
  Serial.println(tel);
  Serial.println(text);
  
  A6_command("AT+CMGF=1", "OK", "ERROR", A6_CMD_TIMEOUT, 2);
  
  mySerial.println("AT+CMGS=\"" + tel + "\"");
  updateSerial();
  
  //mySerial.print("Hello, this is a test"); //text content
  mySerial.print(text);
  updateSerial();
  
  mySerial.write(26);
}

/**
 *
 * @info Return the SMS at index.
 * @param int - index
 * @return SMSmessage - sms message
 */
SMSmessage A6_ReadSMS(int index) {
    String response = "";
    char buffer[30];
    SMSmessage sms = (const struct SMSmessage) { "", "", "", "" };

    // cakanie na odpoved.
    sprintf(buffer, "AT+CMGR=%d", index);
    A6_command(buffer, "OK", "ERROR", A6_CMD_TIMEOUT, 2, &response);

    char number[50];
    char date[50];
    char type[10];
    int respStart = 0;

    //vyparsovanie +CLCC.
    respStart = response.indexOf("CMGR:",2);
    if (respStart >= 0) {
        sscanf(response.substring(respStart).c_str(), "CMGR: \"REC %[^\"]\",\"%[^\"]\",,\"%[^\"]\r\n", type, number, date);
        sms.type = String(type);
        sms.number = String(number);
        sms.number.remove(0,1);       // vymazanie +
        sms.date = String(date);
        sms.message = response.substring(respStart + response.substring(respStart).indexOf("\r\n") +2);
        
        Serial.print("SubString: ");
        Serial.println(response.substring(respStart).c_str());
        Serial.print("TYPE: ");
        Serial.println(sms.type);
        Serial.print("Number: " );
        Serial.println(String(sms.number));
        Serial.print("Date: ");
        Serial.println(String(sms.date));
        Serial.print("Text: ");
        Serial.println(sms.message);
    }

    return sms;
}

/**
 *
 * @info Delete the SMS at index.
 * @param int - index
 * @return byte 
 */
byte A6_DeleteSMS(int index) {
    char buffer[20];
    sprintf(buffer, "AT+CMGD=%d", index);
    int tmp = A6_command(buffer, "OK", "ERROR", A6_CMD_TIMEOUT, 2, NULL);
    Serial.print("StatuS: ");
    Serial.println(tmp);
    return tmp;
}

/**
 *
 * @info Delete all SMS in stoage area
 * @param none
 * @return byte 
 */
byte A6_DeleteAllSMS() {
    int tmp = A6_command("AT+CMGD=1,4", "OK", "ERROR", A6_CMD_TIMEOUT, 2, NULL);
    Serial.print("StatuS: ");
    Serial.println(tmp);
    return tmp;
}

/**
 *
 * @info Check incomming SMS
 * @param SMSmessage - incomming SMS message
 * @return bool - status 
 */
bool A6_SmsIncommingCheck(SMSmessage *sms) {
  bool ret = A6_NOTOK;
  int i_sms = 0;
  int r_sms = 0;

  if (A6_IncommingDataBuffer.length() > 3) {
    i_sms = A6_IncommingDataBuffer.indexOf("CMT: ");
    r_sms = A6_IncommingDataBuffer.indexOf("CMGS: ");
    Serial.println("SMSIncommingBuffer: ");
    Serial.println(A6_IncommingDataBuffer);
  }

  //+CMGS: 1
  if (r_sms > 0) {
    // prisla mi odpoved na odoslanu SMS. vymazem buffer.
    if (A6_IncommingDataBuffer.indexOf("OK") > 0) {
      A6_ClearBuffer();
    }
  }

  if (i_sms > 0) {
    Serial.println("Found SMS");
    delay(A6_CMD_TIMEOUT);
    A6_ReadIncommingData();

    char number[50];
    char date[50];
    sscanf(A6_IncommingDataBuffer.substring(i_sms).c_str(), "CMT: \"%[^\"]\",,\"%[^\"]\"\r\n", number, date);
    sms->message = A6_IncommingDataBuffer.substring(i_sms + A6_IncommingDataBuffer.substring(i_sms).indexOf("\r\n") +2);
    sms->number = String(number);
    sms->number.remove(0,1);       // vymazanie +
    sms->date = String(date);

    Serial.print(F("Number: "));
    Serial.println(sms->number);
    Serial.print(F("Date: "));
    Serial.println(sms->date);
    Serial.print(F("Text: "));
    Serial.println(sms->message);

    A6_ClearBuffer();
    ret = A6_OK;
  }

  return ret;
}

/**
 *
 * @info End call
 * @param none
 * @return none
 */
void A6_CallEnd() {
  mySerial.println("ATH");
  updateSerial();
}

/**
 *
 * @info Check incomming call
 * @param String - tel.n 
 * @return bool - status
 */
bool A6_CallIncommingCheck(String *number) {

  bool ret = A6_NOTOK;
  int ring = 0;
  // pozriem sa, ci je buffer nenulovy
  if (A6_IncommingDataBuffer.length() > 3) {
    ring = A6_IncommingDataBuffer.indexOf("RING",2);
    Serial.println(F("CallIncommingBuffer: "));
    Serial.println(A6_IncommingDataBuffer);
  }
  
  // Skontrolujem ci mi prisiel RING
  if (ring > 0) {
    Serial.println(F("Found RING"));

    // Dalej skontrolujem ci som prijal ake cislo mi vola, alebo sa to mam spytat
    int pos = A6_IncommingDataBuffer.indexOf("+CLCC:",2);
    int pos_c = A6_IncommingDataBuffer.indexOf("+CLIP:",2);

    // kontrola ci viem ake cislo mi vola
    if ((pos > 0) || (pos_c > 0)) {
      
      // skontrolujem, ci mam cely ramec aj s cilom, a vytiahnem poziciu kde je cislo
      int pos_n = 0;
      if (pos > 0) {
        Serial.println(F("Found CLCC"));
        if (A6_IncommingDataBuffer.length() > (pos + 32))
          pos_n = A6_IncommingDataBuffer.indexOf("\"",pos);
        else
          return A6_NOTOK;
      } else if (pos_c > 0) {
        Serial.println(F("Found CLIP"));
        if (A6_IncommingDataBuffer.length() > (pos_c + 22))
          pos_n = A6_IncommingDataBuffer.indexOf("\"",pos_c);
        else
          return A6_NOTOK;
      }

      A6_CallEnd(); // ukoncim hovor

      // idem vytiahnut cislo 
      // +CLCC: 1,1,4,0,0,"42190123456",145
      // +CLIP: "42190123456",145,,,,1
      String cislo = "";
      for (int i=0;i<20;i++) {
        pos_n++;
        if (A6_IncommingDataBuffer[pos_n] != 0x22) {
          cislo += A6_IncommingDataBuffer[pos_n];
        } else {
          break;
        }
      }
      Serial.print(F("Number: "));
      *number = String(cislo);
      Serial.println(*number);
      
      delay(5000);
      A6_ReadIncommingData();
      A6_CallGetNumber = false;
      A6_ClearBuffer();
      ret = A6_OK;
      
    } else if (A6_CallGetNumber != true) {
      // ak som naprijal CLIP, tak potom sa spytam na cislo
      Serial.println(F("CLCC request"));
      mySerial.println("AT+CLCC");
      A6_CallGetNumber = true;
    }
  }

  return ret;
}

/**
 *
 * @info resetne modem. SKONTROLOVAT
 * @param none
 * @return bool - status
 */
bool A6_ResetModem() {
  digitalWrite(GSM_A6_RST, HIGH);
  delay(5000);
  digitalWrite(GSM_A6_RST, LOW);
  delay(500);
  
  if (A6_Init() != A6_OK) {
    Serial.println(F("Error"));
    return A6_NOTOK;
  }

  return A6_OK;
}

/**
 *
 * @info vymazem buffer s prijatymi datami
 * @param none
 * @return none
 */
void A6_ClearBuffer() {
  A6_IncommingDataBuffer = "";
}

/**
 *
 * @info vycitam ake data mi prisli a ulozim ich
 * @param none
 * @return none
 */
void A6_ReadIncommingData() {
  while (mySerial.available()) {
    A6_IncommingDataBuffer += (char) mySerial.read();
  }

  delay(100);
}

/**
 *
 * @info caka kym nepride bud prva alebo druha odpoved, do timeoutu
 * @param String - response1
 * @param String - response2
 * @param int - Timeout
 * @return bool - status
 */
byte A6_waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  String reply = A6_read();
  byte retVal = 99;
  
  do {
    reply = A6_read();
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && ((millis() - entry) < timeOut ));

  if ((millis() - entry) >= timeOut) {
    retVal = A6_TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) 
      retVal = A6_OK;
    else 
      retVal = A6_NOTOK;
  }
  return retVal;
}

/**
 *
 * @info caka kym nepride bud prva alebo druha odpoved, do timeoutu  + prida odpoved
 * @param String - response1
 * @param String - response2
 * @param int - Timeout
 * @param String - returned data from GSM
 * @return bool - status
 */
byte A6_waitFor(const char *resp1, const char *resp2, int timeout, String *response) {
    unsigned long entry = millis();
    String reply = "";
    byte retVal = 99;
    do {
        reply += A6_read();
    } while (((reply.indexOf(resp1) + reply.indexOf(resp2)) == -2) && ((millis() - entry) < timeout));

    if (reply != "") {
        Serial.print(F("Reply in "));
        Serial.print((millis() - entry));
        Serial.print(F(" ms: "));
        Serial.println(reply);
    }
    if (response != NULL) {
        *response = reply;
    }
    if ((millis() - entry) >= timeout) {
        retVal = A6_TIMEOUT;
        Serial.println(F("Timed out."));
    } else {
        if (reply.indexOf(resp1) + reply.indexOf(resp2) > -2) {
            Serial.println(F("Reply OK."));
            retVal = A6_OK;
        } else {
            Serial.println(F("Reply NOT OK."));
            retVal = A6_NOTOK;
        }
    }
    return retVal;
}

/**
 *
 * @info vycita prijate data
 * @param none
 * @return String - read
 */
String A6_read() {
  String reply = "";
  if (mySerial.available()) {
    reply = mySerial.readString();
    Serial.println(reply);
  }  
  
  return reply;
}

/**
 *
 * @info odosle prikaz, a caka na odpoved s timeoutom, a poctom opakovani, kolko krat sa moze pokusit command poslat
 * @param String - command
 * @param String - response1
 * @param String - response2
 * @param int - Timeout
 * @param int - repetitions 
 * @return bool - status
 */
byte A6_command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = A6_NOTOK;
  byte count = 0;
  while ((count < repetitions) && (returnValue != A6_OK)) {
    mySerial.println(command);
    Serial.print(F("Command: "));
    Serial.println(command);
    
    if (A6_waitFor(response1, response2, timeOut) == A6_OK) {
      returnValue = A6_OK;
    } else {
      returnValue = A6_NOTOK;
    }
    
    count++;
  }
  
  return returnValue;
}

/**
 *
 * @info odosle prikaz, a caka na odpoved s timeoutom, a poctom opakovani, kolko krat sa moze pokusit command poslat
 * @param String - command
 * @param String - response1
 * @param String - response2
 * @param int - Timeout
 * @param int - repetitions 
 * @param String - returned data from GSM
 * @return bool - status
 */
byte A6_command(const char *command, const char *resp1, const char *resp2, int timeout, int repetitions, String *response) {
    byte returnValue = A6_NOTOK;
    byte count = 0;

    // Get rid of any buffered output.
    mySerial.flush();

    while (count < repetitions && returnValue != A6_OK) {
        Serial.print(F("Issuing command: "));
        Serial.println(command);

        mySerial.write(command);
        mySerial.write('\r');

        if (A6_waitFor(resp1, resp2, timeout, response) == A6_OK) {
            returnValue = A6_OK;
        } else {
            returnValue = A6_NOTOK;
        }
        count++;
    }
    return returnValue;
}

/* redirect seriovej konzoly */
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
