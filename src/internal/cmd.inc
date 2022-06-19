#pragma once

typedef void (*cmd_proc_t)(int argc, char **argv);

typedef  const struct {
    char *string;
    cmd_proc_t proc;
    char *help_str;
}cmd_t;

#define help_hstring NULL
#define test_hstring "test help"

#define CMD(__cmd) {\
                .string = #__cmd,\
                .proc = __cmd##_proc,\
                .help_str = __cmd##_hstring,\
                }

static void help_proc(int argc, char **argv);
static void test_proc(int argc, char **argv);

static cmd_t cmds[] = {
    CMD(help),
    CMD(test),
};


static void help_proc(int argc, char **argv) {
    cmd_t *p_cmd = &cmds[0];

    printf("help:\n");
    for (int i = 0; i < sizeof_array(cmds); i++, p_cmd++) {
        if (p_cmd->help_str == NULL)
            continue;
        printf("%s", p_cmd->help_str);
    }
    (void) argc;
    (void) argv;
    return;
}


static void test_proc(int argc, char **argv) {
    printf("just for test\n");
    return;
}
