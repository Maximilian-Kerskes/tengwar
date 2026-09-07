#pragma once

#include "tengwar/parser.h"

int builtin_cd(const Command *command);
int builtin_exit(const Command *command);
int builtin_export(const Command *command);
int builtin_unset(const Command *command);
int builtin_pwd(const Command *command);
int builtin_echo(const Command *command);
int builtin_true(const Command *command);
int builtin_false(const Command *command);
