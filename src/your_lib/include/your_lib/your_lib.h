#ifndef ABDFA2BC_B3C6_4AC3_ACE5_B074B2582347
#define ABDFA2BC_B3C6_4AC3_ACE5_B074B2582347

/**
 * \ingroup your_lib_namespace
 *
 * your_lib namespace.
 */
namespace your_lib {

/**
 * @class your_lib::summationClass
 * @headerfile your_lib.h <your_lib/your_lib.h>
 *
 * @brief A class from your_lib namespace.
 *
 * This class does a summation.
 */
class summationClass
{
public:
    /**
     * Constructor
     */
    summationClass();

    /**
     * Destructor
     */
    virtual ~summationClass();

    /**
     * A method that does a summation
     */
    virtual double doSomething(double op1, double op2);
};


/**
 * @class your_lib::differenceClass
 * @headerfile your_lib.h <your_lib/your_lib.h>
 *
 * @brief A derived class from your_lib namespace.
 *
 * This class performs a difference.
 */
class differenceClass : public summationClass
{
public:
    /**
     * Constructor
     */
    differenceClass();

    /**
     * Destructor
     */
    virtual ~differenceClass();

    /**
     * A method that does something
     */
    virtual double doSomething(double op1, double op2);
};


} // namespace your_lib

#endif /* ABDFA2BC_B3C6_4AC3_ACE5_B074B2582347 */
