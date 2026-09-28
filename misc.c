#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include "misc.h"

static pthread_mutex_t print_lock;

///////////////////////////////////////////////////////////////////////////////////////
//
//  void misc_print(int priority, const char* format, ... )
//
void misc_print(int priority, const char* format, ... )
{
    if(priority == 0)
    {
        pthread_mutex_lock(&print_lock);
        va_list args;
        va_start (args, format);
        vprintf (format, args);
        va_end (args);
        pthread_mutex_unlock(&print_lock);
    }
}


///////////////////////////////////////////////////////////////////////////////////////
//
// BOOL misc_check_device (char * deviceName)
//
int misc_check_device (char * deviceName)
{
    struct stat filestat;
    misc_print(0, "Checking for --> '%s' : ", deviceName);
    if (stat(deviceName, &filestat) != 0)
    {
        misc_print(0, "FALSE\n");
        return FALSE;
    }
    misc_print(0, "TRUE\n");
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// BOOL misc_check_device (char * fileName)
//
int misc_check_file(char * fileName)
{
    return misc_check_device(fileName);
}

//////////////////////////////////////////////////////////////////////////////////////
//
// void misc_make_file(const char *filename, const char *data)
//
void misc_write_file(const char *filename, const char *data)
{
	FILE * file;
	file = fopen(filename, "w");
	fwrite(data, strlen(data), 1, file);
	fclose(file);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// BOOL misc_check_args_option (int argc, char *argv[], char * option)
//
int misc_check_args_option (int argc, char *argv[], char * option)
{
    int result = FALSE;
    if(argc > 1)
        for (int i = 1; i< argc; i++)
        {
            if (strcasecmp(argv[i], option) == 0)
            {
                result = i;
                break;
            }
        }
    return result;
}
