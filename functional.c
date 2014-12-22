/* Author: Brugnara Martin #157791 */

#include "interpreter.h"
#include "functional.h"

float ass(float a, float b) {return a;}
float neg(float a, float b) {return -a;}

float sum(float a, float b) {return a + b;}
float mni(float a, float b) {return a - b;}
float mul(float a, float b) {return a * b;}
float dvi(float a, float b) {return a / b;} // DIV was yet taken

float gt(float a, float b) {return a > b;}
float lt(float a, float b) {return a < b;}
float gte(float a, float b) {return a >= b;}
float lte(float a, float b) {return a <= b;}
float deq(float a, float b) {return a == b;}
float neq(float a, float b) {return a != b;}

float and(float a, float b) {return a && b;}
float or(float a, float b) {return a || b;}
float not(float a, float b) {return !a;}

varTypeEnum max(varTypeEnum a, varTypeEnum b) {return a>b?a:b;}
