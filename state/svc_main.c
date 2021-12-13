#include <stdio.h>
#include <sysrepo.h>
#include <signal.h>
#include <time.h>
#include <libyang/libyang.h>
#include <string.h>
#include "utils.h"

static void cleanup();
static void sigint_handler(int);
static int get_items(sr_session_ctx_t *session, const char *module_name, const char *path,
                     const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

sr_conn_ctx_t* conn = NULL;
sr_session_ctx_t* session = NULL;
int exit_application = 0;
uint32_t current_state = 0;

int main()
{
    atexit(cleanup);
    srand(clock());

    sr_log_stderr(SR_LL_WRN); // Shove warnings and above onto stderr

    SR_TRY(sr_connect(SR_CONN_DEFAULT, &conn));
    SR_TRY(sr_session_start(conn, SR_DS_RUNNING, &session));

    // Listen for changes
    sr_subscription_ctx_t *subscription = NULL;
    SR_TRY(sr_oper_get_items_subscribe(session, "state", "/state:s", get_items, NULL, 0, &subscription));

    signal(SIGINT, sigint_handler);

    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 100 * 1000 * 1000 // 100 ms
    };

    while (!exit_application) {
        nanosleep(&ts, NULL);
        current_state = rand() % 1000;
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

static int get_items(sr_session_ctx_t *session, const char *module_name, const char *path,
                     const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
    if (!strcmp(module_name, "state") && !strcmp(path, "/state:s")) {
        char buf[10] = { 0 };
        snprintf(buf, sizeof(buf), "%u", current_state);

        *parent = lyd_new_path(NULL, sr_get_context(sr_session_get_connection(session)), "/state:s/value", buf, LYD_ANYDATA_STRING, 0);
    }

    return SR_ERR_OK;
}
