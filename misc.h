#define TRUE 1
#define FALSE 0

void misc_print(int priority, const char* format, ... );
int misc_check_device (char * deviceName);
int misc_check_file(char * fileName);
void misc_write_file(const char *filename, const char *data);
int misc_check_args_option (int argc, char *argv[], char * option);

