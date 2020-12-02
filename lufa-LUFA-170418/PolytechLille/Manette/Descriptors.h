#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

/* Includes: */
#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>

/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be defined in the
 *  application code, as the configuration descriptor contains several sub-descriptors which
 *  vary between devices, and which describe the device's usage to the host.
 */
typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;

	// Joystick HID Interface
	USB_Descriptor_Interface_t            HID1_JoystickInterface;
	USB_HID_Descriptor_HID_t              HID1_JoystickHID;
	USB_Descriptor_Endpoint_t             HID1_ReportINEndpoint;

	// LEDs Interface
	USB_Descriptor_Interface_t            HID2_LedInterface;
	USB_Descriptor_Endpoint_t             HID2_ReportOUTEndpoint;

	// Buzzers Interface
	USB_Descriptor_Interface_t            HID3_BuzzerInterface;
	USB_Descriptor_Endpoint_t             HID3_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

/** Enum for the device interface descriptor IDs within the device. Each interface descriptor
 *  should have a unique ID index associated with it, which can be used to refer to the
 *  interface from other descriptors.
 */
enum InterfaceDescriptors_t
{
	INTERFACE_ID_Joystick = 0, /**< Joystick interface desciptor ID */
	INTERFACE_ID_Led = 1, /**< LEDs interface desciptor ID */
	INTERFACE_ID_Buzzer = 2, /**< Buzzers interface desciptor ID */
};

/** Enum for the device string descriptor IDs within the device. Each string descriptor should
 *  have a unique ID index associated with it, which can be used to refer to the string from
 *  other descriptors.
 */
enum StringDescriptors_t
{
	STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
	STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
	STRING_ID_Product      = 2, /**< Product string ID */
};

/* Macros: */
/** Endpoint address of the Joystick HID reporting IN endpoint. */
#define JOYSTICK_IN_EPADDR         	(ENDPOINT_DIR_IN | 1)

/** Size in bytes of the Joystick HID reporting IN endpoint. */
#define JOYSTICK_EPSIZE             8

/** Endpoint address of the LEDs HID reporting OUT endpoint. */
#define LED_OUT_EPADDR           	(ENDPOINT_DIR_OUT | 2)

/** Size in bytes of the LEDs HID reporting OUT endpoint. */
#define LED_EPSIZE            		2

/** Endpoint address of the Buzzers HID reporting OUT endpoint. */
#define BUZZER_OUT_EPADDR          	(ENDPOINT_DIR_OUT | 3)

/** Size in bytes of the Buzzers HID reporting OUT endpoint. */
#define BUZZER_EPSIZE            	1

/* Function Prototypes: */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint16_t wIndex,
									const void** const DescriptorAddress)
									ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

