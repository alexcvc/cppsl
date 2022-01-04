#include "cppsl/cppsl.h"

cppsl::summationClass::summationClass()
{
}

cppsl::summationClass::~summationClass()
{
}

double cppsl::summationClass::doSomething(double op1, double op2)
{
    return op1 + op2;
}

cppsl::differenceClass::differenceClass() :
        summationClass()
{
}

cppsl::differenceClass::~differenceClass()
{
}

double cppsl::differenceClass::doSomething(double op1, double op2)
{
    return op1 - op2;
}
