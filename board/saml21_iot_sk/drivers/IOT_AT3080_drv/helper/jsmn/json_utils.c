#include "mx_common.h"

#define mico_int_part_of(x)     ((int)(x))

static inline int mico_frac_part_of(float x, short precision)
{
    int scale = 1;

    while (precision--)
        scale *= 10;

    return (x < 0 ? (int)(((int)x - x) * scale) : (int)((x - (int)x) * scale));
}

/* Function to convert a string to float.
 * \param[in] str Pointer to a string to be addressed
 * \param[out] endptr Pointer pointing to next character in a string */
float mico_strtof(const char *str, char **endptr)
{
    char *start_ptr = (char *)str;
    int sign = 1;

    if (endptr == NULL) {
        char *end_ptr;
        endptr = &end_ptr;
    }

    if (*start_ptr == '-') {
        sign = -1;
        start_ptr++;
    }
    int dec_val = 0;
    int powten = 1;

    int int_val = strtoul(start_ptr, endptr, 10);
    if (**endptr == '.') {
        start_ptr = *endptr + 1;
        dec_val = strtoul(start_ptr, endptr, 10);
        while (start_ptr++ != *endptr)
            powten *= 10;
    } else
        return sign * (float)int_val;

    float dec_frac = (float)dec_val/powten;
    float result = (float)(int_val + dec_frac);

    /* Below part is done in order to improve the accuracy of the result.
     * Since addition above results in float value being drifted from
     * the actual value by narrow margin. e.g 50.10 results in float
     * equivalent of 50.09.*/
    /* TODO: Visit again to see if the below code really helps.
     * Sometimes, reporting values differently is a result of the
     * way float values are stored in memory. In that case, the below
     * code will not improve anything. Eg. If 50.1 is stored as 50.09
     * in memory, we cannot do much about it.
     */
    int result_int_value = mico_int_part_of(result);
    float result_frac_value = (float)(result) - result_int_value;

    /* Generally difference between two float values comes out to be in
     * the order of 1/powten. e.g 0.10-0.09 comes out to be 0.00...1.
     * Hence we multiply the result of subtraction to achieve the accuracy
     * within desired float precision. */
    if (mico_frac_part_of(dec_frac, powten/10) >
            mico_frac_part_of(result_frac_value, powten/10))
        result += ((dec_frac - result_frac_value) * powten);
    if (mico_frac_part_of(result_frac_value, powten/10) >
            mico_frac_part_of(dec_frac, powten/10))
        result += ((result_frac_value - dec_frac) * powten);

    return sign * result;
}
