#include <stdio.h>
#include <sysrepo.h>
#include <signal.h>
#include "utils.h"

static void cleanup();
static void sigint_handler(int);
static int subscribe_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath,
                        sr_event_t event, uint32_t request_id, void *private_data);

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
    SR_TRY(sr_module_change_subscribe(session, "observer", NULL, subscribe_cb, NULL, 0, 0, &subscription));

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

static void
print_change(sr_change_oper_t op, sr_val_t *old_val, sr_val_t *new_val)
{
    switch (op) {
    case SR_OP_CREATED:
        printf("CREATED: ");
        print_val(new_val, 1, 1);
        break;
    case SR_OP_DELETED:
        printf("DELETED: ");
        print_val(old_val, 1, 1);
        break;
    case SR_OP_MODIFIED:
        printf("MODIFIED: ");
        print_val(old_val, 1, 1);
        printf(" to ");
        print_val(new_val, 0, 1);
        break;
    case SR_OP_MOVED:
        printf("MOVED: %s\n", new_val->xpath);
        break;
    }
}

static int
print_current_config(sr_session_ctx_t *session, const char *module_name)
{
    sr_val_t *values = NULL;
    size_t count = 0;
    int rc = SR_ERR_OK;
    char *xpath;

    if (asprintf(&xpath, "/%s:*//.", module_name) == -1) {
        return SR_ERR_NOMEM;
    }
    rc = sr_get_items(session, xpath, 0, 0, &values, &count);
    free(xpath);
    if (rc != SR_ERR_OK) {
        return rc;
    }

    for (size_t i = 0; i < count; i++) {
        printf("   ");
        print_val(&values[i], 1, 0);
        puts("");
    }
    sr_free_values(values, count);

    return rc;
}

static const char*
ev_to_str(sr_event_t ev)
{
    switch (ev) {
    case SR_EV_CHANGE:
        return "change";
    case SR_EV_DONE:
        return "done";
    case SR_EV_ABORT:
    default:
        return "abort";
    }
}

static int subscribe_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath,
                        sr_event_t event, uint32_t request_id, void *private_data)
{
    sr_change_iter_t *it = NULL;
    int rc = SR_ERR_OK;
    char path[512];
    sr_change_oper_t oper;
    sr_val_t *old_value = NULL;
    sr_val_t *new_value = NULL;

    printf("\n\n========== EVENT %s START =====================================\n\n", ev_to_str(event));

    if (xpath) {
        sprintf(path, "%s//.", xpath);
    } else {
        sprintf(path, "/%s:*//.", module_name);
    }
    rc = sr_get_changes_iter(session, path, &it);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }

    while ((rc = sr_get_change_next(session, it, &oper, &old_value, &new_value)) == SR_ERR_OK) {
        printf("   ");
        print_change(oper, old_value, new_value);
        puts("");
        sr_free_val(old_value);
        sr_free_val(new_value);
    }

    printf("\n========== EVENT %s END =====================================", ev_to_str(event));

    if (event == SR_EV_DONE) {
        printf("\n\n========== CONFIG HAS CHANGED, RUNNING CONFIG START ==========\n\n");
        if (print_current_config(session, module_name) != SR_ERR_OK) {
            goto cleanup;
        }
        printf("\n\n========== CONFIG HAS CHANGED, RUNNING CONFIG END ==========\n\n");
    }

cleanup:
    sr_free_change_iter(it);
    return SR_ERR_OK;
}

static void sigint_handler(int sig)
{
    exit_application = 1;
}