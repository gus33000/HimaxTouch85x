// Copyright (c) Microsoft Corporation. All Rights Reserved. 
// Copyright (c) Bingxing Wang. All Rights Reserved. 

#pragma once

#define PTP_MAX_CONTACT_POINTS 10
#define PTP_BUTTON_TYPE_CLICK_PAD 0
#define PTP_BUTTON_TYPE_PRESSURE_PAD 1

#define PTP_COLLECTION_MOUSE 0
#define PTP_COLLECTION_WINDOWS 3

#define PTP_CONTACT_CONFIDENCE_BIT   1
#define PTP_CONTACT_TIPSWITCH_BIT    2

#define REPORTID_DIAGNOSTIC_1 0xF3
#define REPORTID_DIAGNOSTIC_2 0xF2
#define REPORTID_DIAGNOSTIC_FEATURE_1 0xF1
#define REPORTID_DIAGNOSTIC_3 0xF4
#define REPORTID_DIAGNOSTIC_4 0xF5
#define REPORTID_DIAGNOSTIC_FEATURE_4 0xF6

#define REPORTID_FINGER 0x01
#define REPORTID_REPORTMODE 0x07
#define REPORTID_DEVICE_CAPS 0x08
#define REPORTID_KEYPAD 0x09
#define REPORTID_STYLUS 0x0B

#define REPORTID_PTPHQA 0x0E
#define REPORTID_PENHQA 0x0F

#define BUTTON_SWITCH 0x57
#define SURFACE_SWITCH 0x58

#define USAGE_PAGE         0x05
#define USAGE_PAGE_1       0x06
#define USAGE              0x09
#define USAGE_2            0x0A
#define USAGE_MINIMUM      0x19
#define USAGE_MAXIMUM      0x29
#define LOGICAL_MINIMUM    0x15
#define LOGICAL_MINIMUM_2  0x16
#define LOGICAL_MAXIMUM    0x25
#define LOGICAL_MAXIMUM_2  0x26
#define LOGICAL_MAXIMUM_3  0x27
#define PHYSICAL_MINIMUM   0x35
#define PHYSICAL_MINIMUM_2 0x36
#define PHYSICAL_MAXIMUM   0x45
#define PHYSICAL_MAXIMUM_2 0x46
#define PHYSICAL_MAXIMUM_3 0x47
#define UNIT_EXPONENT      0x55
#define UNIT               0x65
#define UNIT_2             0x66

#define REPORT_ID       0x85
#define REPORT_COUNT    0x95
#define REPORT_COUNT_2	0x96
#define REPORT_SIZE     0x75
#define INPUT           0x81
#define FEATURE         0xb1

#define BEGIN_COLLECTION 0xa1
#define END_COLLECTION   0xc0

#define REPORT_BUFFER_SIZE   1024
#define DEVICE_VERSION 0x01
#define MAX_FINGERS	16