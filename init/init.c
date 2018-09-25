#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "init.h"

extern init_call _init_start;
extern init_call _init_end;

static void A_init(void)
{
    write(1, "A_init\n", sizeof("A_init\n"));
}

static void B_init(void)
{
    printf("B_init\n");
}

static void C_init(void)
{
    printf("C_init\n");
}
DECLARE_INIT(C_init);
DECLARE_INIT(B_init);
DECLARE_INIT(A_init);

void do_initcalls(void)
{
    init_call *init_ptr = &_init_start;
    for (; init_ptr < &_init_end; init_ptr++) {
 //       printf("init address: %p\n", init_ptr);
        (*init_ptr)();
    }
}

int main(void)
{
    do_initcalls();
    return 0;
}