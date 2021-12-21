#include "your_lib/your_lib.h"

your_lib::summationClass::summationClass()
{
}

your_lib::summationClass::~summationClass()
{
}

double your_lib::summationClass::doSomething(double op1, double op2)
{
    return op1 + op2;
}

your_lib::differenceClass::differenceClass() :
        summationClass()
{
}

your_lib::differenceClass::~differenceClass()
{
}

double your_lib::differenceClass::doSomething(double op1, double op2)
{
    return op1 - op2;
}
