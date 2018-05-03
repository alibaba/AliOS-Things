#ifndef COMMON_H
#define COMMON_H

size_t charncpy(char *dest, const char *src, size_t n);

uint16_t getHostOrderShortFromNetwork(void * buff);
uint16_t getNetworkOrderShortFromHost(uint16_t hostshort, void * buff);

#endif
