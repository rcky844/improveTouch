/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "improveTouch_Main"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <unistd.h>

#include "improveTouch.h"

int daemon_loop(improveTouch::TouchServer::Configs config, const char *userConfig);

/* Operation modes:
 * - 0: daemon
 * - 1: tool
 */
int mode = 0;

/* Prints program usage to |where|. */
static int usage(int /* argc */, char* argv[]) {
    fprintf(stdout, "Usage: %s [<options>] [<command>] [<args>]\n", argv[0]);
    printf("          -h        : display help message\n");
    printf("          -c fname  : specify config file\n");
    printf("          -a        : use arm touch processor\n");
    printf("          -d        : use arm dsp processor\n");
    printf("  default: <noarg> : touchproc\n");
    return 0;
}

int main(int argc, char* argv[]) {
    int ret = 0;

    struct option options[] = {
        { "help", no_argument, nullptr, 'h' },
        { "config", required_argument, nullptr, 'c' },
        { "arm", no_argument, nullptr, 'a' },
        { "dsp", no_argument, nullptr, 'd' },
        { nullptr, 0, nullptr, 0 },
    };

    const char *userConfig = nullptr;
    improveTouch::TouchServer::Configs config;

    // Set default tocuh processor to dsp
    config.processor = PROCESSOR_DSP;

    // Detect operation mode
    if (!strcmp("hbtp_daemon", *argv)) {
        mode = 0;
    } else if (!strcmp("hbtp_tool", *argv)) {
        mode = 1;
    }

    int rv;
    while ((rv = getopt_long_only(argc, argv, "c:", options, NULL)) != -1) {
        switch (rv) {
            case 'h':
                usage(argc, argv);
                goto exit;

            case 'c':
                userConfig = optarg;
                break;

            case 'a':
                config.processor = PROCESSOR_ARM;
                break;

            case 'd':
                config.processor = PROCESSOR_DSP;
                break;

            default:
                break;
        }
    }

    // Skip any options we consumed with getopt_long_only
    argc -= optind;
    argv += optind;

    // Check for commands after consuming options
    if (argv[0] && !strcmp("tool", argv[0])) {
        mode = 1;
        argc -= 1;
        argv += 1;
    }

    // Start appropriate functions
    switch (mode) {
        case 0:
            ret = daemon_loop(config, userConfig);
            break;

        case 1:
            // TODO
            break;
    }

exit:
    return ret;
}

