/*
 * Chat client. The top panel shows messages, polled from the server every two
 * seconds. Type in the bottom field: the first [enter] sets the server
 * (host:port, saved for next time), the second your name, and after that each
 * one sends a message. [clear] on an empty field quits.
 */

#include <ctype.h>
#include <fileioc.h>
#include <stdlib.h>
#include <string.h>
#include <ti/getcsc.h>
#include <ti/screen.h>

#include "tinclib.h"
#include "titrm.h"

/* Appvar holding the last server address. */
#define SAVE_VAR "TCHATSRV"

#define TICK_MS    100
#define POLL_TICKS 20 /* fetch every 2 s */

static term_panel_t *log_p, *input_p;

static char host[TERM_INPUT_MAX + 1];
static char name[17];
static char url[100];
static char post[80];          /* streamed from during tinc_poll(): keep static */
static char line[256];
static uint16_t line_len;
static unsigned long last_id;  /* newest message shown */

static bool busy, sending, want_send;
static uint8_t idle;
static tinc_err_t last_err;

/* One "id\tname\ttext" line from GET /messages. */
static void got_line(void) {
    char *who, *text;

    line[line_len] = '\0';
    line_len = 0;
    if (!(who = strchr(line, '\t')) || !(text = strchr(++who, '\t')))
        return;
    *text++ = '\0';
    last_id = strtoul(line, NULL, 10);
    term_text_appendf(log_p, TERM_S_BOLD "%s:" TERM_S_NORMAL " %s\n", who, text);
}

static void start(tinc_method_t method, const char *body) {
    tinc_request_t req = { method, url, NULL, body, body ? strlen(body) : 0 };

    tinc_request(&req);
    busy = true;
    sending = method == TINC_POST;
    idle = 0;
}

/* Drives one request at a time: a pending send first, else a poll. */
static void net_tick(void) {
    if (!host[0])
        return;
    if (busy) {
        tinc_state_t st = tinc_poll();
        char buf[64];
        int16_t n;

        while ((n = tinc_read(buf, sizeof buf)) > 0) {
            if (sending || tinc_httpStatus() != 200)
                continue;
            for (int16_t i = 0; i < n; i++) {
                if (buf[i] == '\n')
                    got_line();
                else if (line_len < sizeof line - 1)
                    line[line_len++] = buf[i];
            }
        }
        if (st == TINC_DONE || st == TINC_ERROR) {
            tinc_err_t err = st == TINC_ERROR ? tinc_error() : TINC_OK;

            /* Say so once, not on every poll while the server is down. */
            if (err && err != last_err)
                term_text_appendf(log_p, "! %s\n", tinc_errString(err));
            last_err = err;
            busy = false;
            line_len = 0;
            if (sending)
                idle = POLL_TICKS; /* fetch our message right away */
        }
        return;
    }
    if (want_send) {
        want_send = false;
        term_snprintf(url, sizeof url, "http://%s/messages", host);
        start(TINC_POST, post);
    } else if (++idle >= POLL_TICKS) {
        term_snprintf(url, sizeof url, "http://%s/messages?after=%lu", host, last_id);
        start(TINC_GET, NULL);
    }
}

static bool on_event(term_ctx_t *ctx, const term_event_t *ev, void *state) {
    (void)state;
    switch (ev->type) {
    case TERM_EV_TICK:
        net_tick();
        return true;
    case TERM_EV_SUBMIT: {
        const char *text = term_input_text(input_p);

        if (!*text || want_send)
            return true;
        if (!host[0]) {
            uint8_t h;
            char *c;

            strcpy(host, text);
            for (c = host; *c; c++)
                *c = tolower((unsigned char)*c);
            if ((h = ti_Open(SAVE_VAR, "w"))) {
                ti_Write(host, strlen(host), 1, h);
                ti_SetArchiveStatus(true, h);
                ti_Close(h);
            }
            term_panel_set_title(input_p, "Your name");
        } else if (!name[0]) {
            strncpy(name, text, sizeof name - 1);
            term_panel_set_title(input_p, "Message");
        } else {
            term_snprintf(post, sizeof post, "%s\n%s", name, text);
            want_send = true;
        }
        term_input_set(input_p, "");
        return true;
    }
    case TERM_EV_KEY:
        if (ev->key == TERM_KEY_CLEAR) {
            term_quit(ctx, 0);
            return true;
        }
        return false;
    default:
        return false;
    }
}

int main(void) {
    static const tinc_config_t cfg = { "TINCCHAT", 0, TINC_CF_ASCII };
    tinc_err_t err;
    term_ctx_t *ctx;

    os_ClrHome();
    err = tinc_init(&cfg);
    if (err == TINC_OK && !tinc_isActive(TINC_WIFI))
        err = tinc_openConfig("Chat needs Wi-Fi");
    if (err != TINC_OK) {
        os_PutStrFull(tinc_errString(err));
        tinc_shutdown();
        while (!os_GetCSC()) {
        }
        return 0;
    }

    ctx = term_init();
    log_p = term_split(term_root(ctx), TERM_VERTICAL, TERM_FILL);
    term_panel_set_border(log_p, true);
    term_panel_set_title(log_p, "tinclib chat");
    term_make_text(log_p, "Enter the server as host:port (the PC running "
                          "server.js, e.g. 192.168.1.5:3000), then your name.\n");
    term_text_limit(log_p, 2048);
    term_text_autoscroll(log_p, true);

    input_p = term_split(term_root(ctx), TERM_VERTICAL, TERM_FIXED(3));
    term_panel_set_border(input_p, true);
    term_panel_set_title(input_p, "Server (host:port)");
    term_make_input(input_p);
    {
        char saved[TERM_INPUT_MAX + 1] = "";
        uint8_t h = ti_Open(SAVE_VAR, "r");

        if (h) {
            ti_Read(saved, 1, sizeof saved - 1, h);
            ti_Close(h);
        }
        term_input_set(input_p, saved);
    }
    term_focus(ctx, input_p);

    term_set_tick(ctx, TICK_MS);
    idle = POLL_TICKS; /* first fetch on the first tick */
    term_run(ctx, on_event, NULL);
    term_shutdown(ctx);
    tinc_shutdown();
    return 0;
}
