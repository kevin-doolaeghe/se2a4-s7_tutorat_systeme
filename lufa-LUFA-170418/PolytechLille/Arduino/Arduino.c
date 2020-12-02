#include "Arduino.h"

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevJoystickHIDReportBuffer[sizeof(USB_JoystickReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Joystick_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Joystick,
				.ReportINEndpoint             =
					{
						.Address              = JOYSTICK_IN_EPADDR,
						.Size                 = JOYSTICK_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevJoystickHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevJoystickHIDReportBuffer),
			},
	};

/*
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#pragma message(VAR_NAME_VALUE(DDRC))
*/

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED2);
		HID_Device_USBTask(&Joystick_HID_Interface);
		Led_Task();
		Buzzer_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	DDRD = 0x08;
	PORTD = 0x00;

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	Buttons_Init();
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup Endpoints */
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Joystick_HID_Interface);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(LED_OUT_EPADDR, EP_TYPE_INTERRUPT, LED_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(BUZZER_OUT_EPADDR, EP_TYPE_INTERRUPT, BUZZER_EPSIZE, 1);

	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Joystick_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Joystick_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	USB_JoystickReport_Data_t* JoystickReport = (USB_JoystickReport_Data_t*)ReportData;


	uint8_t JoyStatus_LCL    = Joystick_GetStatus();
	uint8_t ButtonStatus_LCL = Buttons_GetStatus();

	if (JoyStatus_LCL & JOY_UP)
	  JoystickReport->Y = -100;
	else if (JoyStatus_LCL & JOY_DOWN)
	  JoystickReport->Y =  100;

	if (JoyStatus_LCL & JOY_LEFT)
	  JoystickReport->X = -100;
	else if (JoyStatus_LCL & JOY_RIGHT)
	  JoystickReport->X =  100;

	if (JoyStatus_LCL & JOY_PRESS)
	  JoystickReport->Button |= (1 << 1);

	if (ButtonStatus_LCL & BUTTONS_BUTTON1)
	  JoystickReport->Button |= (1 << 0);


	*ReportSize = sizeof(USB_JoystickReport_Data_t);
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	// Unused (but mandatory for the HID class driver) in this demo, since there are no Host->Device reports
}

void ProcessLedReport(uint16_t LedReport) {
	for (uint16_t i = 0; i < 16; i++) {
		if (LedReport & (1 << i)) {
			
		}
	}
}

void ProcessBuzzerReport(uint8_t BuzzerReport) {
	if (BuzzerReport) {
		PORTD |= 0x08;
	} else {
		PORTB &= ~0x08;
	}
}

/*
ISR(USB_COM_vect, ISR_BLOCK)
{
    uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();

	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;
	
    // Handle endpoints here
    Endpoint_SelectEndpoint(PrevSelectedEndpoint);

	if (PrevSelectedEndpoint == LED_OUT_EPADDR) {
		uint16_t LedReport = Endpoint_Read_16_LE();

		ProcessLedReport(LedReport);
	} else if (PrevSelectedEndpoint == BUZZER_OUT_EPADDR) {
		uint8_t BuzzerReport = Endpoint_Read_8();

		ProcessBuzzerReport(BuzzerReport);
	}
}
*/

void Led_Task(void) {
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;
	
	/* Select the Keyboard LED Report Endpoint */
	Endpoint_SelectEndpoint(LED_OUT_EPADDR);

	/* Check if Keyboard LED Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint16_t LedReport = Endpoint_Read_16_LE();

			/* Process the read LED report from the host */
			ProcessLedReport(LedReport);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}

void Buzzer_Task(void) {
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;
	
	/* Select the Keyboard LED Report Endpoint */
	Endpoint_SelectEndpoint(BUZZER_OUT_EPADDR);

	/* Check if Keyboard LED Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Read in the Buzzer report from the host */
			uint8_t BuzzerReport = Endpoint_Read_8();

			/* Process the read Buzzer report from the host */
			ProcessBuzzerReport(BuzzerReport);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}
