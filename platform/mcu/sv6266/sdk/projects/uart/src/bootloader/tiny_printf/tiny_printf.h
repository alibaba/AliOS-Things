#ifndef _TINY_PRINTF_H
#define _TINY_PRINTF_H

int tiny_printf(const char *fmt, ...);


/*! \brief tiny_printf output switch
 *
 *  this function can control the tiny_printf output the log or not.
 *
 * \param enable 1, the log will output, 0: the log will disable.
 * \return none
 */
void tiny_printf_display(int enable);

#endif
