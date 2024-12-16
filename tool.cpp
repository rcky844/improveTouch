/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "improveTouch_Tool"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

#include <map>
#include <string>

#include <sys/stat.h>
#include <android-base/logging.h>

#include "linenoise/linenoise.h"
#include "improveTouch.h"

using std::map;
using std::string;

improveTouch::TouchCompanionClient *touchCompanionClient;
bool dataStorageAvailable = true;
bool loggingAvailable = true;

// Defines
#define HBTP_DATA_PATH "/data/vendor/hbtp"
#define HBTP_SESSIONID_PATH "/data/vendor/hbtp/sessionid"

// Command functions
void cmd_pingService(char * /* args */) {
    if (touchCompanionClient->pingService()) {
        puts("service is alive!");
    } else {
        puts("service is dead!");
    }
}

void cmd_startRecord(char * /* args */) {
    printf("start recording with 0x%x filter\n", RECORD_FILTER);
    touchCompanionClient->setRecordFilter(RECORD_FILTER);
    touchCompanionClient->startRecord();
}

void cmd_stopRecord(char * /* args */) {
    printf("stop recording\n");
    touchCompanionClient->stopRecord();
}

void cmd_startRecordRolling(char * /* args */) {
    // TODO
}

void cmd_stopRecordRolling(char * /* args */) {
    // TODO
}

void cmd_setRecordFilter(char *args) {
    if (!args) {
        printf("Not Enough Arguments\n Format:\n setRecordFilter mask(hex)32bit\n");
        return;
    }

    // TODO
}

void cmd_sendCommand(char *args) {
    int ret;

    if (!args) {
        puts("Invalid command");
        return;
    }
    printf("Command you type = %s\n", args);

    char *result;
    ret = touchCompanionClient->sendCommand(args, &result);
    if (ret == 0) {
        if (result) printf("Result: %s\n", result);
        puts("Success");
    } else {
        puts("Failed to get result");
    }
}

void cmd_usb(char *args) {
    improveTouch::eventParam eventParam;
    if (!strcmp(args, "d")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_D;
    } else if (!strcmp(args, "c")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_C;
    } else {
        puts("Invalid parameter");
        return;
    }


    touchCompanionClient->sendEvent(
        improveTouch::eventType::EVENT_TYPE_USB, eventParam);
}

void cmd_hdmi(char *args) {
    improveTouch::eventParam eventParam;
    if (!strcmp(args, "d")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_D;
    } else if (!strcmp(args, "c")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_C;
    } else {
        puts("Invalid parameter");
        return;
    }

    touchCompanionClient->sendEvent(
        improveTouch::eventType::EVENT_TYPE_HDMI, eventParam);
}

void cmd_lcd(char *args) {
    improveTouch::eventParam eventParam;
    if (!strcmp(args, "on")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_ON;
    } else if (!strcmp(args, "off")) {
        eventParam = improveTouch::eventParam::EVENT_PARAM_OFF;
    } else {
        puts("Invalid parameter");
        return;
    }

    touchCompanionClient->sendEvent(
        improveTouch::eventType::EVENT_TYPE_LCD, eventParam);
}

void cmd_help(char * /* args */) {
    printf("Usage:\n"
           "startRecord: start data recording\n"
           "stopRecord: stop data recording\n"
           "startRecordRolling: start data recording with rolling log.\n"
           "\tRecording will be stopped automatically after 1 hour.\n"
           "stopRecordRolling: stop data recording started with startRecordRolling.\n"
           "setRecordFilter <filter>: specify log filter for data recording\n"
           "sendCommand <command> [arguments]: send comamnd to touch companion\n"
           "lcd <on|off>: focibly set LCD on (only for touch, not actual LCD)\n"
           "help: print usage message\n"
           "exit: exit tool\n"
    );
}

void cmd_exit(char * /* args */) {
    exit(0);
}

// Main loop
std::map<const char *, void (*)(char *)> commandMap = {
    { "pingService", &cmd_pingService },
    { "startRecord", &cmd_startRecord },
    { "stopRecord", &cmd_stopRecord },
    { "startRecordRolling", &cmd_startRecordRolling },
    { "stopRecordRolling", &cmd_stopRecordRolling },
    { "setRecordFilter", &cmd_setRecordFilter },
    { "sendCommand", &cmd_sendCommand },
    { "usb", &cmd_usb },
    { "hdmi", &cmd_hdmi },
    { "lcd", &cmd_lcd },
    { "help", &cmd_help },
    { "exit", &cmd_exit },
};

void handle_cmd(char *line) {
    char *cmd = strtok(line, " ");
    bool handled = false;

    map<const char *, void (*)(char *)>::iterator it;
    for (it = commandMap.begin(); it != commandMap.end(); ++it) {
        if (!strcmp(it->first, cmd)) {
            handled = true;
            LOG(DEBUG) << "Handling command: " << cmd;

            cmd = strtok(NULL, "");
            if (cmd)
                it->second(cmd);
            else
                it->second(0);
            break;
        }
    }

    if (!handled) {
        puts("Invalid command");
    }
}

void tool_cli_init() {
    struct stat buf;

    // Check for data storage
    if (!stat(HBTP_DATA_PATH, &buf) && (buf.st_mode & 0xF000) == 0x4000) {
        LOG(DEBUG) << "Using data path: " << HBTP_DATA_PATH;
    } else {
        dataStorageAvailable = false;
        LOG(WARNING) << "Data path " << HBTP_DATA_PATH << "not found, disabling data storage";
    }

    // FIXME: Mad hax
    char *token = (char *) malloc(15);
    strcpy(token + 1, "token=0");
    if ((long) token & 1) token = 0;

    // Test for logging feature
    if (dataStorageAvailable && !touchCompanionClient->setAuth(token)) {
        int ret = touchCompanionClient->setRecordOutput(0);
        if (ret) {
            printf("Error message: %s\n", improveTouch::errStr(ret));
            puts("Failed to prepare logging feature");
            loggingAvailable = false;
        } else {
            // Check if there is a logging session present
            FILE *logCheck = fopen(HBTP_SESSIONID_PATH, "r");
            if (logCheck) {
                fclose(logCheck);
                puts("You have a recording session which was started with startRecordRolling");
                puts("but, was not stopped yet");
                puts("You can stop it with stopRecordRolling");
            }
        }
    }
    free(token);
}

int tool_loop(int argc, char* argv[]) {
    int ret = 0;

    // Get a handle to TouchCompanionClient
    touchCompanionClient = new improveTouch::TouchCompanionClient();
    ret = touchCompanionClient->init();
    if (ret) {
        printf("Error message: %s\n", improveTouch::errStr(ret));
        puts("Failed to connect to service... exiting");
        goto exit;
    }

    // Process commands when CLI is not requested
    if (argc > 0) {
        argv += 1;
        handle_cmd(*argv);
        puts(*argv);
        return 0;
    }

    // Perform additional checks
    tool_cli_init();

    puts("improveTouch CLI");
    while (true) {
        char *line = linenoise("> ");

        // Process lines
        if (*line) {
            linenoiseHistoryAdd(line);
            handle_cmd(line);
        }
        free(line);

        // Check service
        if (!touchCompanionClient->pingService()) {
            puts("lost connection with service... exiting");
            break;
        }
    }

exit:
    return 0;
}
