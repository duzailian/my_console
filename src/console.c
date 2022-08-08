#if TEST_EN
#include "test/test.h"
#else
#include "include.h"
#endif

#include "internal/config.h"
#include "internal/key_map.h"
#include "internal/cmd.inc"

struct __info_t;

typedef enum {// 输入的键类型定义
    #define DEFINE(name) name##_key,
    #include "internal/key_type.h"
    #undef DEFINE
    max_key,// 仅作计数
}key_type_t;


#if 0
typedef enum {// 当前光标所在位置定义
    status_head,
    status_tail,
    status_middle,
    status_max,// 仅作计数
}status_t;
#endif

typedef void (*proc_t) (uint8_t recv, struct __info_t *);

typedef const struct {//状态处理函数定义
    proc_t proc[max_key];
}status_proc_t;

typedef struct __info_t{
    status_proc_t *cur_status;
    char history[HISTORY_MAX][STRING_MAX];
    uint8_t cur_row; // 当前光标所在行号
    uint8_t cur_column;// 当前光标所在列号
    uint8_t display_num;// 屏幕上已显示的字符数
}info_t;

static void cmd_proc (info_t *p_info);

#define put_space(n) do{ putnchar(' ', n); }while(0)

static inline void putnchar(char data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        putchar(data);
    }
    return;
}

static inline void put_backspace(size_t n, bool need_space) {
    putnchar('\b', n);
    if (need_space)
        put_space(n);
    return;
}

static info_t info = {0};

#include "internal/key_proc.inc"

static void get_argv(char *string, int *p_argc, char **argv) {
    uint8_t index = 0;

    /*TODO:为提高程序运行效率，可将strtok()函数替换为将空格替换为\0的函数*/
    argv[0] = strtok(string, " ");
    while (argv[index] && (index < ARGV_MAX)) {
        argv[index] = strtok(NULL, " ");
    }
    return;
}

static void cmd_proc (info_t *p_info) {
    cmd_t *p_cmd;
    int argc;
    char *argv[ARGV_MAX];
    char buffer[STRING_MAX];

    strncpy(buffer, &p_info->history[p_info->cur_row][0], sizeof(buffer));
    get_argv(buffer, &argc, &argv[0]);
    for (p_cmd = &cmds[0]; p_cmd < &cmds[sizeof_array(cmds)]; p_cmd++) {
        if (strcmp(buffer, p_cmd->string) != 0)
            continue;
        p_cmd->proc(argc, argv);
        break;
    }

    if (p_cmd == &cmds[sizeof_array(cmds)])
        printf("cmd not found!");
    fflush(stdout);
    return;
}

static proc_t get_proc_func(uint8_t key) {
    status_proc_t *cur_status = info.cur_status;

    switch (key) {
        case LEFT_KEY: {
            return cur_status->proc[left_key];
        }
        case RIGHT_KEY: {
            return cur_status->proc[right_key];
        }
        case DOWN_KEY: {
            return cur_status->proc[down_key];
        }
        case UP_KEY: {
            return cur_status->proc[up_key];
        }

        case TAB_KEY: {
            return cur_status->proc[tab_key];
        }
        case C_A: {
            return cur_status->proc[head_key];
        }
        case C_E: {
            return cur_status->proc[tail_key];
        }
        case BS_KEY: {
            return cur_status->proc[bs_key];
        }
        case ENTRY_KEY: {
            return cur_status->proc[entry_key];
        }
        default: {
            return cur_status->proc[normal_key];
        }

    }
    return NULL;
}



extern void console_init(void) {
    info_t *p_info = &info;

    p_info->cur_status = status_tail;
    printf("#");
    fflush(stdout);
    return;
}

#if TEST_EN
static struct termios saved_attributes;

static void reset_input_mode (void) {
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
    return;
}

static void set_input_mode (void) {
    struct termios tattr;
    char *name;

    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
    }

    tcgetattr (STDIN_FILENO, &saved_attributes);
    atexit (reset_input_mode);

    tcgetattr (STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON|ECHO);
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
    return;
}

int main (int arc, char **argv) {
    char tmp;
    info_t *p_info = &info;
    proc_t proc_func = NULL;

    set_input_mode ();
    console_init();

    while (1) {
        read (STDIN_FILENO, &tmp, 1);
        // printf("value:0x%x\n", tmp);

        proc_func = get_proc_func(tmp);
        if (proc_func)
            proc_func(tmp, p_info);
        fflush(stdout);
    }
    return 0;
}
#endif

