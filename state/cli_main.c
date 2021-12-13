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
    
    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_OPERATIONAL, &session)); // We need to read from the operational store, because it is the combination of ALL config + state!

    // Go through all values in the module and print them
    size_t val_count;
    sr_val_t* vals;
    SR_TRY(sr_get_items(session, "/state:*//.", 0, 0, &vals, &val_count));

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
