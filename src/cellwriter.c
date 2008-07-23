/******************************************************************************\
 CellWriter (GPL) - Copyright (C) 2008 - Michael Levin

 CellWriter is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 CellWriter is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 CellWriter. If not, see <http://www.gnu.org/licenses/>.
\******************************************************************************/

#include "c_shared.h"
#include "windows/w_shared.h"
#include <signal.h>

/* Command line arguments */
static GOptionEntry command_line_opts[] = {
        { "log-level", 0, 0, G_OPTION_ARG_INT, &c_log_level,
          "Log threshold (0=silent, 3=debug)", "2" },
        { "log-file", 0, 0, G_OPTION_ARG_STRING, &c_log_filename,
          "Log file to use instead of stdout", PACKAGE ".log" },
        { "mem-check", 0, 0, G_OPTION_ARG_INT, &c_mem_check,
          "Enable to debug memory allocations", "0" },

        /* Sentinel */
        { NULL, 0, 0, 0, NULL, NULL, NULL }
};

/******************************************************************************\
 Cleanup before exit.
\******************************************************************************/
static void cleanup(void)
{
        static int ran_once;

        /* Disable the log event handler */
        c_log_mode = C_LM_CLEANUP;

        /* It is possible that this function will get called multiple times
           for certain kinds of exits, do not clean-up twice! */
        if (ran_once) {
                C_warning("Cleanup already called");
                return;
        }
        ran_once = TRUE;

        C_status("Cleaning up");
        C_cleanup_log();
        C_check_leaks();
        C_debug("Done");
}

/******************************************************************************\
 Called by Unix when we've caught a nasty signal and need to die gracefully.
\******************************************************************************/
static void catch_sigterm(int sig)
{
        g_warning("Caught signal %d, cleaning up", sig);
        cleanup();
        exit(1);
}

/******************************************************************************\
 Setup POSIX signal catchers. Try to save and exit cleanly -- gdb is not
 affected by any of these signals being caught.
\******************************************************************************/
static void hook_signals(void)
{
        sigset_t sigset;
        int *ps, catch_signals[] = {SIGSEGV, SIGHUP, SIGINT,
                                    SIGTERM, SIGQUIT, SIGALRM, -1};

        sigemptyset(&sigset);
        ps = catch_signals;
        while (*ps != -1) {
                signal(*ps, catch_sigterm);
                sigaddset(&sigset, *(ps++));
        }
        if (sigprocmask(SIG_UNBLOCK, &sigset, NULL) == -1)
                C_error("Failed to set signal blocking mask");
}

/******************************************************************************\
 Initialize GTK/GNOME.
\******************************************************************************/
static int init_gtk_gnome(int argc, char *argv[])
{
        GError *error;

        /* Initialize GTK+ */
        error = NULL;
        if (!gtk_init_with_args(&argc, &argv,
                                "grid-entry handwriting input panel",
                                command_line_opts, NULL, &error))
                return FALSE;

        C_init_log();

#ifdef HAVE_GNOME
        /* Initialize GNOME for the Help button */
        gnome_program_init(PACKAGE, VERSION, LIBGNOME_MODULE,
                           argc, argv, NULL);
#endif

        return TRUE;
}

/******************************************************************************\
 Program entry point.
\******************************************************************************/
int main(int argc, char *argv[])
{
        /* Ensure that if there is a crash, data is saved properly */
        hook_signals();
        atexit(cleanup);

        /* Initialize */
        if (!init_gtk_gnome(argc, argv))
                return 1;
        C_status("Initializing " PACKAGE_STRING);
        W_init_defaults();
        I_init_defaults();

        /* TODO: Load profile data here */

        W_init_main();

        /* Startup notification is sent when the first window shows but if
           the window was closed during last start, it won't show at all so
           we need to do this manually */
        gdk_notify_startup_complete();

        /* Run the interface */
        gtk_main();
        cleanup();

        return 0;
}

