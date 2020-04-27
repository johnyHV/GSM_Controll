/**
 * @file Variable.cpp
 *
 * @brief Globalne premenne
 *
 * @author Miroslav Pivovarsky
 * Contact: miroslav.pivovarsky@gmail.com
 * 
 * @bug: no know bug
 *
 */

#include "Variable.h"

SoftwareSerial mySerial(GSM_A6_TX, GSM_A6_RX); 
struct SMSmessage IncommingSms = { "", "", "", "" };
String IncommingCallNumber;
