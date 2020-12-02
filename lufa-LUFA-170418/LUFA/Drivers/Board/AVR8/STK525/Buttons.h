#ifndef __BUTTONS_STK525_H__
#define __BUTTONS_STK525_H__

	/* Includes: */
		#include "../../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_BUTTONS_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/Buttons.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Button mask for the first button on the board. */
			#define BUTTONS_BUTTON1      (1 << 2)

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void Buttons_Init(void)
			{
				DDRB  &= ~BUTTONS_BUTTON1;
				PORTB |=  BUTTONS_BUTTON1;
			}

			static inline void Buttons_Disable(void)
			{
				DDRB  &= ~BUTTONS_BUTTON1;
				PORTB &= ~BUTTONS_BUTTON1;
			}

			static inline uint8_t Buttons_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Buttons_GetStatus(void)
			{
				return ((PINB & BUTTONS_BUTTON1) ^ BUTTONS_BUTTON1);
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

