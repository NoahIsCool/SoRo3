/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#ifndef SORO_UTIL_H
#define SORO_UTIL_H

namespace Soro {
namespace Util {

template <typename T>
inline void serialize(char *arr, T data) {
    int max = sizeof(T) - 1;
    for (int i = 0; i <= max; i++) {
        arr[max - i] = (data >> (i * 8)) & 0xFF;
    }
}

template <typename T>
inline T deserialize(const char *arr) {
    T result;
    for (unsigned int i = 0; i < sizeof(T); i++) {
        result = (result << 8) + reinterpret_cast<const unsigned char&>(arr[i]);
    }
    return result;
}

} // namespace Util
} // namespace Soro

#endif // SORO_UTIL_H
