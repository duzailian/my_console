static void head_up_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_down_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_left_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_right_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_entry_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_bs_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_head_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_tail_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_tab_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void head_normal_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

/*当前光标在行尾，且收到上方向键时的动作*/
static void tail_up_key_proc(uint8_t recv, info_t *p_info) {
    size_t len;
    
    if (p_info->cur_row == 0)
        return;
    p_info->cur_row--;
    put_backspace(p_info->display_num, false);
    len = printf("%s", &p_info->history[p_info->cur_row][0]);
    if (p_info->display_num > len)// 原字符长度大于新字符长度
        put_space(p_info->display_num - len);
    p_info->display_num = len;
    p_info->cur_column = p_info->display_num;
    return;
}

static void tail_down_key_proc(uint8_t recv, info_t *p_info) {
    if ((p_info->cur_row + 1) >= sizeof_array(p_info->history))
        return;
    if (p_info->history[0][0] == '\0')
        return;
    p_info->cur_row++;
    put_backspace(p_info->display_num, false);
    p_info->display_num = printf("%s", &p_info->history[p_info->cur_row][0]);
    p_info->cur_column = p_info->display_num;
    return;
}

static void tail_left_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0) 
        return;
    p_info->cur_column--;
    put_backspace(1, false);
    return;
}

static void tail_right_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column >= p_info->display_num) 
        return;
    p_info->cur_column++;
    putchar(p_info->history[p_info->cur_row][p_info->cur_column]);
    return;
}

static void tail_entry_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0) {
        printf("\n#");
        return;
    } 
    printf("\n#");
    cmd_proc(p_info);
    printf("\n#");
    return;
}

static void tail_bs_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->cur_column == 0) 
        return;
    p_info->cur_column--;
    put_backspace(1, true);
    putchar('\b');
    return;
}

static void tail_head_key_proc(uint8_t recv, info_t *p_info) {
    put_backspace(p_info->cur_column, false);
    p_info->cur_column = 0;
    p_info->cur_status = status_head;// 将状态切换为光标在行首状态
    return;
}

static void tail_tail_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void tail_tab_key_proc(uint8_t recv, info_t *p_info) {
    return;
}

static void tail_normal_key_proc(uint8_t recv, info_t *p_info) {
    if (p_info->display_num >= STRING_MAX) 
        return;
    putchar(recv);
    p_info->history[p_info->cur_row][p_info->cur_column++] = recv;
    return;
}

