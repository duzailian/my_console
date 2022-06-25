#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include <unistd.h>
#include <termios.h>

#define print_string(string) do {printf("%s", string);}while(0)


#define sizeof_array(array) (sizeof(array)/sizeof(array[0]))

#define print_tag()     do{printf("func:%s,line:%d\r\n", __FUNCTION__, __LINE__);}while(0)



