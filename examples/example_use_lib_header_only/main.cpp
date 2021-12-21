#include <your_lib_header_only/your_lib_header_only.h>

#include <cstdlib>
#include <cmath>
#include <iostream>

int main()
{
    std::cout << "Executable example using the header-only library!" << std::endl;

    double tol = 1e-10;
    double op1 = 15.0;
    double op2 = 10.0;

    if( fabs(your_lib::sum(op1, op2) - (op1 + op2)) > tol )
    {
        std::cerr << "[ERR] sumClass.doSomething(" << op1 << "," << op2
                  << ") is equal to " << your_lib::sum(op1, op2)
                  << " instead of the expected " << op1 + op2 << std::endl;
        return EXIT_FAILURE;
    }

    if( fabs(your_lib::sub(op1, op2) - (op1 - op2)) > tol )
    {
        std::cerr << "[ERR] sumClass.doSomething(" << op1 << "," << op2
                  << ") is equal to " << your_lib::sub(op1, op2)
                  << " instead of the expected " << op1 - op2 << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
