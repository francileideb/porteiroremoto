#include "httpd.h"

int main(int c, char** v)
{
    serve_forever("12913");
    return 0;
}

void route()
{
    ROUTE_START()

    ROUTE_GET("/login")
    {
        printf("HTTP/1.1 200 OK\r\n");
        printf("Access-Control-Allow-Origin: http://192.168.0.178\r\n\r\n");
        char uname[255];
        char pass[255];
        char *init = strchr(qs,'=');
        char *end = strchr(qs,'&');
        strncpy(uname,init+1,end-init-1);
        init = strrchr(qs,'=');
        end = &qs[strlen(qs)-1];
        strncpy(pass,init+1,end-init);
        
    }

    ROUTE_POST("/")
    {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
        printf("Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}