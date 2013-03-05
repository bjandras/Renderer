/*
 * This file is part of the Objavi Renderer.
 *
 * Copyright (C) 2013 Borko Jandras <borko.jandras@sourcefabric.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <cstring>
#include <cerrno>
#include <cstdio>


// Process ID of the application process.
pid_t g_app_pid = -1;


void handle_sigalrm(int)
{
    fprintf(stderr, "ERROR: renderer hang-up detected\n");
    ::kill(g_app_pid, SIGKILL);
}


int run_shell(pid_t child_pid)
{
    g_app_pid = child_pid;

    sigset(SIGALRM, handle_sigalrm);
    alarm(60);

    int status = 0;
    pid_t wait_pid;

    do
    {
        wait_pid = ::waitpid(child_pid, &status, 0);
    }
    while (wait_pid < 0 && errno == EINTR);

    if (wait_pid >= 0)
    {
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }

        return -1;
    }
    else
    {
        fprintf(stderr, "waitpid() error: %s\n", strerror(errno));

        return -1;
    }
}


int run_app(int argc, char ** argv)
{
    Objavi::Application app(argc, argv);

    return app.exec();
}


int main(int argc, char ** argv)
{
    bool gui_mode = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-gui") == 0)
        {
            gui_mode = true;
            break;
        }
    }

    if (gui_mode == true)
    {
        return run_app(argc, argv);
    }
    else
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            return run_app(argc, argv);
        }
        else if (pid > 0)
        {
            return run_shell(pid);
        }
        else
        {
            fprintf(stderr, "fork() error: %s\n", strerror(errno));
            return -1;
        }
    }
}
