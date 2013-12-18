#ifndef S_BITSET_H_
#define S_BITSET_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * 用ANSI C实现一个bitset
*/
typedef uint32_t                         s_bitset_slot;
#define S_BITSET_SLOT_LENGTH             (sizeof(s_bitset_slot) * 8)
#define S_BITSET_DEFAULT_BITS            (sizeof(s_bitset_slot) * 8)
#define S_BITSET_SLOT_NUM(num)           (((num) + 31) / (S_BITSET_SLOT_LENGTH))
#define S_BITSET_BYTE_LENGTH(num)        ((((num) + 31) / 32) * 4)
#define S_BITSET_SLOT_MASK               0x1f
#define S_BITSET_ALL_TRUE_MASK           0xffffffff
#define S_BITSET_ALL_FALSE_MASK          0
#define S_BITSET_NEXT_POW_2(next,cur)    next=cur;\
                                         next--;\
                                         next|=next>>1;\
                                         next|=next>>2;\
                                         next|=next>>4;\
                                         next|=next>>8;\
                                         next|=next>>16;\
                                         next++;

#define S_BITSET_FATAL(msg) \
    fprintf(stderr, "%s:%d  bitset error: %s\n", __FILE__, __LINE__,  msg); \
    exit(EXIT_FAILURE)

typedef struct s_bitset_t {
    s_bitset_slot *slots;          //bitset的存储
    size_t bits_num;               //bitset最多可容纳的位数
    size_t bits_seted;            //bitset中1的个数
} s_bitset;

/**
 * 新建一个bitset
 */
 s_bitset *s_bitset_create(void);

/**
 * 释放一个bitset的空间
 */
 void s_bitset_free(s_bitset *);
 
/**
 * 获取一个bitset的字节数
 */
 size_t s_bitset_length(const s_bitset *);
 
/**
 * 复制一个bitset
 */
 s_bitset *s_bitset_copy(const s_bitset *);
  
 /**
 * 检查是否有任何一位设置为1
 */
 bool s_bitset_any(const s_bitset *);
  
 /**
 * 检查被设置为1的个数
 */
 size_t s_bitset_count(const s_bitset *);
 
 /**
 * 反置bitset，指定pos上的位被反置
 */
 void s_bitset_flip_pos(s_bitset *, size_t pos);
 
  /**
 * 反置bitset的每一位
 */
 void s_bitset_flip(s_bitset *);
 
 /**
 * 检查是否所有位都没有被设置为1
 */
 bool s_bitset_none(const s_bitset *);
 
 /**
 * 重置bitset，指定pos上的位被重置为0
 */
 void s_bitset_reset_pos(s_bitset *, size_t);
 
 /**
 * 重置bitset的每一位为0
 */
 void s_bitset_reset(s_bitset *);
 
 /**
 * 设置bitset，指定pos上的位被设置为1
 */
 void s_bitset_set_pos(s_bitset *, size_t);
 
 /**
 * 设置bitset的每一位为1
 */
 void s_bitset_set(s_bitset *);
 
 /**
 * 获取一个bitset所能容纳的位
 */
 size_t s_bitset_size(const s_bitset *);
 
 /**
 * 返回pos位上的值
 */
 bool s_bitset_test(const s_bitset *, size_t);
 
 /**
 * 返回bitset的字符串形式
 */
 char *s_bitset_to_string(const s_bitset *);
 
 /**
 * 返回bitset的无符号长整数形式
 */
 size_t s_bitset_to_ulong(const s_bitset *);
 
#ifdef __cplusplus
}
#endif

#endif
