/**
 * @file def.h
 *
 * @date Created on: Apr 8, 2011
 * @author iizke
 */

#ifndef DEF_H_
#define DEF_H_

/**
 * Time definition
 */
typedef union time {
  /// time is represented as slot time
  long slot;
  /// time is considered as real time
  float real;
} TIME;

#define MAX_INT INT_MAX

#endif /* DEF_H_ */
