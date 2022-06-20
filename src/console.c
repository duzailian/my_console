#if TEST_EN
#include "test/test.h"
#else
#include "include.h"
#endif

#include "internal/config.h"
#include "internal/key_map.h"
#include "internal/cmd.inc"

struct __info_t;

typedef void (*fsm_t) (struct __info_t *);

typedef struct __info_t{
    fsm_t fsm;
    char history[HISTORY_MAX][STRING_MAX];
    uint8_t cur_row; // 当前光标所在行号
    uint8_t cur_column;// 当前光标所在列号
    uint8_t display_num;// 屏幕上已显示的字符数
}info_t;

static void cmd_proc (info_t *p_info);

static info_t info = {0};

static void auto_completion(info_t *p_info) {
    return;
}

static inline void back_space(size_t n) {
    for (int i = 0; i < n; i++)
        putchar('\b');
    return;
}

static void char_proc(const char recv, info_t *p_info) {
    switch(recv) {
        case BS_KEY: {
            if ((p_info->display_num == 0) || (p_info->cur_column == 0))
                break;

            p_info->cur_column--;
            p_info->display_num--;
            if (p_info->cur_column != p_info->display_num) {// 光标未在字符串尾
                memmove(&p_info->history[p_info->cur_row][p_info->cur_column],
                    &p_info->history[p_info->cur_row][p_info->cur_column + 1], p_info->display_num - p_info->cur_column);
                p_info->history[p_info->cur_row][p_info->display_num] = '\0';
                printf("\b%s ", &p_info->history[p_info->cur_row][p_info->cur_column]);
                back_space(p_info->display_num - p_info->cur_column + 1);
            } else {// 光标在字符串尾
                printf("\b \b");
                p_info->history[p_info->cur_row][p_info->cur_column] = '\0';
            }
            break;
        }
        case LEFT_KEY: {
            if ((p_info->display_num == 0) || (p_info->cur_column == 0))
                break;
            printf("\b");
            p_info->cur_column--;
            break;
        }
        case RIGHT_KEY: {
            if (p_info->cur_column > (p_info->display_num - 1))
                break;
            p_info->cur_column++;
            printf("%c", p_info->history[p_info->cur_row][p_info->cur_column - 1]);
            break;
        }
        case UP_KEY: {
            if (p_info->cur_column == 0)
                break;
            printf("\b%c", p_info->history[p_info->cur_row][p_info->cur_column]);
            p_info->cur_column--;
            break;
        }
        case '\t': {
            auto_completion(p_info);
            break;
        }
        case '\n': {
            p_info->fsm = cmd_proc;
            break;
        }
        case '\r': {
            break;
        }
        case C_A: {
            while(p_info->cur_column--) {
                putchar('\b');
            }
            break;
        }
        case C_E: {
            for (; p_info->cur_column < p_info->display_num; p_info->cur_column++)
                putchar( p_info->history[p_info->cur_row][p_info->cur_column]);
            break;
        }
        default: {
            putchar(recv);
            //printf("val:%x\r\n", recv);
            p_info->history[p_info->cur_row][p_info->cur_column]=recv;
            p_info->display_num++;
            p_info->cur_column++;
            break;
        }
    }
    fflush(stdout);
    return;
}

static void fsm_print (info_t *p_info) {
    return;
}

static void get_argv(char *string, int *p_argc, char **argv) {
    uint8_t index = 0;

    argv[0] = strtok(string, " ");
    while (argv[index] && (index < ARGV_MAX)) {
        argv[index] = strtok(NULL, " ");
    }
    return;
}

static void cmd_proc (info_t *p_info) {
    cmd_t *p_cmd;
    char *string = &p_info->history[p_info->cur_row][0];
    int argc;
    char *argv[ARGV_MAX];

    get_argv(string, &argc, &argv[0]);
    for (p_cmd = &cmds[0]; p_cmd < &cmds[sizeof_array(cmds)]; p_cmd++) {
        if (strcmp(string, p_cmd->string) != 0)
            continue;
        p_cmd->proc(argc, argv);
        break;
    }

    if (p_cmd == &cmds[sizeof_array(cmds)])
        printf("cmd not found!");
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

    set_input_mode ();

    while (1) {
        read (STDIN_FILENO, &tmp, 1);

        char_proc(tmp, p_info);
        if (p_info->fsm)
            p_info->fsm(p_info);
    }
    return 0;
}
#endif

