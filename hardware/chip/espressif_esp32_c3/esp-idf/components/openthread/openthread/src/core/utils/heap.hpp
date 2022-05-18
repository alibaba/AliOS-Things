/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes definitions for heap.
 *
 */

#ifndef OT_HEAP_HPP_
#define OT_HEAP_HPP_

#include "openthread-core-config.h"

#if !OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE

#include <stddef.h>
#include <stdint.h>

#include "common/const_cast.hpp"
#include "common/non_copyable.hpp"

namespace ot {
namespace Utils {

/**
 * This class represents a memory block.
 *
 * A block is of the structure as below.
 *
 *     +------------------------------+
 *     | mSize   |  mMemory |  mNext  |
 *     |---------|----------| --------|
 *     | 2 bytes |  n bytes | 2 bytes |
 *     +------------------------------+
 *
 * Since block metadata is of 4-byte size, mSize and mNext are separated at the beginning
 * and end of the block to make sure the mMemory is aligned with long.
 *
 */
class Block
{
    friend class Heap;

public:
    /**
     * This method returns the size of this block.
     *
     * @returns Size of this block.
     *
     */
    uint16_t GetSize(void) const { return mSize; }

    /**
     * This method updates the size of this block.
     *
     * @param[in]   aSize   Size of this block in bytes.
     *
     */
    void SetSize(uint16_t aSize) { mSize = aSize; }

    /**
     * This method returns the offset of the free block after this block.
     *
     * @note This offset is relative to the start of the heap.
     *
     * @returns Offset of the next free block in bytes.
     *
     * @retval  0   This block is not free.
     *
     */
    uint16_t GetNext(void) const
    {
        return *reinterpret_cast<const uint16_t *>(
            reinterpret_cast<const void *>(reinterpret_cast<const uint8_t *>(this) + sizeof(mSize) + mSize));
    }

    /**
     * This method updates the offset of the free block after this block.
     *
     * @note This offset @p aNext must be relative to the start of the heap.
     *
     * @param[in]   aNext   Offset of the next free block in bytes.
     *
     */
    void SetNext(uint16_t aNext)
    {
        *reinterpret_cast<uint16_t *>(
            reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(this) + sizeof(mSize) + mSize)) = aNext;
    }

    /**
     * This method returns the pointer to the start of the memory for user.
     *
     * @retval  Pointer to the user memory. The pointer address is aligned to sizeof(long).
     *
     */
    void *GetPointer(void) { return &mMemory; }

    /**
     * This method returns the offset of the free block after the left neighbor block.
     *
     * @returns Offset in bytes.
     *
     */
    uint16_t GetLeftNext(void) const { return *(&mSize - 1); }

    /**
     * This method returns whether the left neighbor block is a free block.
     *
     * @retval  true    The left neighbor block is free.
     * @retval  false   The left neighbor block is not free.
     *
     */
    bool IsLeftFree(void) const { return GetLeftNext() != 0; }

    /**
     * This method returns whether the current block is a free block.
     *
     * @retval  true    The block is free.
     * @retval  false   The block is not free.
     *
     */
    bool IsFree(void) const { return mSize != kGuardBlockSize && GetNext() != 0; }

private:
    static constexpr uint16_t kGuardBlockSize = 0xffff; // Size value of the guard block.

    uint16_t mSize; // Number of bytes in mMemory.

    // Memory for user, with size of `mNext` to ensure size of this
    // structure is equal to size of block metadata, i.e.,
    // sizeof(mSize) + sizeof(mNext).
    uint8_t mMemory[sizeof(uint16_t)];
};

/**
 * This class defines functionality to manipulate heap.
 *
 * This implementation is currently for mbedTLS.
 *
 * The memory is divided into blocks. The whole picture is as follows:
 *
 *     +--------------------------------------------------------------------------+
 *     |    unused      |    super   | block 1 | block 2 | ... | block n | guard  |
 *     +----------------+------------+---------+---------+-----+---------+--------+
 *     | kAlignSize - 2 | kAlignSize | 4 + s1  | 4 + s2  | ... | 4 + s4  |   2    |
 *     +--------------------------------------------------------------------------+
 *
 */
class Heap : private NonCopyable
{
public:
    /**
     * This constructor initializes a memory heap.
     *
     */
    Heap(void);

    /**
     * This method allocates at least @p aCount * @aSize bytes memory and initialize to zero.
     *
     * @param[in]   aCount  Number of allocate units.
     * @param[in]   aSize   Unit size in bytes.
     *
     * @returns A pointer to the allocated memory.
     *
     * @retval  nullptr    Indicates not enough memory.
     *
     */
    void *CAlloc(size_t aCount, size_t aSize);

    /**
     * This method free memory pointed by @p aPointer.
     *
     * @param[in]   aPointer    A pointer to the memory to free.
     *
     */
    void Free(void *aPointer);

    /**
     * This method returns whether the heap is clean.
     *
     */
    bool IsClean(void) const
    {
        Heap &       self  = *AsNonConst(this);
        const Block &super = self.BlockSuper();
        const Block &first = self.BlockRight(super);
        return super.GetNext() == self.BlockOffset(first) && first.GetSize() == kFirstBlockSize;
    }

    /**
     * This method returns the capacity of this heap.
     *
     */
    size_t GetCapacity(void) const { return kFirstBlockSize; }

    /**
     * This method returns free space of this heap.
     */
    size_t GetFreeSize(void) const { return mMemory.mFreeSize; }

private:
#if OPENTHREAD_CONFIG_DTLS_ENABLE
    static constexpr uint16_t kMemorySize = OPENTHREAD_CONFIG_HEAP_INTERNAL_SIZE;
#else
    static constexpr uint16_t kMemorySize = OPENTHREAD_CONFIG_HEAP_INTERNAL_SIZE_NO_DTLS;
#endif
    static constexpr uint16_t kAlignSize          = sizeof(void *);
    static constexpr uint16_t kBlockRemainderSize = kAlignSize - sizeof(uint16_t) * 2;
    static constexpr uint16_t kSuperBlockSize     = kAlignSize - sizeof(Block);
    static constexpr uint16_t kFirstBlockSize     = kMemorySize - kAlignSize * 3 + kBlockRemainderSize;
    static constexpr uint16_t kSuperBlockOffset   = kAlignSize - sizeof(uint16_t);
    static constexpr uint16_t kFirstBlockOffset   = kAlignSize * 2 - sizeof(uint16_t);
    static constexpr uint16_t kGuardBlockOffset   = kMemorySize - sizeof(uint16_t);

    static_assert(kMemorySize % kAlignSize == 0, "The heap memory size is not aligned to kAlignSize!");

    /**
     * This method returns the block at offset @p aOffset.
     *
     * @param[in]   aOffset     Offset in bytes.
     *
     * @returns A reference to the block.
     *
     */
    Block &BlockAt(uint16_t aOffset) { return *reinterpret_cast<Block *>(&mMemory.m16[aOffset / 2]); }

    /**
     * This method returns the block of @p aPointer.
     *
     * @param[in]   aPointer     The pointer returned by CAlloc().
     *
     * @returns A reference to the block.
     *
     */
    Block &BlockOf(void *aPointer)
    {
        uint16_t offset = static_cast<uint16_t>(reinterpret_cast<uint8_t *>(aPointer) - mMemory.m8);
        offset -= sizeof(uint16_t);
        return BlockAt(offset);
    }

    /**
     * This method returns the super block.
     *
     * @returns Reference to the super block.
     *
     */
    Block &BlockSuper(void) { return BlockAt(kSuperBlockOffset); }

    /**
     * This method returns the free block after @p aBlock in the free block list.
     *
     * @param[in]   aBlock  A reference to the block.
     *
     * @returns Reference to the free block after this block.
     *
     */
    Block &BlockNext(const Block &aBlock) { return BlockAt(aBlock.GetNext()); }

    /**
     * This method returns the block on the right side of @p aBlock.
     *
     * @param[in]   aBlock  A reference to the block.
     *
     * @returns Reference to the block on the right side.
     *
     */
    Block &BlockRight(const Block &aBlock) { return BlockAt(BlockOffset(aBlock) + sizeof(Block) + aBlock.GetSize()); }

    /**
     * This method returns the free block before @p aBlock in the free block list.
     *
     * @returns Reference to the free block before this block.
     *
     */
    Block &BlockPrev(const Block &aBlock);

    /**
     * This method returns whether the block on the left side of @p aBlock is free.
     *
     * @param[in]   aBlock  A reference to the block.
     *
     */
    bool IsLeftFree(const Block &aBlock) { return (BlockOffset(aBlock) != kFirstBlockOffset && aBlock.IsLeftFree()); }

    /**
     * This method returns the offset of @p aBlock.
     *
     * @param[in]   aBlock  A reference to the block.
     *
     * @returns Offset in bytes of @p aBlock.
     *
     */
    uint16_t BlockOffset(const Block &aBlock)
    {
        return static_cast<uint16_t>(reinterpret_cast<const uint8_t *>(&aBlock) - mMemory.m8);
    }

    /**
     * This method inserts @p aBlock into the free block list.
     *
     * The free block list is single linked and is sorted by size from minimal to maximum.
     *
     * @param[in]   aPrev   A reference to the block after which to place @p aBlock.
     * @param[in]   aBlock  A reference to the block.
     *
     */
    void BlockInsert(Block &aPrev, Block &aBlock);

    union
    {
        uint16_t mFreeSize;
        // Make sure memory is long aligned.
        long     mLong[kMemorySize / sizeof(long)];
        uint8_t  m8[kMemorySize];
        uint16_t m16[kMemorySize / sizeof(uint16_t)];
    } mMemory;
};

} // namespace Utils
} // namespace ot

#endif // !OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE

#endif // OT_HEAP_HPP_
