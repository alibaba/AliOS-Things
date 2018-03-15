#ifndef _ALC5640_H_
#define _ALC5640_H_
void alc5640_reg_dump(void);
void alc5640_index_dump(void);
void alc5640_init(void);
void alc5640_set_word_len(int len_idx);
void alc5640_init_interface1(void);
void alc5640_init_interface2(void);
#endif