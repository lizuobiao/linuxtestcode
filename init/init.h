#ifndef _INIT_H_
#define _INIT_H_
typedef void (*init_call)(void);

#define _init __attribute__((unused, section(".myinit")))
#define DECLARE_INIT(func) init_call _fn_##func _init = func

void do_initcalls(void);

#endif