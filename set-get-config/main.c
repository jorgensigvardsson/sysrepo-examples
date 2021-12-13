#include <stdio.h>
#include <sysrepo.h>
#include "utils.h"

static void cleanup();

sr_conn_ctx_t* conn = NULL;
sr_session_ctx_t* session = NULL;

int main()
{
    atexit(cleanup);

    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_RUNNING, &session));

    sr_val_t new_config_value;
    new_config_value.type = SR_UINT32_T;

    sr_val_t* config_value = NULL;
    if (SR_ERR_NOT_FOUND == sr_get_item(session, "/set-get-config:config/config-value", 0, &config_value)) {
        // Config value was not found, so set the new value to 1
        new_config_value.type = SR_UINT32_T;
        new_config_value.data.uint32_val = 1;
    } else {
        // Config value was found, so set the new value to it + 1
        new_config_value.data.uint32_val = config_value->data.uint32_val + 1;
    }

    // Set item
    SR_TRY(sr_set_item(session, "/set-get-config:config/config-value", &new_config_value, SR_EDIT_DEFAULT));

    // Free memory if sysrepo allocated it
    if (config_value)
        sr_free_val(config_value);

    // Commit changes in the current session!
    SR_TRY(sr_apply_changes(session, 0, 0));

    // Go through all values in the module and print them
    size_t val_count;
    sr_val_t* vals;
    SR_TRY(sr_get_items(session, "/set-get-config:*//.", 0, 0, &vals, &val_count));

    for (size_t i = 0; i < val_count; ++i) {
        print_val(&vals[i], 1, 0);
        puts("");
    }

    sr_free_values(vals, val_count);

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
