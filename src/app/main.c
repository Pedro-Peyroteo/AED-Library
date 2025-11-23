#include <stdio.h>
#include "app.h"

int main(void) {
    printf("A iniciar o sistema...\n");
    app_init();
    app_run();
    app_shutdown();
    return 0;
}
