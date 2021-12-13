#include <stdio.h>
#include <sysrepo.h>
#include <signal.h>
#include <time.h>
#include <libyang/libyang.h>
#include <string.h>
#include "utils.h"

static void cleanup();
static void sigint_handler(int);
static void update_operational_datastore_with_state(uint32_t state);

sr_conn_ctx_t* conn = NULL;
sr_session_ctx_t* session = NULL;
int exit_application = 0;

int main()
{
    atexit(cleanup);
    srand(clock());

    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_OPERATIONAL, &session));

    signal(SIGINT, sigint_handler);

    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 100 * 1000 * 1000 // 100 ms
    };

    while (!exit_application) {
        update_operational_datastore_with_state(rand() % 1000);
        nanosleep(&ts, NULL);        
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

static void update_operational_datastore_with_state(uint32_t state)
{
    sr_val_t new_state_value = { 0 };
    new_state_value.type = SR_UINT32_T;
    new_state_value.data.uint32_val = state;

    SR_TRY(sr_set_item(session, "/state:status/value", &new_state_value, 0));
    SR_TRY(sr_apply_changes(session, 0, 0));
}