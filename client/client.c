#include "common.h"

void res_free(void ** p) { free(p); }
void str_free(char ** p) { res_free(p); }
void msg_free(struct msgbuf ** p) { res_free(p); }

int main(int argc, char * argv[])
{
    while (access(TOKEN_FILENAME, F_OK) == -1)
	{
        sleep(1);
	}

	key_t key = ftok(TOKEN_FILENAME, TOKEN_ID);

	int msgd = msgget(key, 0644);

	struct msgbuf * msg1 ON_SCOPE_EXIT(msg_free) =
            calloc(sizeof(struct msgbuf) + MSGMAX, 1);
    struct msgbuf * msg2 ON_SCOPE_EXIT(msg_free) =
            calloc(sizeof(struct msgbuf) + MSGMAX, 1);

	msgrcv(msgd, msg1, MSGMAX, 1, 0);
    msgrcv(msgd, msg2, MSGMAX, 2, 0);

    char * pid = calloc(strlen(msg1->mtext), 1);
    char * time = calloc(strlen(msg2->mtext), 1);
    int pid_len;
    int time_len;
    int proc_count = 0;
    int max_lifetime = 0;
    int max_pid = 0;

	strcpy(pid, msg1->mtext);
    strcpy(time, msg2->mtext);

	pid = strtok(pid, '\n');
    time = strtok(time, '\n');

	do
    {
		pid_len = strlen(pid);
		time_len = strlen(time);

		++proc_count;

		if (stoi(time) > max_lifetime)
		{
			max_lifetime = stoi(time);
            max_pid = stoi(pid);
		}
                    
    } while ((pid = strtok(pid + pid_len + 1, '\n')) != NULL &&
             (time = strtok(time + time_len + 1, '\n')) != NULL);

    return 0;
}