/**
 * @file error.h
 * Define some error codes and some utility functions/macros to announce errors.
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef ERROR_H_
#define ERROR_H_

/// Turn on Debug mode
#define DEBUG
/// Print out all error-labeled messages
#define LEVEL_ERROR     0b00000001
/// Print out all warning-labeled messages
#define LEVEL_WARNING   0b00000010
/// Print out all info-labeled messages
#define LEVEL_INFO      0b00000100
/// Print out all messages labeled with Error and Warning
#define LEVEL_EW        0b00000011
/// Print out all messages labeled with Error and Info
#define LEVEL_EI        0b00000101
/// LEVEL_EWI Print out all messages labeled with Error, Info, Warning
#define LEVEL_EWI       0b00000111

#ifdef DEBUG
extern long debug;
#define iprintf(level, fmt, args...)                  \
  {                                                       \
    if (level & debug) {                                 \
      printf("File %s, line %d: \n", __FILE__, __LINE__); \
      printf(fmt, ## args);                               \
    }                                                     \
  }
#else
#define iprintf(level, fmt, args...)
#endif

#define try(stm)                                          \
  {                                                       \
    int _err_ = stm;                                      \
    if (_err_ < 0)                                        \
      return _err_;                                       \
  }

#define check_null_pointer(_p)                            \
  {                                                       \
    if (!_p) {                                            \
      iprintf(LEVEL_WARNING, "NULL pointer\n");             \
      return ERR_POINTER_NULL;                            \
    }                                                     \
  }

#define TRUE                        1
#define FALSE                       0
/// Return this value when a function finishes successfully
#define SUCCESS                     0
/// Error when a pointer is null
#define ERR_POINTER_NULL            (-1)
/// Error when not enough memory in system
#define ERR_MALLOC_FAIL             (-2)
/// Error when user does not provide a correct Flow Type ID
#define ERR_RANDOM_TYPE_FAIL        (-17)
/// Error when the time of event is not consistent, eg. it is late than the current time
#define ERR_EVENT_TIME_WRONG        (-18)
/// Error when an event-type is not supported
#define ERR_EVENT_TYPE_FAIL         (-19)
/// Error when packet state is not correct
#define ERR_PACKET_STATE_WRONG      (-20)
/// Error while inconsistent happen in simulating CSMA
#define ERR_CSMA_INCONSISTENT       (-30)
#endif /* ERROR_H_ */
