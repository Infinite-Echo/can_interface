#ifndef CAN_INTERFACE__VISIBILITY_CONTROL_H_
#define CAN_INTERFACE__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define CAN_INTERFACE_EXPORT __attribute__ ((dllexport))
    #define CAN_INTERFACE_IMPORT __attribute__ ((dllimport))
  #else
    #define CAN_INTERFACE_EXPORT __declspec(dllexport)
    #define CAN_INTERFACE_IMPORT __declspec(dllimport)
  #endif
  #ifdef CAN_INTERFACE_BUILDING_LIBRARY
    #define CAN_INTERFACE_PUBLIC CAN_INTERFACE_EXPORT
  #else
    #define CAN_INTERFACE_PUBLIC CAN_INTERFACE_IMPORT
  #endif
  #define CAN_INTERFACE_PUBLIC_TYPE CAN_INTERFACE_PUBLIC
  #define CAN_INTERFACE_LOCAL
#else
  #define CAN_INTERFACE_EXPORT __attribute__ ((visibility("default")))
  #define CAN_INTERFACE_IMPORT
  #if __GNUC__ >= 4
    #define CAN_INTERFACE_PUBLIC __attribute__ ((visibility("default")))
    #define CAN_INTERFACE_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define CAN_INTERFACE_PUBLIC
    #define CAN_INTERFACE_LOCAL
  #endif
  #define CAN_INTERFACE_PUBLIC_TYPE
#endif

#endif  // CAN_INTERFACE__VISIBILITY_CONTROL_H_
