/**
 * @file math_util.c
 *
 * @date Created on: Jun 7, 2011
 * @author iizke
 */

long math_factorial(int n) {
  int i = 0;
  long val = 1;
  for (i=1;i<=n;i++)
    val *= i;
  return val;
}
