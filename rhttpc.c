#include "rhttp.h"
#include <stdio.h>

int main() {
    while(1){
        char * response = rhttp_client_get("127.0.0.1",8888,"/");
        if(response)
        printf("%s\n", response);
    }
    return 0;
}
