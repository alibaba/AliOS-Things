/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_TABLE_H_
#define _TEE_TABLE_H_

/*
 * Define a new Table
 * SORT .table section in lds is a MUST!!
 * usage: NEW_TABLE(struct dev_t, dev)
 *
 * table.name.begin
 * table.name.data
 * table.name.data
 * tabl.name.end
 */
#define NEW_TABLE(type, name)         \
    type g_table_##name##_start[0] __attribute__((__section__(".table." #name ".begin"))); \
    type g_table_##name##_end[0] __attribute__((__section__(".table." #name ".end"))); \

/*
 * Add a new element to the .table.name.data
 * usage: struct dev_t uart_dev TABLE_ATTR(name) = { };
 */
#define TABLE_ATTR(name)              \
    __attribute__((__section__(".table." #name ".data")))

/*
 * query the table.name.data. p variable should be decleared as volatile because
 * the table could be empty.
 * usage: struct dev_t *p;
 *        TABLE_FOREACH(p, dev) {
 *        }
 */
#define TABLE_FOREACH(p, name)         \
    for((p) = g_table_##name##_start; (g_table_##name##_start != g_table_##name##_end) && ((p)!= g_table_##name##_end); (p)++)

#endif /* _TEE_TABLE_H_ */
