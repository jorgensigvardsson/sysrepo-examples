#include <stdio.h>
#include <sysrepo.h>
#include <time.h>
#include "utils.h"

static void cleanup();

sr_conn_ctx_t* conn = NULL;
sr_session_ctx_t* session = NULL;

int main()
{
    atexit(cleanup);
    
    srand(clock());
    
    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_RUNNING, &session));

    sr_val_t new_config_value;
    new_config_value.type = SR_UINT32_T;
    new_config_value.data.uint32_val = rand() % 100;

    // Set item
    SR_TRY(sr_set_item(session, "/observer:config/config-value", &new_config_value, SR_EDIT_DEFAULT));

    // Commit changes in the current session!
    SR_TRY(sr_apply_changes(session, 0, 0));

    return 0;
}

static void cleanup()
{
    if (session)
        sr_session_stop(session);
    session = NULL;

    if (conn)
        sr_disconnect(conn);
    conn = NULL;
}
