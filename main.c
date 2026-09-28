#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <ctype.h>
#include "misc.h"
#include "ini.h"

static char MiSTer_INI[] = "/media/fat/MiSTer.ini";
//static char MiSTer_INI[] = "MiSTer.ini";
static char MiSTer_CMD[] = "/dev/MiSTer_cmd";

// MiSTer default video modes:
// cribbed from: https://github.com/MiSTer-devel/Main_MiSTer/blob/master/video.cpp
struct vmode_t
{
    char * num;   // but we add a num, lable, width, height
    char * label; // to make the coding cleaner.
    uint32_t width;
    uint32_t height;
    uint32_t vpar[8];
    double Fpix;
    uint8_t vic_mode;
    uint8_t pr;
};


struct vmode_t vmodes[] =
{
    {"0",  "1280x720@60",  1280,  720, { 1280, 110,  40, 220,  720,  5,  5, 20 },  74.25,  4, 0 },
    {"1",  "1024x768@60",  1024,  768, { 1024,  24, 136, 160,  768,  3,  6, 29 },  65,     0, 0 },
    {"2",  "720x480@60",    720,  480, {  720,  16,  62,  60,  480,  9,  6, 30 },  27,     3, 0 },
    {"3",  "720x576@50",    720,  576, {  720,  12,  64,  68,  576,  5,  5, 39 },  27,    18, 0 },
    {"4",  "1280x1024@60", 1280, 1024, { 1280,  48, 112, 248, 1024,  1,  3, 38 }, 108,     0, 0 },
    {"5",  "800x600@60",    800,  600, {  800,  40, 128,  88,  600,  1,  4, 23 },  40,     0, 0 },
    {"6",  "640x480@60",    640,  480, {  640,  16,  96,  48,  480, 10,  2, 33 },  25.175, 1, 0 },
    {"7",  "1280x720@50",  1280,  720, { 1280, 440,  40, 220,  720,  5,  5, 20 },  74.25, 19, 0 },
    {"8",  "1920x1080@60", 1920, 1080, { 1920,  88,  44, 148, 1080,  4,  5, 36 }, 148.5,  16, 0 },
    {"9",  "1920x1080@50", 1920, 1080, { 1920, 528,  44, 148, 1080,  4,  5, 36 }, 148.5,  31, 0 },
    {"10", "1366x768@60",  1366,  768, { 1366,  70, 143, 213,  768,  3,  3, 24 },  85.5,   0, 0 },
    {"11", "1024x600@60",  1024,  600, { 1024,  40, 104, 144,  600,  1,  3, 18 },  48.96,  0, 0 },
    {"12", "1920x1440@60", 1920, 1440, { 1920,  48,  32,  80, 1440,  2,  4, 38 }, 185.203, 0, 0 },
    {"13", "2048x1536@60", 2048, 1536, { 2048,  48,  32,  80, 1536,  2,  4, 38 }, 209.318, 0, 0 },
    {"14", "2560x1440@60", 2560, 1440, { 1280,  24,  16,  40, 1440,  3,  5, 33 }, 120.75,  0, 1 },
    {"15", "400x300@60",    400,  300, {  800,  40, 128,  88,  600,  1,  4, 23 },  40,     0, 0 },
    {"16", "320x240@60",    320,  240, {  640,  16,  96,  48,  480, 10,  2, 33 },  25.175, 1, 0 },
    {"17", "320x200@60",    320,  200, {  640,  16,  96,  48,  480, 10,  2, 33 },  25.175, 1, 0 },
};
#define VMODES_NUM (sizeof(vmodes) / sizeof(vmodes[0]))

///////////////////////////////////////////////////////////////////////////////////////
//
// int find_based_on(int vmode)
// find the mode this the scaled mode is based on. - or not -1
//
int find_based_on(int vmode)
{
	 for(int i = 0; i < VMODES_NUM; i++)
		 if (memcmp(vmodes[vmode].vpar, vmodes[i].vpar, sizeof(uint32_t[8])) == 0 &&
	         vmodes[vmode].Fpix == vmodes[i].Fpix &&
			 vmodes[vmode].vic_mode == vmodes[i].vic_mode &&
			 vmodes[vmode].pr == vmodes[i].pr)
			 return i;
     return -1;
}
///////////////////////////////////////////////////////////////////////////////////////
//
// void process_key_value_pair(char * key, char * value)
//
static unsigned int video_mode_int = -1;
static char video_mode_str[256]    = "";

void process_key_value_pair(char * key, char * value)
{
	//misc_print(0, "--> KEY = '%s' VALUE= '%s'\n", key, value);
    if(strcmp("VIDEO_MODE", key) == 0)
    {
        if (strchr(value, ',') != NULL)
        {
            video_mode_int = -1;
            ini_str(key, value, video_mode_str, sizeof(video_mode_str));
        }
        else
        {
            ini_uint(value, &video_mode_int);
            video_mode_str[0] = (char) 0x00;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void ini_print_settings(int priority)
//
void print_settings(int p)
{
    misc_print(p, "MiSTer.INI Settings:\n\n");
    // if (video_mode_str[0] != (char) 0x00)
    misc_print(p, "  - video_mode(str) --> '%s'\n", video_mode_str);
    // else
    misc_print(p, "  - video_mode(int) --> '%d'\n", video_mode_int);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void process_ini()
//
void process_ini()
{
    if(misc_check_file(MiSTer_INI)) //read the INI file to recover correct video mode
    {
        ini_read_ini(process_key_value_pair, MiSTer_INI, "", 0);
        ini_read_ini(process_key_value_pair, MiSTer_INI, "MISTER", 0);
        ini_read_ini(process_key_value_pair, MiSTer_INI, "MENU", 0);
        print_settings(0);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void show_help(int argc, char *argv[])
//
void show_help(int argc, char *argv[])
{
    process_ini();
    if(argc > 0)
        misc_print(0, "\nUSAGE:\n\t%s:\n\n", argv[0]);
    else
        misc_print(0, "\nUSAGE:\nb\tbvmode:\n\n");
	char scaledStr[128] = "";
    for(int vmode = 0; vmode < VMODES_NUM; vmode++)
	{
		if (vmodes[vmode].height != vmodes[vmode].vpar[4])
		{
			int bomode = find_based_on(vmode);
			sprintf(scaledStr,  " - scaled from %s", bomode == -1?"?":vmodes[bomode].label);	
		}
		misc_print(0, "\t\t%s%s or %s%s\n", vmode < 10?" ":"", vmodes[vmode].num, vmodes[vmode].label, scaledStr);
	}
    misc_print(0, "\t\tRGB16 or RGB32 or IDX8\n");
    if (video_mode_int != -1)
        misc_print(0, "\t\tMENU (%d=%s)\n", video_mode_int, vmodes[video_mode_int].label);
    else if (video_mode_str[0] != (char) 0x00)
        misc_print(0, "\t\tMENU (%s)\n", video_mode_str);
    else
        misc_print(0, "\t\tMENU (ERROR: unable to parse '%s')\n", MiSTer_INI);

    misc_print(0, "\n\thttps://github.com/bbond007/bbvmode\n");
}

///////////////////////////////////////////////////////////////////////////////////////
//
// get_arg_vmode_by_lbl(int argc, char *argv[])
//
int get_arg_vmode_by_lbl(int argc, char *argv[])
{
    if(argc > 1)
        for (int i = 1; i< argc; i++)
            for (int vmode = 0; vmode < VMODES_NUM; vmode++)
                if (strcasecmp(argv[i], vmodes[vmode].label) == 0)
                    return vmode;
    return -1;
}
///////////////////////////////////////////////////////////////////////////////////////
//
// get_arg_vmode_by_num(int argc, char *argv[])
// 
int get_arg_vmode_by_num(int argc, char *argv[])
{
    if(argc > 1)
        for (int i = 1; i< argc; i++)
            for (int vmode = 0; vmode < VMODES_NUM; vmode++)
                if (strcasecmp(argv[i], vmodes[vmode].num) == 0)
                    return vmode;
    return -1;
}


///////////////////////////////////////////////////////////////////////////////////////
//
// parse_rez(char * str, unsigned int * width, unsigned int * height)
// parse width and height from monitor string
int parse_rez(char * str, unsigned int * width, unsigned int * height)
{
    int c = 0;
    char widthStr[7]     = "";
    char heightStr[7]    = "";
    unsigned int iWidth  = 0;
    unsigned int iHeight = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ',')
            c++;
        else if (isdigit(c))
            switch(c)
            {
            case 0:
                if (iWidth < sizeof(widthStr))
                    widthStr[iWidth++] = str[i];
                break;
            case 4:
                if (iHeight < sizeof(heightStr))
                    heightStr[iHeight++] = str[i];
                break;
            }
    }
    widthStr[iWidth]   = (char) 0x00;
    heightStr[iHeight] = (char) 0x00;
    ini_uint(widthStr,  width);
    ini_uint(heightStr, height);

    if( width > 0 && height > 0)
        return TRUE;
    else
        return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void MiSTer_cmd_write(char * buf)
//
void MiSTer_cmd_write(char * buf)
{
    if (misc_check_device(MiSTer_CMD))
    {
        misc_write_file(MiSTer_CMD, buf);
        misc_print(0, "%s\n", buf);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// void do_vmode(char * buf, unsigned int width, unsigned int height, char * pix_format)
//
void do_vmode(char * buf, unsigned int width, unsigned int height, char * pix_format)
{
    misc_print(0, "Setting framebuffer to --> %dx%d %s\n", width, height, pix_format);
    sprintf(buf, "vmode -r %d %d %s", width, height, pix_format);
    system(buf);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// int main(int argc, char *argv[])
//
int main(int argc, char *argv[])
{
    char buf[512];
    char pix_format[6] = "rgb16";

    if(argc > 1)
    {
        int vmode = -1;
        int bMenu = misc_check_args_option(argc, argv, "MENU");

        if (bMenu)
        {
            process_ini();
            vmode = video_mode_int;
            strcpy(pix_format, "rgb32");
        }
        else
        {
            vmode =  get_arg_vmode_by_lbl(argc, argv);
            if (vmode == -1)
                vmode =  get_arg_vmode_by_num(argc, argv);
        }

        if (misc_check_args_option(argc, argv, "RGB16"))
            strcpy(pix_format, "rgb16");
        if (misc_check_args_option(argc, argv, "RGB32"))
            strcpy(pix_format, "rgb32");
        if (misc_check_args_option(argc, argv, "IDX8"))
            strcpy(pix_format, "idx8");

        if (vmode != -1)
        {
            uint32_t pix = roundf(vmodes[vmode].Fpix * 1000);
            sprintf(buf, "video_mode %" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32,
                    vmodes[vmode].vpar[0],
                    vmodes[vmode].vpar[1],
                    vmodes[vmode].vpar[2],
                    vmodes[vmode].vpar[3],
                    vmodes[vmode].vpar[4],
                    vmodes[vmode].vpar[5],
                    vmodes[vmode].vpar[6],
                    vmodes[vmode].vpar[7],
                    pix);
            misc_print(0, "Setting core resolution to --> %dx%d\n", vmodes[vmode].vpar[0], vmodes[vmode].vpar[4]);
            MiSTer_cmd_write(buf);
            do_vmode(buf, vmodes[vmode].width, vmodes[vmode].height, pix_format);
        }
        else if ((video_mode_str[0] != (char) 0x00) && bMenu)
        {
            sprintf(buf, "video_mode %s", video_mode_str);
            MiSTer_cmd_write(buf);
            unsigned int width;
            unsigned int height;
            if (parse_rez(video_mode_str, &width, &height))
                do_vmode(buf, width, height, pix_format);
        }
        else
            misc_print(0, "SYNTEX ERROR! bummer :(\n");
    }
    else
        show_help(argc, argv);
}


