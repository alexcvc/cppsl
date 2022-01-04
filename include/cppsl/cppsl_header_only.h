#ifndef B5987B1D_D0E2_445C_BBB7_A4089035AF6D
#define B5987B1D_D0E2_445C_BBB7_A4089035AF6D

/**
 * \ingroup cppsl_namespace
 *
 * cppsl namespace.
 */
namespace cppsl {

/**
 * @brief Computes the sum of two values.
 * @param[in] op1 first input value.
 * @param[in] op2 second input value.
 * @returns sum of op1 and op2.
 */
template<typename DatumType>
DatumType sum(DatumType op1, DatumType op2)
{
    return (op1 + op2);
}


/**
 * @brief Computes the difference of two values.
 * @param[in] op1 first input value.
 * @param[in] op2 second input value.
 * @returns difference of op1 and op2.
 */
template<typename DatumType>
DatumType sub(DatumType op1, DatumType op2)
{
    return (op1 - op2);
}

} // namespace cppsl

#endif /* B5987B1D_D0E2_445C_BBB7_A4089035AF6D */
