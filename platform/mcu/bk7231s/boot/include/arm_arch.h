#ifndef _ARM_ARCH_H_
#define _ARM_ARCH_H_

/**
 * RD an 8-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 8-bit datum
 */
#define REG_RD8(_pBase, _offset) *((volatile UINT8 *)(_pBase + _offset))

/**
 * RD a 16-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 16-bit datum
 */
#define REG_RD16(_pBase, _offset) *((volatile UINT16 *)(_pBase + _offset))

/**
 * RD a 32-bit register from the core
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @return 32-bit datum
 */
#define REG_RD32(_pBase, _offset) *((volatile UINT32 *)(_pBase + _offset))

/**
 * WR an 8-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 8-bit datum
 */
#define REG_WR8(_pBase, _offset, _data) \
	(*((volatile UINT8 *)(_pBase + _offset)) = _data)

/**
 * WR a 16-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 16-bit datum
 */
#define REG_WR16(_pBase, _offset, _data) \
	(*((volatile UINT16 *)(_pBase + _offset)) = _data)

/**
 * WR a 32-bit core register
 * @param _pBase core base address in memory
 * @param _offset offset into the core's register space
 * @param _data 32-bit datum
 */
#define REG_WR32(_pBase, _offset, _data) \
	(*((volatile UINT32 *)(_pBase + _offset)) = _data)

#define REG_READ(addr)          (*((volatile UINT32 *)(addr)))
#define REG_WRITE(addr, _data) 	(*((volatile UINT32 *)(addr)) = (_data))

#endif
