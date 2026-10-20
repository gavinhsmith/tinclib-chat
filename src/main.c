#include <ti/getcsc.h>
#include <ti/screen.h>

#include "tinclib.h"

int main(void) {
    static const tinc_config_t cfg = { "TINCCHAT", 0, TINC_CF_ASCII };
    tinc_err_t err;

    os_ClrHome();
    err = tinc_init(&cfg);
    if (err == TINC_OK && !tinc_isActive(TINC_WIFI))
        err = tinc_openConfig("Chat needs Wi-Fi");
    os_PutStrFull(err == TINC_OK ? "Connected." : tinc_errString(err));

    tinc_shutdown();
    while (!os_GetCSC()) {
    }
    return 0;
}
