/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "improveTouch_Daemon"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "improveTouch.h"

#define DAEMON_DEBUG false

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

int daemon_loop(improveTouch::TouchServer::Configs config, const char *userConfig) {
    printf("Start touch daemon\n");
    LOG(DEBUG) << "Start touch daemon";

    printf("Debug[%s]:%s\n", DAEMON_DEBUG ? "On" : "Off", "None");
    printf("UserConfig[%s]:%s\n", userConfig ? "On" : "Off", userConfig ?: "None");
    printf("Processor:%s\n", config.processor);

    if (!improveTouch::TouchServer::instantiate(config)) {
        configureRpcThreadpool(1, true /*callerWillJoin*/);
        improveTouch::startTouchCompanionService();
        improveTouch::startGestureManagerService();
        improveTouch::startBlobManagerService();
        LOG(DEBUG) << "start thread pool for hwbinder";
        joinRpcThreadpool();
        return 0;
    } else {
        LOG(ERROR) << "fail to initiate touch server";
        return 1;
    }

    // In normal operation, we don't expect the thread pool to exit
    LOG(DEBUG) << "stop thread pool for hwbinder";
    return 0;
}
