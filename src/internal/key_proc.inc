#pragma once

#define head_up_key_proc        up_key_proc
#define head_down_key_proc      down_key_proc
#define head_left_key_proc      NULL
#define head_right_key_proc     right_key_proc
#define head_entry_key_proc     entry_key_proc
#define head_bs_key_proc        bs_key_proc
#define head_head_key_proc      NULL
#define head_tail_key_proc      tail_key_proc
#define head_tab_key_proc       NULL
#define head_normal_key_proc    normal_key_proc


#define tail_up_key_proc        up_key_proc
#define tail_down_key_proc      down_key_proc
#define tail_left_key_proc      left_key_proc
#define tail_right_key_proc     NULL
#define tail_entry_key_proc     entry_key_proc
#define tail_head_key_proc      head_key_proc
#define tail_tail_key_proc      NULL
#define tail_tab_key_proc       NULL


#define middle_up_key_proc      up_key_proc
#define middle_down_key_proc    down_key_proc
#define middle_left_key_proc    left_key_proc
#define middle_right_key_proc   right_key_proc
#define middle_entry_key_proc   entry_key_proc
#define middle_bs_key_proc      bs_key_proc
#define middle_head_key_proc    head_key_proc
#define middle_tail_key_proc    tail_key_proc
#define middle_tab_key_proc     tab_key_proc
#define middle_normal_key_proc  normal_key_proc

/*通用处理函数声明*/
static void up_key_proc(uint8_t recv, info_t *p_info);
static void down_key_proc(uint8_t recv, info_t *p_info);
static void left_key_proc(uint8_t recv, info_t *p_info);
static void right_key_proc(uint8_t recv, info_t *p_info);
static void entry_key_proc(uint8_t recv, info_t *p_info);
static void bs_key_proc(uint8_t recv, info_t *p_info);
static void head_key_proc(uint8_t recv, info_t *p_info);
static void tail_key_proc(uint8_t recv, info_t *p_info);
static void tab_key_proc(uint8_t recv, info_t *p_info);
static void normal_key_proc(uint8_t recv, info_t *p_info);

/*光标在行尾状态下的专用处理函数声明*/
static void tail_bs_key_proc(uint8_t recv, info_t *p_info);
static void tail_normal_key_proc(uint8_t recv, info_t *p_info);

/*光标在行首时的动作定义*/
#define DEFINE(key)    .proc[key##_key] = head_##key##_key_proc,
static status_proc_t *status_head = &(status_proc_t) {
    #include "internal/key_type.h"
};
#undef DEFINE

/*光标在行尾时的动作定义*/
#define DEFINE(key)    .proc[key##_key] = tail_##key##_key_proc,
static status_proc_t *status_tail = &(status_proc_t) {
    #include "internal/key_type.h"
};
#undef DEFINE


/*光标在行中时的动作定义*/
#define DEFINE(key)    .proc[key##_key] = middle_##key##_key_proc,
static status_proc_t *status_middle = &(status_proc_t) {
    #include "internal/key_type.h"
};
#undef DEFINE


/*****************************************通用处理函数定义*****************************************/
/*收到上方向键时的动作*/
static void up_key_proc(uint8_t recv, info_t *p_info) {
    size_t len;

    if (p_info->cur_row == 0)
        return;
    p_info->cur_row--;
    put_backspace(p_info->display_num, false);
    len = printf("%s", &p_info->history[p_info->cur_row][0]);
    if (p_info->display_num > len) {// 原字符长度大于新字符长度
        put_space(p_info->display_num - len);
        put_backspace(p_info->display_num - len, false);
    }
    p_info->display_num = len;
    p_info->cur_column = p_info->display_num;
    p_info->cur_status = status_tail;// 将状态切换为光标在行尾状态
    return;
}

static void down_key_proc(uint8_t recv, info_t *p_info) {
    if ((p_info->cur_row + 1) >= sizeof_array(p_info->history))
        return;
    if (p_info->history[p_info->cur_row + 1][0] == '\0')
        return;
    p_info->cur_row++;
    put_backspace(p_info->display_num, false);
    p_info->display_num = printf("%s", &p_info->history[p_info->cur_row][0]);
    p_info->cur_column = p_info->display_num;
    p_info->cur_status = status_tail;// 将状态切换为光标在行尾状态
    return;
}

static void left_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0)
        return;
    p_info->cur_column--;
    put_backspace(1, false);
    if (p_info->cur_column == 0)
        p_info->cur_status = status_head;// 将状态切换为光标在行首状态
    else
        p_info->cur_status = status_middle;// 将状态切换为光标在行中状态

    return;
}

static void right_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column >= p_info->display_num)
        return;
    putchar(p_info->history[p_info->cur_row][p_info->cur_column]);
    p_info->cur_column++;

    if (p_info->cur_column == p_info->display_num)
        p_info->cur_status = status_tail;// 将状态切换为光标在行尾状态
    else
        p_info->cur_status = status_middle;// 将状态切换为光标在行中状态

    return;
}

static void entry_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0) {
        printf("\n#");
        return;
    }
    printf("\n");
    cmd_proc(p_info);
    printf("\n#");
    p_info->cur_row++;
    p_info->display_num = 0;
    p_info->cur_column = 0;

    if (p_info->cur_row == sizeof_array(p_info->history))
        p_info->cur_row = 0;
    p_info->cur_status = status_tail;// 将状态切换为光标在行尾状态
    return;
}

static void bs_key_proc(uint8_t recv, info_t *p_info) {
    size_t num;

    if (p_info->cur_column == 0)
        return;

    put_backspace(1, false);
    num = printf("%s", &p_info->history[p_info->cur_row][p_info->cur_column]);
    putchar(' ');
    putnchar('\b', num + 1);

    memmove(&p_info->history[p_info->cur_row][p_info->cur_column - 1],
            &p_info->history[p_info->cur_row][p_info->cur_column],
            p_info->display_num - p_info->cur_column);
    p_info->cur_column--;
    p_info->display_num--;
    p_info->history[p_info->cur_row][p_info->display_num] = '\0';
    if (p_info->cur_column == 0)
        p_info->cur_status = status_head;// 将状态切换为光标在行首状态
    return;
}

static void head_key_proc(uint8_t recv, info_t *p_info) {
    put_backspace(p_info->cur_column, false);
    p_info->cur_column = 0;
    p_info->cur_status = status_head;// 将状态切换为光标在行首状态
    return;
}

static void tail_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == p_info->display_num)
        return;
    printf("%s", &p_info->history[p_info->cur_column][0]);
    p_info->cur_column = p_info->display_num;
    p_info->cur_status = status_tail;// 将状态切换为光标在行尾状态
    return;
}

static void tab_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void normal_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->display_num >= STRING_MAX)
        return;
    putchar(recv);
    p_info->history[p_info->cur_row][p_info->cur_column++] = recv;
    p_info->history[p_info->cur_row][p_info->cur_column] = '\0';
    p_info->display_num++;
    return;
}

/*****************************************光标在行尾状态下的专用处理函数声明*****************************************/
static void tail_bs_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0)
        return;
    p_info->cur_column--;
    p_info->display_num--;
    put_backspace(1, true);
    putchar('\b');
    p_info->history[p_info->cur_row][p_info->display_num] = '\0';
    return;
}

static void tail_normal_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->display_num >= STRING_MAX)
        return;
    putchar(recv);
    p_info->history[p_info->cur_row][p_info->cur_column++] = recv;
    p_info->history[p_info->cur_row][p_info->cur_column] = '\0';
    p_info->display_num++;
    return;
}