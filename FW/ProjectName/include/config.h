#ifndef __CONFIG_H
#define __CONFIG_H

#include <Arduino.h>

#define _STRINGIFY(x)      #x
#define STRINGIFY(x)      _STRINGIFY(x)

#define VERSION                BUILD_DATETIME
#define VERSIONSTR_LONG        STRINGIFY(GIT_BRANCH-VERSION)
#define VERSIONSTR_SHORT       STRINGIFY(VERSION)

#define DEBUG
#ifdef DEBUG
	// Enable DBG(...) globally:
	// #define TOOLS_LOG_DEBUG

	// Or per file:
	#define DEBUG_MAIN_CPP

	// Specific parts:
#endif

// PINS
#define PIN_MOSI				GPIO_NUM_12
#define PIN_MISO				GPIO_NUM_13
#define PIN_SCLK				GPIO_NUM_14

#define PIN_BTN_A				GPIO_NUM_34
#define PIN_BTN_B 				GPIO_NUM_35
#define PIN_BTN_C 				GPIO_NUM_36

#endif // __CONFIG_H

