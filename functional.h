#ifndef FUNCTIONAL
#define FUNCTIONAL

typedef float(*mappable)(float, float);

float ass(float a, float b);
float neg(float a, float b);

float sum(float a, float b);
float min(float a, float b);
float mul(float a, float b);
float dvi(float a, float b);

float gt(float a, float b);
float lt(float a, float b);
float gte(float a, float b);
float lte(float a, float b);
float deq(float a, float b);
float neq(float a, float b);

varTypeEnum max(varTypeEnum a, varTypeEnum b);
conNodeType * apply(mappable f, conNodeType * a, conNodeType * b, varTypeEnum dstType);

#endif


