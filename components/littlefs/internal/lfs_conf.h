#ifndef _LFS_CONF_H_
#define _LFS_CONF_H_

#ifndef CONFIG_LITTLEFS_CNT
#define CONFIG_LITTLEFS_CNT 1
#endif

#ifndef CONFIG_LFS_PROG_SIZE
#define CONFIG_LFS_PROG_SIZE 256
#endif

#ifndef CONFIG_LFS_PAGE_NUM_PER_BLOCK
#define CONFIG_LFS_PAGE_NUM_PER_BLOCK 16
#endif

#ifndef CONFIG_LFS_BLOCK_NUMS
#define CONFIG_LFS_BLOCK_NUMS 32
#endif

#if (CONFIG_LITTLEFS_CNT > 1)
#ifndef CONFIG_LFS2_BLOCK_NUMS
#define CONFIG_LFS2_BLOCK_NUMS 32
#endif
#endif /* CONFIG_LITTLEFS_CNT > 1 */

#if !defined(CONFIG_LITTLEFS_CNT) || \
    !defined(CONFIG_LFS_PROG_SIZE) || \
    !defined(CONFIG_LFS_PAGE_NUM_PER_BLOCK) || \
    !defined(CONFIG_LFS_BLOCK_NUMS)
#pragma message("Attention: you are going to use the default configurations for littlefs. Please double check that these config are suitable for you; otherwise please define your customized 'CONFIG_LITTLEFS_CNT', 'CONFIG_LFS_PROG_SIZE', 'CONFIG_LFS_PAGE_NUM_PER_BLOCK', 'CONFIG_LFS_BLOCK_NUMS' in your board or solution yaml!")
#endif

#if defined(CONFIG_LITTLEFS_CNT) && (CONFIG_LITTLEFS_CNT > 1)
#if !defined(CONFIG_LFS2_BLOCK_NUMS)
#pragma message ("Attention: you have chosen to use more than one littlefs partition. Please double check the default config are suitbale for you; otherwise please define 'CONFIG_LFS2_BLOCK_NUMS' in your board or solution yaml!")
#endif
#endif

#endif /* _LFS_CONF_H_ */
