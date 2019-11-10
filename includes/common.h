#pragma once

#define ON_SCOPE_EXIT(destructor) __attribute__((__cleanup__(destructor)))

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/msg.h>
#include <sys/msg.h>

#define TOKEN_FILENAME "/tmp/tok_file"
#define TOKEN_ID 42