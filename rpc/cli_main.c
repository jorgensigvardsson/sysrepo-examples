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
    SR_TRY(sr_session_start(conn, SR_DS_RUNNING, &session));

    size_t output_cnt;
    sr_val_t* output = NULL;
    sr_val_t input = { 0 };
    input.xpath = "rpc:when-from-now";
    input.type = SR_UINT32_T;
    input.data.uint32_val = 5; // Reboot 5 seconds from now
    SR_TRY(sr_rpc_send(session, "/rpc:reboot", &input, 1, 0, &output, &output_cnt));

    for (size_t i = 0; i < output_cnt; ++i) {
        print_val(&output[i], 1, 0);
        puts("");
    }

    sr_free_values(output, output_cnt);

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
