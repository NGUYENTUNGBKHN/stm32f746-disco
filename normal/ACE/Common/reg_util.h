/**
 * @file       reg_util.h
 * @brief      
 * @date       2025/10/19
 * @author     [Gentantun] (nguyenthanhtung8196@gmail.com)
 * @details    
 * @ref        
 * @copyright  Copyright (c) 2025 RoboTun
*/
#ifndef _REG_UTIL_H_
#define _REG_UTIL_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */

/* Register bit manipulation macros */

/* Write a val to address of register reg */
#define REG_WRITE(reg, val)                 ((reg) = (val))
/* Read value of address register reg */
#define REG_READ(reg)                       ((reg))
/* Set at bit position of reg */
#define REG_SET_BIT(reg, pos)               ((reg) |= (1 << (pos)))
/* Clear at bit position of reg */
#define REG_CLR_BIT(reg, pos)               ((reg) &= ~(1 << (pos)))
/* Read at bit postion of reg */
#define REG_READ_BIT(reg, pos)              ((reg) & (1 << (pos)))

#define REG_CLR_VAL(reg, clrmask, pos)      ((reg) &= ~(clrmask << (pos)))

#define REG_SET_VAL(reg, val, setmask, pos) do{\
                                                REG_CLR_VAL(reg, setmask, pos);\
                                                ((reg) |= ((val) << (pos)));\
                                            }while(0)


#ifdef __cplusplus
}
#endif
#endif
