#include <stdio.h>
#include <sysrepo.h>
#include <signal.h>
#include <time.h>
#include <libyang/libyang.h>
#include <string.h>
#include "utils.h"

static void cleanup();
static void sigint_handler(int);
static int rpc_handler(sr_session_ctx_t *session, const char *xpath, const sr_val_t *input, const size_t input_cnt,
                       sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

sr_conn_ctx_t* conn = NULL;
sr_session_ctx_t* session = NULL;
int exit_application = 0;

int main()
{
    atexit(cleanup);

    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_RUNNING, &session));

    // Listen for changes
    sr_subscription_ctx_t *subscription = NULL;
    SR_TRY(sr_rpc_subscribe(session, "/rpc:reboot", rpc_handler, NULL, 0, 0, &subscription));

    signal(SIGINT, sigint_handler);

    while (!exit_application) {
        sleep(1000);
    }

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

static void sigint_handler(int sig)
{
    exit_application = 1;
}

static int rpc_handler(sr_session_ctx_t *session, const char *xpath, const sr_val_t *input, const size_t input_cnt,
                       sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
    for (size_t i = 0; i < input_cnt; ++i) {
        if (!strcmp(input[i].xpath, "/rpc:reboot/when-from-now")) {
            char ack_msg[50] = { 0 };

            snprintf(ack_msg, sizeof(ack_msg), "System will reboot in %u seconds.", input[i].data.uint32_val);
            *output = calloc(1, sizeof(**output));
            *output_cnt = 1;
            (*output)[0].type = SR_STRING_T;
            (*output)[0].xpath = strdup("/rpc:reboot/ack");
            (*output)[0].data.string_val = strdup(ack_msg);
        }
    }
    return SR_ERR_OK;
}
