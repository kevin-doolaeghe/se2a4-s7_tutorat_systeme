#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>

#include "Descriptors.h"
#include "Lib_Manette.h"

#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

/* Type Defines: */
/** Type define for the joystick HID report structure, for creating and sending HID reports to the host PC.
 *  This mirrors the layout described to the host in the HID report descriptor, in Descriptors.c.
 */
typedef struct
{
	int8_t  X; /**< Current absolute joystick X position, as a signed 8-bit integer */
	int8_t  Y; /**< Current absolute joystick Y position, as a signed 8-bit integer */
	int8_t  Z; /**< Current absolute joystick Z position, as a signed 8-bit integer */
	uint8_t Button; /**< Bit mask of the currently pressed joystick buttons */
} USB_JoystickReport_Data_t;

unsigned char bt_up[]		= { 0x00, 0x00, 0x00, 0x00, 0x01 };
unsigned char bt_left[]		= { 0x00, 0x00, 0x00, 0x00, 0x02 };
unsigned char bt_right[]	= { 0x00, 0x00, 0x00, 0x00, 0x10 };
unsigned char bt_down[] 	= { 0x00, 0x00, 0x00, 0x00, 0x20 };
unsigned char bt_tir[]		= { 0x80, 0x00, 0x00, 0x00, 0x00 };
unsigned char bt_joy[]		= { 0x20, 0x00, 0x00, 0x00, 0x00 };

unsigned char led1[]        = { 0x00, 0x00, 0x01, 0x00, 0x00 };
unsigned char led2[]        = { 0x00, 0x00, 0x02, 0x00, 0x00 };
unsigned char led3[]        = { 0x00, 0x00, 0x04, 0x00, 0x00 };
unsigned char led4[]        = { 0x00, 0x00, 0x08, 0x00, 0x00 };
unsigned char led5[]        = { 0x00, 0x00, 0x20, 0x00, 0x00 };
unsigned char led6[]        = { 0x00, 0x00, 0x10, 0x00, 0x00 };
unsigned char led7[]        = { 0x10, 0x00, 0x00, 0x00, 0x00 };
unsigned char led8[]        = { 0x40, 0x00, 0x00, 0x00, 0x00 };
unsigned char led9[]        = { 0x00, 0x40, 0x00, 0x00, 0x00 };
unsigned char led10[]       = { 0x00, 0x80, 0x00, 0x00, 0x00 };
unsigned char led11[]       = { 0x00, 0x00, 0x00, 0x04, 0x00 };

unsigned char buzzer1[]     = { 0x00, 0x00, 0x40, 0x00, 0x00 };
unsigned char buzzer2[]		= { 0x00, 0x00, 0x80, 0x00, 0x00 };

unsigned char *leds[]       = { led1, led2, led3, led4, led5, led6, led7, led8, led9, led10, led11 };
unsigned char *buzzers[]	= { buzzer1, buzzer2 };

/* Macros: */
/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY      LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

/* Function Prototypes: */
void SetupHardware(void);
void Led_Task(void);
void Buzzer_Task(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
											uint8_t* const ReportID,
											const uint8_t ReportType,
											void* ReportData,
											uint16_t* const ReportSize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
											const uint8_t ReportID,
											const uint8_t ReportType,
											const void* ReportData,
											const uint16_t ReportSize);

void ProcessLedReport(uint16_t LedReport);
void ProcessBufferReport(uint8_t BuzzerReport);
void Manette_Init(void);

#endif

