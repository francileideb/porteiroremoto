#include "httpd.h"
#include <mysql.h>
#include <stdio.h>
int getUser(char uname[255], char pass[255]);

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
        int i;
        i=getUser(uname,pass);
    }

    ROUTE_POST("/")
    {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
        printf("Fetch the data using `payload` variable.");
    }
  
    ROUTE_END()
}

int getUser(char uname[255], char pass[255])
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

 char *server = "localhost";
 char *user = "root";
 char *password = "slfpr07c"; /*password is not set in this example for security*/
 char *database = "porteiroremoto";

 conn = mysql_init(NULL);

 /* Connect to database */
 if (!mysql_real_connect(conn, server,
 user, password, database, 0, NULL, 0))
 {
   printf("Failed to connect MySQL Server %s. Error: %s\n", server, mysql_error(conn));
   return 0;
 }

 
 /* send SQL query */
 char querryString[255]="select id from users where uname=\'";
 strcat(querryString,uname);
 strcat(querryString,"\' and pass=\'");
 strcat(querryString,pass);
 strcat(querryString,"\' ;");
 if (mysql_query(conn, querryString))
 {
   printf("Failed to execute quesry. Error: %s\n", mysql_error(conn));
   return 0;
 }

 res = mysql_store_result(conn);
 if (res == NULL)
 {
   return 0;
 }

 int columns = mysql_num_fields(res);

 int i = 0;

//  printf("Entries in the table my_table:\n");
 if(row = mysql_fetch_row(res))
 {
   for (i = 0; i < columns; i++)
   {
     printf("%s ", row[i] ? row[i] : "NULL");
   }
   printf("\n");
 }
 else{
     printf("-1");
 }

 mysql_free_result(res);
 mysql_close(conn);

 return 1;
}