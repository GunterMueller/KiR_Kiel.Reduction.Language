/*******************************************************/
/*                                                     */
/* rncheap.h - include file for the nCube heap manager */
/*                                                     */
/*******************************************************/


#define GET_CODE_START()  heap_code_start
#define SET_CODE_START(data) heap_code_start = (INSTR *)(data)

#define IS_STATIC(addr)             (((addr) <= static_heap_upper_border) && ((addr) >= static_heap_lower_border))
#define STATIC_OFFSET(addr)         (static_heap_upper_border - (addr))
#define REAL_STATIC_ADDRESS(offset) (static_heap_upper_border - (offset)) 
#define SET_STATIC_BORDER(addr)     (static_heap_upper_border = (addr))
