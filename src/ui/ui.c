
#include "ui.h"

#include "nano/flow_user_confirm.h"
#include "nano/flow_main_menu.h"
#include "nano/flow_generating_addresses.h"
#include "nano/flow_signed_successfully.h"
#include "nano/flow_signing.h"

#pragma GCC diagnostic error "-Wall"
#pragma GCC diagnostic error "-Wextra"
#pragma GCC diagnostic error "-Wmissing-prototypes"

void ui_reset()
{
}

void ui_init()
{
    flow_init();
    flow_main_menu();
}

void ui_timer_event()
{
    flow_timer_event();
}


