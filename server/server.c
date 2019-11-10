#include "common.h"
#include <errno.h>

void res_free(void ** p) { free(p); }
void str_free(char ** p) { res_free(p); }
void msg_free(struct msgbuf ** p) { res_free(p); }
void file_close(int * fd) { close(*fd); }
void proc_file_close(FILE ** fp) { pclose(*fp); }

int main(int argc, char * argv[])
{
    int fd ON_SCOPE_EXIT(file_close) = creat(TOKEN_FILENAME, O_RDWR);
    key_t key = ftok(TOKEN_FILENAME, TOKEN_ID);
    int msgd = msgget(key, IPC_CREAT | IPC_EXCL | 0644);

    char * cmd_out ON_SCOPE_EXIT(str_free) = NULL;
    int out_len = 0;
    FILE * cmdp ON_SCOPE_EXIT(proc_file_close) = popen(
        "ps - eo pid, etimes, pri-- no - headers | awk {'if ($3>30) print $1 "
        " $2'}",
        "r");

    struct msgbuf * msg1 ON_SCOPE_EXIT(msg_free);
    struct msgbuf * msg2 ON_SCOPE_EXIT(msg_free);

    int msg1_len = 0;
    char * msg1_text = NULL;
    int msg2_len = 0;
    char * msg2_text = NULL;

    char * pid;
    int pid_len;
    char * time;
    int time_len;

    while (getline(&cmd_out, &out_len, cmdp) != -1)
    {
        pid = strtok(cmd_out, " ");
        pid_len = strlen(pid);

        msg1_len += pid_len + 1;
        msg1_text = realloc(&msg1_text, msg1_len);
        strcat(msg1_text, pid);
        strcat(msg1_text, "\n");

        msg1 = realloc(&msg1, sizeof(struct msgbuf) + msg1_len);
        strcpy(msg1->mtext, msg1_text);
        msg1->mtype = 1;

        time = strtok(NULL, "\n");
        time_len = strlen(time);

        msg2_len += time_len + 1;
        msg2_text = realloc(&msg2_text, msg2_len);
        strcat(msg2_text, time);
        strcat(msg2_text, "\n");

        msg2 = realloc(&msg2_text, sizeof(struct msgbuf) + msg2_len);
        strcpy(msg2->mtext, msg2_text);
        msg2->mtype = 2;
    }

	msg1->mtext[msg1_len] = '\0';
    msg2->mtext[msg2_len] = '\0';

    msgsnd(msgd, msg1, msg1_len, 0);
    msgsnd(msgd, msg2, msg2_len, 0);

    return 0;
}