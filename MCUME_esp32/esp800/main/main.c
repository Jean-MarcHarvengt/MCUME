#include "esp_system.h"
#include "go.h"


int app_main(void)
{
    setup();
    while(1) {
        loop();        
    }  

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

