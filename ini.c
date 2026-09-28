#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <error.h>
#include <errno.h>
#include "ini.h"
#include "misc.h"

///////////////////////////////////////////////////////////////////////////////////////
//
// void ini_bool(char * value, int * dest)
//
void ini_bool(char * value, int * dest)
{
    if (strlen(value) > 0)
    {
        char c = toupper(value[0]);
        if (c == 'T' || c == 'Y')
            *dest = TRUE;
        else
            *dest = FALSE;
    }
    else
        *dest = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void int_str(char * dest)
//
void ini_str(char * key, char * value, char * dest, int max)
{
    if(strlen(value) < max)
    {
        //if(strlen(value) > 0)
        strcpy(dest, value);
    }
    else
        misc_print(0, "ERROR: ini_process_key_value() --> '%s' > %d chars!\n", key, max);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void ini_int(char * value, int * dest)
//
void ini_int(char * value, int * dest)
{
    char * endPtr;
    char * p = strchr(value, ' ');
    if (p > 0)
        *p = 0x00;
    int iTmp = strtol(value, &endPtr, 10);
    if(*endPtr == (char) 0x00)
        *dest = iTmp;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void ini_uint(char * value, unsigned int * iValue )
//
void ini_uint(char * value, unsigned int * dest )
{
    char * endPtr;
    char * p = strchr(value, ' ');
    if (p > 0)
        *p = 0x00;
    unsigned int iTmp = strtol(value, &endPtr, 10);
    if(*endPtr == (char) 0x00)
        *dest = iTmp;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void ini_print_settings(int priority)
//
void ini_print_settings(int p)
{
    //misc_print(p, "Settings:\n");
    //misc_print(p, "  - video_mode      --> '%d'\n", video_mode);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// char ini_first_char(char * str, int len)
//
char ini_first_char(char * str, int len)
{
    for (int i = 0; i < len; i++)
        if (str[i] != ' ') return str[i];
    return 0x00;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// int ini_parse_line(char * str, int len, char * key, int keyMax, char * val, int valMax, char * sec, int secMax)
//
int ini_parse_line(char * str, int len, char * key, int keyMax, char * val, int valMax, char * sec, int secMax)
{
    enum mode {mode_sec, mode_val, mode_key};
    int mode   = mode_key;
    int iKey   = 0;
    int iVal   = 0;
    int iSec   = 0;
	char quote = 0;
    keyMax--;
    valMax--;
    secMax--;

    for (int i = 0; i < len; i++)
        switch(str[i])
        {
		case '"': 
			quote ^= 1;
		    break;
        case '#': // COMMENT, break out of for loop
        case ';':
            if (!quote)
				i = len;
            break;
        case '[':
            if(mode == mode_key)
            {
                mode = mode_sec;
                iSec = 0;
            }
            break;
        case ']':
            if (mode == mode_sec)
                mode = mode_key;
            break;
        case '=':
            if(mode == mode_key)
                mode = mode_val;
            break;
        case ' ':
            if (mode == mode_val || mode == mode_sec)
                switch(mode)
                {
                case mode_val:
                    if(iVal && (iVal < valMax))
                        val[iVal++] = str[i];
                    break;
                case mode_sec:
                    if(iSec && (iSec < secMax))
                        sec[iSec++] = str[i];
                    break;
                }
            break;
        case 0x0a: // [NEW LINE]
        case 0x0d: // [RETURN]
        case 0x00: // [NULL]
        case '\t': // [TAB]
            break;
        default :
            switch (mode)
            {
            case mode_val:
                if(iVal < valMax)
                    val[iVal++] = str[i];
                break;
            case mode_key:
                if(iKey < keyMax)
                    key[iKey++] = toupper(str[i]);
                break;
            case mode_sec:
                if (iSec < secMax)
                    sec[iSec++] = toupper(str[i]);
                break;
            }
        }
    key[iKey] = 0x00;
    val[iVal] = 0x00;
    if(iSec)
        sec[iSec] = 0x00;
    if (mode == mode_val)
        return TRUE;
    else
        return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// int ini_read_loop (char * fileName, char * section, int priority, char * key,
//                    int keyMax, char * val, int valMax, char * sec, int secMax)
//
int ini_read_loop (void (*process_key_value_pair) (char * key, char * value),
                   char * fileName, char * section, int priority, char * key,
                   int keyMax, char * val, int valMax, char * sec, int secMax)
{
    int count;
    char str[1024];
    FILE * file;
    file = fopen(fileName, "r");
    sec[0] = 0x00;

    if (file)
    {
        while (fgets(str, sizeof(str), file)!= NULL)
        {
            if(strchr(";#", ini_first_char(str, strlen(str))) == NULL)
                if(ini_parse_line(str, strlen(str), key, keyMax, val, valMax, sec, secMax))
                    if(sec[0] == 0x00 || strcmp(section, sec) == 0)
                        process_key_value_pair(key, val);
        }
        fclose(file);
        return TRUE;
    }
    else
    {
        misc_print(0, "ERROR: ini_read_loop() : Unable to open --> '%s'\n", fileName);
        return FALSE;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// int ini_read_ini(void (*process_key_value_pair) (char * key, char * value), char * fileName, char * section)
//
int ini_read_ini(void (*process_key_value_pair) (char * key, char * value),
                 char * fileName, char * section, int priority)
{
    char key[30];
    char val[1024];
    char sec[30];
    ini_read_loop(process_key_value_pair, fileName, section, priority, key, sizeof(key), val, sizeof(val), sec, sizeof(sec));
}

