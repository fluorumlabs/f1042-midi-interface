/*
	ProtoPlasm: DIY digital pro-audio platform
	Copyright (C) 2013  Artem Godin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

///////////////////////////////////////////////////////////////////////////////
/// @file				defines.h
///
/// @defgroup globals			Global Definitions
/// @{
///
/// Global type and macro definitions used throughout project here and there.
///

#ifndef DEFINES_H
#define DEFINES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#if !defined(__DOXYGEN__)
// Undefining this macro will effectively disable all run-time and compile-time
// assertions.
# define ASSERTIONS
# ifdef __clang__
#  define STATIC_ANALYSIS
# endif
# define ERROR_HARDWARE "Hardware fail"
# define ERROR_QUAD_TO_HEX(n) (char)(((n)<10)?'0'+(n):'a'+(n)-10)
#endif

///////////////////////////////////////////////////////////////////////////////
/// @name Flow control
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	@def assert( x )

	Check for expression and abort program if this check fails
	@param [in] x Expression that should be evaluated.
	The expression is evaluated at the runtime and, if possible, at compile time.
	When checking at compile time, division by zero warning will be signalled;
	At the run-time the current function and expression will be showed on LCD
	screen and the system will be stopped.

	@pre The LCD driver should be initialized prior to first assert
	@warning Will abort FreeRTOS tasks will be aborted and issue infinite loop.
	Therefore the only way to reset the device to normal mode is, literally, to
	reset it.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	assert( buf != null );
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	@see system_error()
*/
#ifdef  ASSERTIONS
# ifndef STATIC_ANALYSIS
#		define assert(x) if ( !__builtin_expect( !!(x), 1 )) ( (void)(1/(x)), Error_Handler() )
# else
#		define assert( x ) if ( !!(x)!=1 ) system_error( #x, __func__ )
# endif
#else
#		define assert( x ) __builtin_expect( !!(x), 1 )
#endif

/**
	Notify compiler, that the condition in subject most likely will be true.

	@param x Condition to check.
*/
#define likely(x) __builtin_expect(!!(x), 1)

/**
	Notify compiler, that the condition in subject most likely will be false.

	@param x Condition to check.
*/
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
	Helper macro for calling callbacks.
	Checks if callback pointer is not null and calls it.

	@param [in] x Pointer to callback.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	invoke( _usb_interfaces[ifnum].init )(); // Call init callback only if it is set
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define invoke(x) if ( (x) != null ) x

/**
	Helper macro for calling callbacks.
	Checks if callback pointer is not null, calls it and returns value.

	@param [in] x Pointer to callback.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	invoke_return( _usb_interfaces[ifnum].init )(); // Call init callback only if it is set
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define invoke_return(x) if ( (x) != null ) return x


/**
	Infinite loop helper macro

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	infinite_loop; // Guess yourself
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define infinite_loop for(;;)

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Types
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Enumeration for generic pointer values.
*/
enum null_e {
    null = 0        ///< Null value for pointers
};

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Compiler Attributes
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Silence compiler about unused funcion arguments
	 *
	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void function( int a )
	{
		unused a;
	}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define unused (void)

/**
	Specifies that the function will not return.
	 *
	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void function() noreturn
	{
		infinite_loop;
	}
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define noreturn __attribute__((noreturn))

/**
	Specifies that the function return value depends only on
	parameters and/or global variables. Good examples of such functions are
	`memcmp` and `strlen`.
*/
#define pure __attribute__((pure))

/**
	Specifies that no alignment should be applied to structure
	members.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct test {
		uint8_t byte;
		uint32_t dword;
	} packed;
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	@see aligned
*/
#define packed __attribute__((packed))

/**
	Specifies that the code should always do something with returning
	value of this function. Again, good candidates are `memcmp` and `strlen`
*/
#define check_result __attribute__((warn_unused_result))

/**
	Specifies that some function parameters should be non-null pointers.

	Example:
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	extern void *
		my_memcpy (void *dest, const void *src, size_t len)
		nonnull( 1, 2 );
	// Causes the compiler to check that, in calls to my_memcpy,
	// arguments dest and src are non-null
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	@see return_nonnull
*/
#define nonnull(...) __attribute__((nonnull( __VA_ARGS__ )))

/**
	Specifies that function return value should be a non-null pointer.

	@see nonnull
	@see important
*/
#define return_nonnull __attribute__((return_nonnull))

/**
	Generally, inlining into a function is limited. For a function marked with
	this attribute, every call **inside** this function is inlined, if possible.
	Whether the function itself is considered for inlining depends on its size
	and the current inlining parameters.
*/
#define flatten __attribute__((flatten))

/**
	Indicates that the specified function is an interrupt handler. The compiler
	generates function entry and exit sequences suitable for use in an interrupt
	handler when this attribute is present.
*/
#define interrupt __attribute__((interrupt))

/**
	The hot attribute on a function is used to inform the compiler that the
	function is a hot spot of the compiled program. The function is optimized
	more aggressively and on many target it is placed into special subsection
	of the text section so all hot functions appears close together improving
	locality.
*/
#define hot __attribute__((hot))

/**
	The cold attribute on functions is used to inform the compiler that the
	function is unlikely to be executed. The function is optimized for size
	rather than speed and on many targets it is placed into special subsection
	of the text section so all cold functions appears close together improving
	code locality of non-cold parts of program. The paths leading to call of
	cold functions within code are marked as unlikely by the branch prediction
	mechanism. It is thus useful to mark functions used to handle unlikely
	conditions, such as perror, as cold to improve optimization of hot functions
	that do call marked functions in rare occasions.
*/
#define cold __attribute__((cold))

/**
	GCC defines internal visibility to mean that a function is never called from
	another module.
*/
#define internal __attribute__((visibility("internal")))

/**
	Use this for all other GCC attributes.
 */
#define attribute(x) __attribute__((x))

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Type Conversion
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Convert ordinal value to uint8_t

	@param [in] x

	@return @f$ x[7:0] @f$
*/
#define tobyte(x) ((uint8_t)((x) & 0xff))

/**
	Convert ordinal value to uint16_t

	@param [in] x

	@return @f$ x[15:0] @f$
*/
#define toword(x) ((uint16_t)((x) & 0xffff))

/**
	Convert ordinal value to uint32_t

	@param [in] x

	@return @f$ x[31:0] @f$
*/
#define todword(x) ((uint32_t)((x) & 0xffffffff ))

/**
	Extract least significant byte of uint16_t

	@param [in] x

	@return @f$ x[7:0] @f$
*/
#define lobyte(x)  ((uint8_t)((x) & 0x00ff))

/**
	Extract most significant byte of uint16_t

	@param [in] x

	@return @f$ x[15:8] @f$
*/
#define hibyte(x)  ((uint8_t)(((x) & 0xff00) >>8))

/**
	Extract least significant word of uint32_t

	@param [in] x

	@return @f$ x[15:0] @f$
*/
#define loword(x)  ((uint16_t)((x) & 0x0000ffff))

/**
	Extract most significant word of uint32_t

	@param [in] x

	@return @f$ x[31:16] @f$
*/
#define hiword(x)  ((uint16_t)(((x) & 0xffff0000) >>16))


/**
	Convert 16-bit ordinal value to series of single uint8_t values

	@param [in] x

	@return @f$ x[7:0], x[15:8] @f$
*/
#define field16(x) lobyte( x ), hibyte( x )

/**
	Convert 24-bit ordinal value to series of single uint8_t values

	@param [in] x

	@return @f$ x[7:0], x[15:8], x[23:16] @f$
*/
#define field24(x) lobyte( x ), hibyte( x ), lobyte( (x)>>16 )

/**
	Convert 32-bit ordinal value to series of single uint8_t values

	@param [in] x

	@return @f$ x[7:0], x[15:8], x[23:16], x[31:24] @f$
*/
#define field32(x) lobyte( x ), hibyte( x ), lobyte( (x)>>16 ), hibyte( (x)>>16 )

/**
	Make 16-bit value from hi and low bytes

	@param [in] hi Most significant byte of resulting value
	@param [in] low Least significant byte of resulting value

	@return @f$ hi << 8 | low @f$
*/
#define make16(hi, low) ( ( (hi) << 8 ) | (low) )

/**
	Swap two values.

	@param [in] a
	@param [in] b
*/
#define swap(a, b)                                                                \
        do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

/// @}
///////////////////////////////////////////////////////////////////////////////
/// @name Min/Max
/// @{
///////////////////////////////////////////////////////////////////////////////

/**
	Return maximum of two ordinals.

	@param [in] x
	@param [in] y

	@return @f$ max( x, y ) @f$

	@note Types of x and y should be compatible.
	@note Doesn't support nesting.
*/
#define max(x, y) ({                                                                \
        typeof(x) _max1 = (x);                                                        \
        typeof(y) _max2 = (y);                                                        \
        (void) (&_max1 == &_max2);                                                \
        (typeof(x))( _max1 > _max2 ? _max1 : _max2 ); })

/**
	Return maximum of three ordinals.

	@param [in] x
	@param [in] y
	@param [in] z

	@return @f$ max( x, y, z ) @f$

	@note Types of x, y and z should be compatible.
	@note Doesn't support nesting.
*/
#define max3(x, y, z) ({                                                        \
        typeof(x) _max1 = (x);                                                        \
        typeof(y) _max2 = (y);                                                        \
        typeof(z) _max3 = (z);                                                        \
        (void) (&_max1 == &_max2);                                                \
        (void) (&_max1 == &_max3);                                                \
        (typeof(x))( _max1 > _max2 ? (_max1 > _max3 ? _max1 : _max3) :                \
                (_max2 > _max3 ? _max2 : _max3) ); })

/**
	Return minimum of two ordinals.

	@param [in] x
	@param [in] y

	@return @f$ min( x, y ) @f$

	@note Types of x and y should be compatible.
	@note Doesn't support nesting.
*/
#define min(x, y) ({                                                                \
        typeof(x) _min1 = (x);                                                        \
        typeof(y) _min2 = (y);                                                        \
        (void) (&_min1 == &_min2);                                                \
        (typeof(x)) ( _min1 < _min2 ? _min1 : _min2 ); })

/**
	Return minimum of three ordinals.

	@param [in] x
	@param [in] y
	@param [in] z

	@return @f$ min( x, y, z ) @f$

	@note Types of x, y and z should be compatible.
	@note Doesn't support nesting.
*/
#define min3(x, y, z) ({                                                        \
        typeof(x) _min1 = (x);                                                        \
        typeof(y) _min2 = (y);                                                        \
        typeof(z) _min3 = (z);                                                        \
        (void) (&_min1 == &_min2);                                                \
        (void) (&_min1 == &_min3);                                                \
        (typeof(x)) ( _min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) :                \
                (_min2 < _min3 ? _min2 : _min3) ); })

/**
	Return a value clamped to a given range.

	@param [in] val
	@param [in] min Lower margin for value
	@param [in] max Upper margin for value

	@return Value clamped to a given range.
*/
#define clamp(val, min, max) ({                                                        \
        typeof(val) __val = (val);                                                \
        typeof(val) __min = (min);                                                \
        typeof(val) __max = (max);                                                \
        __val = __val < __min ? __min: __val;                                        \
        (typeof(val))( __val > __max ? __max: __val ); })

/// @}
///////////////////////////////////////////////////////////////////////////////

/**
 *      Error handler to be called on failed assertions
 *
 *      @note The handler itself is defined in main.c by STM32CubeMX
 */
void Error_Handler(void);

#endif // DEFINES_H

/// @}
///////////////////////////////////////////////////////////////////////////////
