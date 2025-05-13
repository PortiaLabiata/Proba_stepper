#include "arch/cc.h"

static volatile uint32_t nesting = 0;
static sys_prot_t primask_state = 0;

sys_prot_t sys_arch_protect(void) {
    if (nesting == 0) {
        primask_state = __get_PRIMASK();
        __disable_irq();
    }
    return nesting++;
}

sys_prot_t sys_arch_unprotect(void) {
    if (--nesting == 0) {
        __set_PRIMASK(primask_state);
    }
    return nesting+1;
}