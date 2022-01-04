
/**
 * \ingroup cppsl_namespace
 *
 * cppsl namespace.
 */

#ifndef C1245BD2_B671_4EF8_82F7_CB1BD24AACB1
#define C1245BD2_B671_4EF8_82F7_CB1BD24AACB1

namespace cppsl {

/**
 * @class cppsl::summationClass
 * @headerfile cppsl.h <cppsl/cppsl.h>
 *
 * @brief A class from cppsl namespace.
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
 * @class cppsl::differenceClass
 * @headerfile cppsl.h <cppsl/cppsl.h>
 *
 * @brief A derived class from cppsl namespace.
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


} // namespace cppsl

#endif /* C1245BD2_B671_4EF8_82F7_CB1BD24AACB1 */
