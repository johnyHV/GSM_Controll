/**
 * @file Variable.h
 *
 * @brief Globalne premenne
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#ifndef __VARIABLE_H_
#define __VARIABLE_H_

#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Arduino.h>
#include "HW_cfg.h"
#include "GSM_A6.h"

extern SoftwareSerial mySerial;           // seriova linka k A6 modulu
extern struct SMSmessage IncommingSms;    // globalna premenna pre prichadzajucu SMS
extern String IncommingCallNumber;        // globalna premenna pre prichadzajuci hovor

#endif
