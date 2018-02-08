#ifndef _ALC5651_H_
#define _ALC5651_H_
void alc5651_reg_dump(void);
void alc5651_index_dump(void);
void alc5651_init(void);
void alc5651_set_word_len(int len_idx);
void alc5651_init_interface1(void);
void alc5651_init_interface2(void);
#endif