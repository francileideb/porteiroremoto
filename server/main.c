#include "httpd.h"
#include <mysql.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

//---Macros---//

#define GPIO4 	4
#define HIGH    1
#define LOW 	0
#define INPUT   0
#define OUTPUT  1

//---Prototipos das funcoes---//

bool access_gpio(int pin);
bool export_gpio(int pin);
bool direction_gpio(int pin, int direction);
bool value_gpio(int pin, int value);
bool unexport_gpio(int pin);
void delay(float time);
void finalization(int nsingnal);
int getUser(char uname[255], char pass[255]);
int abrir (char id[255]);


//---Variaveis Globais--------//
int arquive, pin=GPIO4;
float timeSleep=0.5;
char buffer[3];
char path[35];
//----------------------------//

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
        printf("Access-Control-Allow-Origin: http://172.29.58.40\r\n\r\n");
        char uname[255]={'\0'};
        char pass[255]={'\0'};
        char *init = strchr(qs,'=');
        char *end = strchr(qs,'&');
        strncpy(uname,init+1,end-init-1);
        init = strrchr(qs,'=');
        end = &qs[strlen(qs)-1];
        strncpy(pass,init+1,end-init);
        int i;
        i=getUser(uname,pass);
    }

    ROUTE_GET("/abrir")
    {
        printf("HTTP/1.1 200 OK\r\n");
        printf("Access-Control-Allow-Origin: http://172.29.58.40\r\n\r\n");
        char id[255]={'\0'};
        char *init = strchr(qs,'=');
        char *end = &qs[strlen(qs)-1];
        strncpy(id,init+1,end-init);
        int i;
        i=abrir(id);
        i=abrir(id);
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

int abrir (char id[255])
{
        char command[255];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char dateTime[255];
        sprintf(dateTime,"%d-%02d-%02d_%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
        sprintf(command,"fswebcam /home/pi/Documents/porteiroremoto/www-data/%s_%s.jpg",id,dateTime);
	signal(SIGINT, finalization);
	// === Verifica se o pino ja foi exportado === //
	if(access_gpio(pin))
  {
		// === Export do Pino === //
		if (export_gpio(pin))
		{
		// === Configurando Direcao do Pino === //
			if(direction_gpio(pin, OUTPUT))
			{

				system(command);
					sleep(1);
			}
			else
			{
				printf("Ocorreu um problema na configuracao do pino como I/O\n");
				return 2;
			}
			 // === Desvinculando o pino == //
			if(unexport_gpio(pin))
			{
					return 0;
			}
			else
			{
					printf("Ocorreu um problema para finalizar a utilizacao do pino\n");
					return 2;
			}

		}
		else
		{
			printf("Ocorreu um problema no export do pino\n");
                        return 1;
		}
	}
	else
	{
		printf("Pino ja foi exportado! \n");
		delay(0.5);
		printf("Configurando as caracteristicas de utilizacao! \n");
		delay(0.5);

		if(direction_gpio(pin, OUTPUT))
		{
			system(command);
			printf("GPIO%d configurada como OUTPUT! \n", pin);
			delay(0.5);
			printf("Iniciando o processo de acionamento\n");
			delay(0.5);
			if(unexport_gpio(pin))
			{
				return 0;
			}
			else
			{
				printf("Ocorreu um problema para finalizar a utilizacao do pino\n");
				return 2;
			}
	  	}
		else
		{
				printf("Ocorreu um problema na configuracao do pino como I/O\n");
				return 2;
		}
	}
}

void finalization(int nsignal)
{
	// === Desvinculando o pino == //

        if(unexport_gpio(pin))
        {
		delay(0.5);
                printf("\nAplicando um unexport no pino\n");
		delay(0.5);
		printf("Programa finalizado...\n");
		exit(0);
        }
}

bool access_gpio(int pin)
{
        snprintf(path, 35, "/sys/class/gpio/gpio%d/direction", pin);
        if (access(path, 0) == -1) // Arquivo nao existe
        {
                return true;
        }
        else // Arquivo existe
        {
                return false;
        }
}

bool export_gpio(int pin)
{
        arquive = open ("/sys/class/gpio/export", O_WRONLY);
        if (arquive==-1)
        {
                printf("Arquivo abriu incorretamente\n");
                return false;
        }
        snprintf(buffer, 3, "%d", pin);
        if(write(arquive, buffer, 3) == -1)
        {
                close(arquive);
                return false;
        }

        close(arquive);

	return true;
}

bool direction_gpio(int pin, int direction)
{
	arquive=0;
        snprintf(path, 35, "/sys/class/gpio/gpio%d/direction", pin);
        arquive = open (path, O_WRONLY);
        if (arquive==-1)
        	return false;

        snprintf(buffer, 3, "%d", pin);
        if (write( arquive, ((direction == INPUT)?"in":"out"), 3 )==-1)
        {
                close(arquive);
                return false;
        }
        close(arquive);
	return true;
}

bool value_gpio(int pin, int value)
{
	arquive=0;
        snprintf(path, 35, "/sys/class/gpio/gpio%d/value", pin);
        arquive = open(path, O_WRONLY);
        if (arquive == -1)
        {
                return false;
        }
        if (write (arquive, ((value == HIGH)?"1":"0"), 1) == -1)
        {
                close(arquive);
                return false;
        }
	close(arquive);
	return true;
}

bool unexport_gpio(int pin)
{
	arquive = open ("/sys/class/gpio/unexport", O_WRONLY);
        if (arquive==-1)
        {
                printf("Arquivo abriu incorretamente\n");
                return false;
        }
        if(write(arquive, buffer, 3) == -1)
        {
                close(arquive);
                return false;
        }
        return true;
}

void delay(float time)
{
	struct timespec t;
	int seg;
	seg = time;
	t.tv_sec = seg;
	t.tv_nsec = (time-seg)*1e9;
	nanosleep(&t, NULL);
}