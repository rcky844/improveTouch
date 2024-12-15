/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "improveTouch"

namespace improveTouch {

const char *errStr(int errorCode) {
    switch (errorCode) {
        case 0x40000000:
            return "An unknown error has occurred";
        case 0x40000001:
            return "Error opening file";
        case 0x40000002:
            return "Error parsing XML";
        case 0x40000003:
            return "Error parsing configuration setting";
        case 0x40000004:
            return "Error writing to file";
        case 0x40000005:
            return "Invalid command";
        case 0x40000006:
            return "Error allocating memory";
        case 0x40000007:
            return "Invalid packet size";
        case 0x40000008:
            return "Error involving Android Memory Dealer";
        case 0x40000009:
            return "Broken pipe";
        case 0x4000000a:
            return "Error attempting to acquire resource";
        case 0x4000000b:
            return "Invalid state";
        case 0x4000000c:
            return "Invalid format";
        case 0x4000000d:
            return "Bad parameter";
        case 0x4000000e:
            return "Action/command is unsupported";
        case 0x4000000f:
            return "Error reading file";
        case 0x40000010:
            return "Error involving AFE HAL";
        case 0x40000011:
            return "Error involving Environment Governor";
        case 0x40000012:
            return "Error initializing touch processor";
        case 0x40000013:
            return "Invalid file path";
        case 0x40000014:
            return "Error parsing parameter";
        case 0x40000015:
            return "Timeout has occurred";
        case 0x40000016:
            return "Binder Died";
        case 0x40000017:
            return "Not enough args";
        case 0x40000018:
            return "Invalid Arguments";
        case 0x40000019:
            return "Not Found";
        case 0x4000001a:
            return "Data size exceeds size of buffer";
        case 0x4000001b:
            return "NULL pointer";
        case 0x4000001c:
            return "Index out of bounds";
        case 0x4000001d:
            return "Corrupt data";
    }

    if (errorCode != 0) {
        return "Unsupported error code";
    }
    return "No error"; // Error code = 0
}

}

