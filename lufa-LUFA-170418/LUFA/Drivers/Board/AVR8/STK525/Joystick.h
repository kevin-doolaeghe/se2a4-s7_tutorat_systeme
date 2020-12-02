#ifndef __JOYSTICK_STK525_H__
#define __JOYSTICK_STK525_H__

	/* Includes: */
		#include "../../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_JOYSTICK_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/Joystick.h instead.
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define JOY_BMASK                 ((1 << 5) | (1 << 6) | (1 << 7))
			#define JOY_EMASK                 ((1 << 4) | (1 << 5))

			#define JOY_PORTE_MASK_SHIFT      1
	#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Mask for the joystick being pushed in the left direction. */
			#define JOY_LEFT                  (1 << 6)

			/** Mask for the joystick being pushed in the right direction. */
			#define JOY_RIGHT                ((1 << 4) >> JOY_PORTE_MASK_SHIFT)

			/** Mask for the joystick being pushed in the upward direction. */
			#define JOY_UP                    (1 << 7)

			/** Mask for the joystick being pushed in the downward direction. */
			#define JOY_DOWN                 ((1 << 5) >> JOY_PORTE_MASK_SHIFT)

			/** Mask for the joystick being pushed inward. */
			#define JOY_PRESS                 (1 << 5)

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void Joystick_Init(void)
			{
				DDRB  &= ~JOY_BMASK;
				DDRC  &= ~JOY_EMASK;

				PORTB |=  JOY_BMASK;
				PORTC |=  JOY_EMASK;
			}

			static inline void Joystick_Disable(void)
			{
				DDRB  &= ~JOY_BMASK;
				DDRC  &= ~JOY_EMASK;

				PORTB &= ~JOY_BMASK;
				PORTC &= ~JOY_EMASK;
			}

			static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Joystick_GetStatus(void)
			{
				return (((uint8_t)~PINB & JOY_BMASK) | (((uint8_t)~PINC & JOY_EMASK) >> JOY_PORTE_MASK_SHIFT));
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

