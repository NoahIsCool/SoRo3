/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#ifndef SORO_GAMEPADUTIL_H
#define SORO_GAMEPADUTIL_H

namespace Soro {
namespace GamepadUtil {

/* If an axis value is withing a certain percent of zero, this function
 * will return zero. Otherwise it returns the original value.
 */
short filterGamepadDeadzone(short raw, float percent);
float filterGamepadDeadzoneF(float raw, float percent);

/* Converts a 16 bit signed integer ranging from -32768 to 32767
 * into an unsigned char, ranging from 0 to 200
 *
 * This is designed for converting SDL axis values.
 */
char axisShortToAxisByte(short val);

/* Converts a float ranging from -1 to 1 into an unsigned char,
 * ranging from 0 to 200
 *
 * This is designed for converting GLFW axis values.
 */
char axisFloatToAxisByte(float val);

/* Converts an int ranging from -100 to 100 into an unsigned char,
 * ranging from 0 to 200
 */
char axisIntToAxisByte(int val);

/* Converts an int ranging from -100 to 100 into an unsigned char,
 * ranging from 0 to 200
 */
int axisFloatToAxisInt(float val);

/* Converts a byte encoded joystick axis (see joyFloatToByte) back
 * into it's original float value
 */
float axisByteToAxisFloat(char val);

/* Converts a byte encoded joystick axis (see joyFloatToByte) into
 * an int ranging from -100 to 100
 */
int axisByteToAxisInt(char val);

float axisShortToAxisFloat(short val);

} // namespace GamepadUtil
} // namespace Soro

#endif // SORO_GAMEPADUTIL_H
