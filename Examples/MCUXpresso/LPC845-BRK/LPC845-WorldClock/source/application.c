/*
 * Copyright (c) 2019, Erich Styger
  *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#include "application.h"
#include "McuWait.h"
#include "McuRTOS.h"
#if PL_CONFIG_USE_BUTTON
  #include "buttons.h"
#endif
#include "leds.h"
#if PL_CONFIG_USE_SHELL
  #include "McuShell.h"
  #include "Shell.h"
#endif
#include "McuTimeDate.h"
#if PL_CONFIG_USE_STEPPER
  #include "stepper.h"
#endif
#if PL_CONFIG_USE_MAG_SENSOR
  #include "magnets.h"
#endif
#include "fsl_iap.h"
#if PL_CONFIG_IS_SERVER
 #include "matrix.h"
#endif

static bool clockIsOn = false;
#define APP_DEFAULT_DELAY  (5)

#if PL_CONFIG_USE_STEPPER
static void MoveAllToZeroPosition(void) {
  /* move back to zero position */
  for(int i=0; i<STEPPER_NOF_CLOCKS; i++) {
    STEPPER_MoveClockDegreeAbs(i, STEPPER_HAND_HH, 0, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
    STEPPER_MoveClockDegreeAbs(i, STEPPER_HAND_MM, 0, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
  }
  STEPPER_MoveAndWait(10);
}
#endif /* PL_CONFIG_USE_STEPPER */

#if PL_CONFIG_USE_STEPPER
static void DemoMakeTwelve(void) {
  MoveAllToZeroPosition();
}

static void DemoMakeSquare(void) {
  /* build a square with the hands: 0 index */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true); /* lower right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_MM, 270, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_HH, 270, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true); /* upper right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_HH,  90, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true); /* lower left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_MM,   0, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_HH, 180, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true); /* upper left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_MM,  90, STEPPER_MOVE_MODE_SHORT, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveAndWait(50);

  for(int i=0; i<360; i++) { /* motor 0 is inner shaft (HH) */
    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_HH, 360-i, STEPPER_MOVE_MODE_CCW, APP_DEFAULT_DELAY, true, true); /* lower right */
    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_MM, 270+i, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);

    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_HH, 270-i, STEPPER_MOVE_MODE_CCW, APP_DEFAULT_DELAY, true, true); /* upper right */
    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_MM, 180+i, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);

    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_HH, 90-i,  STEPPER_MOVE_MODE_CCW, APP_DEFAULT_DELAY, true, true); /* lower left */
    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_MM, 0+i,   STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);

    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_HH, 180-i, STEPPER_MOVE_MODE_CCW, APP_DEFAULT_DELAY, true, true); /* upper left */
    STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_MM, 90+i,  STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
    STEPPER_MoveAndWait(5);
  }
}

static void DemoMakePropeller(void) {
  /* build lines */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* lower right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* upper right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* lower left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* upper left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveAndWait(50);
  for(int i=0; i<360; i++) { /* motor 0 is inner shaft (HH) */
    for(int c=0; c<STEPPER_NOF_CLOCKS; c++) { /* move each by 1 degree */
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_HH, 1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_MM, 1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
    }
    STEPPER_MoveAndWait(5);
  }
}

static void DemoMakeClap(void) {
  /* build lines */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* lower right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_0, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* upper right */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_1, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* lower left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_2, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_HH,   0, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true); /* upper left */
  STEPPER_MoveClockDegreeAbs(STEPPER_CLOCK_3, STEPPER_HAND_MM, 180, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveAndWait(50);
  /* clap to the right */
  for(int i=0; i<90; i++) {
    for(int c=0; c<STEPPER_NOF_CLOCKS; c++) { /* move each by 1 degree */
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_HH, 1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_MM, -1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
    }
    STEPPER_MoveAndWait(5);
  }
  /* clap to the left */
  for(int i=0; i<180; i++) {
    for(int c=0; c<STEPPER_NOF_CLOCKS; c++) { /* move each by 1 degree */
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_HH, -1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_MM, 1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
    }
    STEPPER_MoveAndWait(5);
  }
  /* back to starting position */
  for(int i=0; i<90; i++) {
    for(int c=0; c<STEPPER_NOF_CLOCKS; c++) { /* move each by 1 degree */
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_HH, 1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
      STEPPER_MoveClockDegreeRel(c, STEPPER_HAND_MM, -1, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
    }
    STEPPER_MoveAndWait(5);
  }
}

static void SetTime(STEPPER_Clock_e clock, uint8_t hour, uint8_t minute) {
  int32_t angleHour, angleMinute;

  minute %= 60; /* make it 0..59 */
  hour %= 12; /* make it 0..11 */
  angleMinute = (360/60)*minute;
  angleHour = (360/12)*hour + ((360/12)*minute)/60;
  STEPPER_MoveClockDegreeAbs(clock, STEPPER_HAND_HH, angleHour, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
  STEPPER_MoveClockDegreeAbs(clock, STEPPER_HAND_MM, angleMinute, STEPPER_MOVE_MODE_CW, APP_DEFAULT_DELAY, true, true);
}

static void ShowTime(STEPPER_Clock_e clock, uint8_t hour, uint8_t minute) {
  SetTime(clock, hour, minute);
  STEPPER_MoveAndWait(5);
}
#endif /* PL_CONFIG_USE_STEPPER */

#if PL_CONFIG_USE_SHELL
static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  status_t res;
  uint32_t val;
  uint8_t buf[64];
  uint32_t id[4];

  McuShell_SendStatusStr((unsigned char*)"app", (unsigned char*)"Application settings\r\n", io->stdOut);
#if PL_CONFIG_IS_SERVER
  McuShell_SendStatusStr((unsigned char*)"  Mode", (unsigned char*)"server\r\n", io->stdOut);
#else
  McuShell_SendStatusStr((unsigned char*)"  Mode", (unsigned char*)"client\r\n", io->stdOut);
#endif
  McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"V");
  McuUtility_strcatNum8u(buf, sizeof(buf), PL_CONFIG_BOARD_VERSION/10);
  McuUtility_chcat(buf, sizeof(buf), '.');
  McuUtility_strcatNum8u(buf, sizeof(buf), PL_CONFIG_BOARD_VERSION%10);
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((unsigned char*)"  board", buf, io->stdOut);

  McuShell_SendStatusStr((unsigned char*)"  clock", clockIsOn?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);

  res = IAP_ReadPartID(&val);
  if (res == kStatus_IAP_Success) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
    McuUtility_strcatNum32Hex(buf, sizeof(buf), val);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  PartID", buf, io->stdOut);

  res = IAP_ReadUniqueID(&id[0]);
  if (res == kStatus_IAP_Success) {
    buf[0] = '\0';
    for(int i=0; i<sizeof(id)/sizeof(id[0]); i++) {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"0x");
      McuUtility_strcatNum32Hex(buf, sizeof(buf), id[i]);
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)" ");
    }
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  UID", buf, io->stdOut);

  res = IAP_ReadBootCodeVersion(&val);
  if (res == kStatus_IAP_Success) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"Version 0x");
    McuUtility_strcatNum32Hex(buf, sizeof(buf), val);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  BootCode", buf, io->stdOut);

  return ERR_OK;
}

static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"app", (unsigned char*)"Group of application commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  clock on|off", (unsigned char*)"Enable or disable the clock\r\n", io->stdOut);
#if PL_CONFIG_USE_STEPPER
  McuShell_SendHelpStr((unsigned char*)"  time <c> <time>", (unsigned char*)"Show time on clock (0..3)\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  demo twelve", (unsigned char*)"Move pointer to 12\r\n", io->stdOut);
#endif
  McuShell_SendHelpStr((unsigned char*)"  demo square", (unsigned char*)"Demo with square\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  demo propeller", (unsigned char*)"Demo with propeller\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  demo clap", (unsigned char*)"Demo like a snapper\r\n", io->stdOut);
#if PL_CONFIG_WORLD_CLOCK
  McuShell_SendHelpStr((unsigned char*)"  clocks", (unsigned char*)"[0] London    [3] New York\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"",         (unsigned char*)"[1] Beijing   [2] Lucerne\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  hands",  (unsigned char*)"[0] MM,inner  [1] HH,outer\r\n", io->stdOut);
#endif
  return ERR_OK;
}

uint8_t APP_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
#if PL_CONFIG_USE_STEPPER
  const unsigned char *p;
#endif

  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "app help")==0) {
    *handled = true;
    return PrintHelp(io);
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "app status")==0)) {
    *handled = true;
    return PrintStatus(io);
  } else if (McuUtility_strcmp((char*)cmd, "app clock on")==0) {
    *handled = true;
    clockIsOn = true;
  } else if (McuUtility_strcmp((char*)cmd, "app clock off")==0) {
    *handled = true;
    clockIsOn = false;
#if PL_CONFIG_USE_STEPPER
  } else if (McuUtility_strcmp((char*)cmd, "app demo twelve")==0) {
    *handled = TRUE;
    DemoMakeTwelve();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "app demo square")==0) {
    *handled = TRUE;
    DemoMakeSquare();
  } else if (McuUtility_strcmp((char*)cmd, "app demo propeller")==0) {
    *handled = TRUE;
    DemoMakePropeller();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "app demo clap")==0) {
    *handled = TRUE;
    DemoMakeClap();
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, "app time ", sizeof("app time ")-1)==0) {
    uint8_t hour, minute, second, hsec;
    int32_t val;

    *handled = TRUE;
    p = cmd + sizeof("app time ")-1;
    if (
           McuUtility_xatoi(&p, &val)==ERR_OK && val>=0 && val<=3
        && McuUtility_ScanTime(&p, &hour, &minute, &second, &hsec)==ERR_OK
       )
    {
      ShowTime(val, hour, minute);
    } else {
      return ERR_FAILED;
    }
    return ERR_OK;
#endif /* PL_CONFIG_USE_STEPPER */
  }
  return ERR_OK;
}
#endif

#if PL_CONFIG_USE_STEPPER
static uint8_t AdjustHourForTimeZone(uint8_t hour, int8_t gmtDelta) {
  int h;

  h = (int)hour+gmtDelta;
  if (h<0) {
    h = 24+h;
  }
  h %= 24;
  return h;
}
#endif

static void AppTask(void *pv) {
#if PL_CONFIG_USE_RTC
  uint8_t oldHH=-1, oldMM = -1;
  TIMEREC time;
  uint8_t res;
#endif

  PL_InitFromTask();
#if PL_CONFIG_USE_SHELL
#if PL_CONFIG_IS_SERVER
  SHELL_SendString((unsigned char*)"\r\n*****************\r\n* LPC845 Master *\r\n*****************\r\n");
#endif
#if PL_CONFIG_IS_CLIENT
  SHELL_SendString((unsigned char*)"\r\n*****************\r\n* LPC845 Client *\r\n*****************\r\n");
#endif
#endif
#if 0 && PL_CONFIG_USE_STEPPER
  if (STEPPER_ZeroAllHands()!=ERR_OK) {
    SHELL_SendString((unsigned char*)"FAILED to zero all hands!\r\n");
  }
#endif
#if 1
  //DemoRotateSquare();
  //DemoMakeSquare();
  //DemoMakePropeller();
  //DemoMakeClap();
  //DemoMakeTwelve();
#endif
#if PL_CONFIG_USE_STEPPER
  if (clockIsOn) {
    vTaskDelay(pdMS_TO_TICKS(5000)); /* just some delay */
  }
#endif
  for(;;) {
    vTaskDelay(pdMS_TO_TICKS(200));
#if PL_CONFIG_USE_RTC
    if (clockIsOn) {
      res = McuTimeDate_GetTime(&time);
      if (res==ERR_OK && (time.Hour!=oldHH || time.Min != oldMM)) {
  #if PL_CONFIG_WORLD_CLOCK
        uint8_t hour;

        hour = AdjustHourForTimeZone(time.Hour, -1); /* local time is GMT+1 */
        SetTime(STEPPER_CLOCK_0, AdjustHourForTimeZone(hour, 0), time.Min);  /* London, GMT+0, top left */
        SetTime(STEPPER_CLOCK_1, AdjustHourForTimeZone(hour, 8), time.Min);  /* Beijing, GMT+8, top left */
        SetTime(STEPPER_CLOCK_2, AdjustHourForTimeZone(hour, 1), time.Min);  /* Lucerne, GMT+1, top left */
        SetTime(STEPPER_CLOCK_3, AdjustHourForTimeZone(hour, -4), time.Min); /* New York,, GMT-4, top left */
        STEPPER_MoveAndWait(5);
  #endif
        oldHH = time.Hour;
        oldMM = time.Min;
      }
    }
#endif
    McuLED_Toggle(LEDS_Led);
  }
}

void APP_Run(void) {
	PL_Init();
  if (xTaskCreate(
      AppTask,  /* pointer to the task */
      "App", /* task name for kernel awareness debugging */
      600/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY+2,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS) {
     for(;;){} /* error! probably out of memory */
  }
  vTaskStartScheduler();
  for(;;) { /* should not get here */ }
}
