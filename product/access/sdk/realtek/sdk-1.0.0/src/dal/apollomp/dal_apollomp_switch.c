/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 40104 $
 * $Date: 2013-06-08 18:48:58 +0800 (Sat, 08 Jun 2013) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <rtk/switch.h>

#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/raw/apollomp_raw_switch.h>
#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#include <hal/common/miim.h>
#include <osal/time.h>
#include <hal/mac/mac_probe.h>
/*
 * Symbol Definition
 */
#define DAL_APOLLOMP_BROADCAST_PHYID  (0x0010)


typedef struct dal_apollomp_phy_data_s
{
    uint16  phy;
    uint16  page;
    uint16  reg;
    uint16  data;
} dal_apollomp_phy_data_t;


/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;

static uint32  chipId;
static uint32  chipRev;
static uint32  chipSubtype;

#if 1

static dal_apollomp_phy_data_t  phyPatchArray[] = {
    {0x0010,0xa43 ,27, 0x80c1},
    {0x0010,0xa43 ,28, 0xfe00}
};
#else
static dal_apollomp_phy_data_t  phyPatchArray[] = {
    {0x0010,0xa43 ,27, 0x8146},
    {0x0010,0xa43 ,28, 0x7101},
    {0x0010,0xa43 ,27, 0xb82e},
    {0x0010,0xa43 ,28, 0x0001},
    {0x0010,0xa43 ,27, 0xB820},
    {0x0010,0xa43 ,28, 0x0090},
    {0x0010,0xa43 ,27, 0xA012},
    {0x0010,0xa43 ,28, 0x0000},
    {0x0010,0xa43 ,27, 0xA014},
    {0x0010,0xa43 ,28, 0x2c04},
    {0x0010,0xa43 ,28, 0x2c2a},
    {0x0010,0xa43 ,28, 0x2c36},
    {0x0010,0xa43 ,28, 0x2c3d},
    {0x0010,0xa43 ,28, 0xc2f0},
    {0x0010,0xa43 ,28, 0x312a},
    {0x0010,0xa43 ,28, 0x5c0a},
    {0x0010,0xa43 ,28, 0xc2e0},
    {0x0010,0xa43 ,28, 0x404a},
    {0x0010,0xa43 ,28, 0xc2c0},
    {0x0010,0xa43 ,28, 0x406e},
    {0x0010,0xa43 ,28, 0x8510},
    {0x0010,0xa43 ,28, 0x2c0e},
    {0x0010,0xa43 ,28, 0xa510},
    {0x0010,0xa43 ,28, 0xd500},
    {0x0010,0xa43 ,28, 0xc450},
    {0x0010,0xa43 ,28, 0xc100},
    {0x0010,0xa43 ,28, 0xc840},
    {0x0010,0xa43 ,28, 0xc000},
    {0x0010,0xa43 ,28, 0xa37a},
    {0x0010,0xa43 ,28, 0xd027},
    {0x0010,0xa43 ,28, 0xd1a3},
    {0x0010,0xa43 ,28, 0x401a},
    {0x0010,0xa43 ,28, 0x8342},
    {0x0010,0xa43 ,28, 0x1479},
    {0x0010,0xa43 ,28, 0xd700},
    {0x0010,0xa43 ,28, 0x324b},
    {0x0010,0xa43 ,28, 0x1c28},
    {0x0010,0xa43 ,28, 0xd71e},
    {0x0010,0xa43 ,28, 0x6169},
    {0x0010,0xa43 ,28, 0xd704},
    {0x0010,0xa43 ,28, 0x3149},
    {0x0010,0xa43 ,28, 0x5c23},
    {0x0010,0xa43 ,28, 0x300f},
    {0x0010,0xa43 ,28, 0x1c29},
    {0x0010,0xa43 ,28, 0x316e},
    {0x0010,0xa43 ,28, 0x1c16},
    {0x0010,0xa43 ,28, 0x606d},
    {0x0010,0xa43 ,28, 0x3288},
    {0x0010,0xa43 ,28, 0x3c16},
    {0x0010,0xa43 ,28, 0x20ba},
    {0x0010,0xa43 ,28, 0x20f3},
    {0x0010,0xa43 ,28, 0x9404},
    {0x0010,0xa43 ,28, 0xd705},
    {0x0010,0xa43 ,28, 0x6132},
    {0x0010,0xa43 ,28, 0xd71e},
    {0x0010,0xa43 ,28, 0x60c9},
    {0x0010,0xa43 ,28, 0xd704},
    {0x0010,0xa43 ,28, 0x6093},
    {0x0010,0xa43 ,28, 0x606d},
    {0x0010,0xa43 ,28, 0x3288},
    {0x0010,0xa43 ,28, 0x3c35},
    {0x0010,0xa43 ,28, 0x20e8},
    {0x0010,0xa43 ,28, 0x207a},
    {0x0010,0xa43 ,28, 0xd704},
    {0x0010,0xa43 ,28, 0x31ce},
    {0x0010,0xa43 ,28, 0x0c3b},
    {0x0010,0xa43 ,28, 0x6065},
    {0x0010,0xa43 ,28, 0x247d},
    {0x0010,0xa43 ,28, 0x2517},
    {0x0010,0xa43 ,28, 0x2482},
    {0x0010,0xa43 ,28, 0x2c3d},
    {0x0010,0xa43 ,27, 0xA01A},
    {0x0010,0xa43 ,28, 0x0000},
    {0x0010,0xa43 ,27, 0xA006},
    {0x0010,0xa43 ,28, 0x0fff},
    {0x0010,0xa43 ,27, 0xA004},
    {0x0010,0xa43 ,28, 0x0479},
    {0x0010,0xa43 ,27, 0xA002},
    {0x0010,0xa43 ,28, 0x00df},
    {0x0010,0xa43 ,27, 0xA000},
    {0x0010,0xa43 ,28, 0x7096},
    {0x0010,0xa43 ,27, 0xB820},
    {0x0010,0xa43 ,28, 0x0010},
    {0x0010,0xa43 ,27, 0x8368},
    {0x0010,0xa43 ,28, 0xaf83},
    {0x0010,0xa43 ,28, 0x74af},
    {0x0010,0xa43 ,28, 0x838c},
    {0x0010,0xa43 ,28, 0xaf83},
    {0x0010,0xa43 ,28, 0x9baf},
    {0x0010,0xa43 ,28, 0x83a4},
    {0x0010,0xa43 ,28, 0x0719},
    {0x0010,0xa43 ,28, 0xd907},
    {0x0010,0xa43 ,28, 0xbf3a},
    {0x0010,0xa43 ,28, 0x2f02},
    {0x0010,0xa43 ,28, 0x373c},
    {0x0010,0xa43 ,28, 0x0719},
    {0x0010,0xa43 ,28, 0xd907},
    {0x0010,0xa43 ,28, 0xbf3a},
    {0x0010,0xa43 ,28, 0x2c02},
    {0x0010,0xa43 ,28, 0x373c},
    {0x0010,0xa43 ,28, 0x07af},
    {0x0010,0xa43 ,28, 0x0e1a},
    {0x0010,0xa43 ,28, 0xad31},
    {0x0010,0xa43 ,28, 0x0302},
    {0x0010,0xa43 ,28, 0x83f7},
    {0x0010,0xa43 ,28, 0xad32},
    {0x0010,0xa43 ,28, 0x0302},
    {0x0010,0xa43 ,28, 0x84d1},
    {0x0010,0xa43 ,28, 0xaf19},
    {0x0010,0xa43 ,28, 0x5202},
    {0x0010,0xa43 ,28, 0x847e},
    {0x0010,0xa43 ,28, 0x0284},
    {0x0010,0xa43 ,28, 0xb7af},
    {0x0010,0xa43 ,28, 0x0354},
    {0x0010,0xa43 ,28, 0xe6ff},
    {0x0010,0xa43 ,28, 0xf701},
    {0x0010,0xa43 ,28, 0x03af},
    {0x0010,0xa43 ,28, 0x01e0},
    {0x0010,0xa43 ,28, 0xee80},
    {0x0010,0xa43 ,28, 0x127f},
    {0x0010,0xa43 ,28, 0x0285},
    {0x0010,0xa43 ,28, 0x9dee},
    {0x0010,0xa43 ,28, 0x8073},
    {0x0010,0xa43 ,28, 0x14ee},
    {0x0010,0xa43 ,28, 0x807e},
    {0x0010,0xa43 ,28, 0xffee},
    {0x0010,0xa43 ,28, 0x8087},
    {0x0010,0xa43 ,28, 0xffee},
    {0x0010,0xa43 ,28, 0x8090},
    {0x0010,0xa43 ,28, 0x55ee},
    {0x0010,0xa43 ,28, 0x8099},
    {0x0010,0xa43 ,28, 0x55ee},
    {0x0010,0xa43 ,28, 0x809a},
    {0x0010,0xa43 ,28, 0x89ee},
    {0x0010,0xa43 ,28, 0x809b},
    {0x0010,0xa43 ,28, 0x11ee},
    {0x0010,0xa43 ,28, 0x80a0},
    {0x0010,0xa43 ,28, 0x00ee},
    {0x0010,0xa43 ,28, 0x80a2},
    {0x0010,0xa43 ,28, 0xffee},
    {0x0010,0xa43 ,28, 0x80a3},
    {0x0010,0xa43 ,28, 0x92ee},
    {0x0010,0xa43 ,28, 0x80a4},
    {0x0010,0xa43 ,28, 0x33ee},
    {0x0010,0xa43 ,28, 0x80a9},
    {0x0010,0xa43 ,28, 0x22ee},
    {0x0010,0xa43 ,28, 0x80ab},
    {0x0010,0xa43 ,28, 0xffee},
    {0x0010,0xa43 ,28, 0x80b4},
    {0x0010,0xa43 ,28, 0xffd2},
    {0x0010,0xa43 ,28, 0x0002},
    {0x0010,0xa43 ,28, 0x0cdc},
    {0x0010,0xa43 ,28, 0x0284},
    {0x0010,0xa43 ,28, 0xae02},
    {0x0010,0xa43 ,28, 0x849c},
    {0x0010,0xa43 ,28, 0x04f8},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0xf2a0},
    {0x0010,0xa43 ,28, 0x0005},
    {0x0010,0xa43 ,28, 0x0284},
    {0x0010,0xa43 ,28, 0x0dae},
    {0x0010,0xa43 ,28, 0x08a0},
    {0x0010,0xa43 ,28, 0x0105},
    {0x0010,0xa43 ,28, 0x0284},
    {0x0010,0xa43 ,28, 0x54ae},
    {0x0010,0xa43 ,28, 0x00fc},
    {0x0010,0xa43 ,28, 0x04f8},
    {0x0010,0xa43 ,28, 0xfaef},
    {0x0010,0xa43 ,28, 0x69e0},
    {0x0010,0xa43 ,28, 0x8239},
    {0x0010,0xa43 ,28, 0xf621},
    {0x0010,0xa43 ,28, 0xe482},
    {0x0010,0xa43 ,28, 0x39bf},
    {0x0010,0xa43 ,28, 0x3966},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x7ae5},
    {0x0010,0xa43 ,28, 0x82f3},
    {0x0010,0xa43 ,28, 0xe080},
    {0x0010,0xa43 ,28, 0x12ad},
    {0x0010,0xa43 ,28, 0x2427},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0xf3ad},
    {0x0010,0xa43 ,28, 0x2019},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0x37f7},
    {0x0010,0xa43 ,28, 0x21e4},
    {0x0010,0xa43 ,28, 0x8237},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0x54f7},
    {0x0010,0xa43 ,28, 0x21e4},
    {0x0010,0xa43 ,28, 0x8254},
    {0x0010,0xa43 ,28, 0x023b},
    {0x0010,0xa43 ,28, 0xa9ee},
    {0x0010,0xa43 ,28, 0x82f2},
    {0x0010,0xa43 ,28, 0x01ae},
    {0x0010,0xa43 ,28, 0x08d1},
    {0x0010,0xa43 ,28, 0x00bf},
    {0x0010,0xa43 ,28, 0x85d2},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x3cef},
    {0x0010,0xa43 ,28, 0x69fe},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0xf8fb},
    {0x0010,0xa43 ,28, 0x023b},
    {0x0010,0xa43 ,28, 0xcead},
    {0x0010,0xa43 ,28, 0x501f},
    {0x0010,0xa43 ,28, 0xd101},
    {0x0010,0xa43 ,28, 0xbf85},
    {0x0010,0xa43 ,28, 0xd202},
    {0x0010,0xa43 ,28, 0x373c},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0x54f6},
    {0x0010,0xa43 ,28, 0x21e4},
    {0x0010,0xa43 ,28, 0x8254},
    {0x0010,0xa43 ,28, 0x023b},
    {0x0010,0xa43 ,28, 0xa9e0},
    {0x0010,0xa43 ,28, 0x8237},
    {0x0010,0xa43 ,28, 0xf621},
    {0x0010,0xa43 ,28, 0xe482},
    {0x0010,0xa43 ,28, 0x37ee},
    {0x0010,0xa43 ,28, 0x82f2},
    {0x0010,0xa43 ,28, 0x00ff},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0xf8fa},
    {0x0010,0xa43 ,28, 0xef69},
    {0x0010,0xa43 ,28, 0xbf39},
    {0x0010,0xa43 ,28, 0x6602},
    {0x0010,0xa43 ,28, 0x377a},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0xf31f},
    {0x0010,0xa43 ,28, 0x019e},
    {0x0010,0xa43 ,28, 0x08e0},
    {0x0010,0xa43 ,28, 0x8239},
    {0x0010,0xa43 ,28, 0xf721},
    {0x0010,0xa43 ,28, 0xe482},
    {0x0010,0xa43 ,28, 0x39ef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0xf8fa},
    {0x0010,0xa43 ,28, 0xef69},
    {0x0010,0xa43 ,28, 0xbf39},
    {0x0010,0xa43 ,28, 0x6602},
    {0x0010,0xa43 ,28, 0x377a},
    {0x0010,0xa43 ,28, 0xe582},
    {0x0010,0xa43 ,28, 0xf3ef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0xee87},
    {0x0010,0xa43 ,28, 0xfe44},
    {0x0010,0xa43 ,28, 0xee87},
    {0x0010,0xa43 ,28, 0xff44},
    {0x0010,0xa43 ,28, 0x04f8},
    {0x0010,0xa43 ,28, 0xfaef},
    {0x0010,0xa43 ,28, 0x69bf},
    {0x0010,0xa43 ,28, 0x85cf},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x7aad},
    {0x0010,0xa43 ,28, 0x2808},
    {0x0010,0xa43 ,28, 0xe082},
    {0x0010,0xa43 ,28, 0x39f7},
    {0x0010,0xa43 ,28, 0x22e4},
    {0x0010,0xa43 ,28, 0x8239},
    {0x0010,0xa43 ,28, 0xef96},
    {0x0010,0xa43 ,28, 0xfefc},
    {0x0010,0xa43 ,28, 0x04f8},
    {0x0010,0xa43 ,28, 0xfaef},
    {0x0010,0xa43 ,28, 0x69e0},
    {0x0010,0xa43 ,28, 0x8239},
    {0x0010,0xa43 ,28, 0xf622},
    {0x0010,0xa43 ,28, 0xe482},
    {0x0010,0xa43 ,28, 0x39e0},
    {0x0010,0xa43 ,28, 0x8012},
    {0x0010,0xa43 ,28, 0xad26},
    {0x0010,0xa43 ,28, 0x22bf},
    {0x0010,0xa43 ,28, 0x3885},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x7aad},
    {0x0010,0xa43 ,28, 0x2819},
    {0x0010,0xa43 ,28, 0xbf85},
    {0x0010,0xa43 ,28, 0xcc02},
    {0x0010,0xa43 ,28, 0x377a},
    {0x0010,0xa43 ,28, 0xac28},
    {0x0010,0xa43 ,28, 0x0802},
    {0x0010,0xa43 ,28, 0x8512},
    {0x0010,0xa43 ,28, 0x0285},
    {0x0010,0xa43 ,28, 0x32ae},
    {0x0010,0xa43 ,28, 0x08e0},
    {0x0010,0xa43 ,28, 0x87ff},
    {0x0010,0xa43 ,28, 0xe187},
    {0x0010,0xa43 ,28, 0xfeae},
    {0x0010,0xa43 ,28, 0xf3d1},
    {0x0010,0xa43 ,28, 0x00bf},
    {0x0010,0xa43 ,28, 0x85cf},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x3cef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0xf9fa},
    {0x0010,0xa43 ,28, 0xef69},
    {0x0010,0xa43 ,28, 0xe287},
    {0x0010,0xa43 ,28, 0xfd4a},
    {0x0010,0xa43 ,28, 0x09bf},
    {0x0010,0xa43 ,28, 0x8076},
    {0x0010,0xa43 ,28, 0x1a92},
    {0x0010,0xa43 ,28, 0xdaef},
    {0x0010,0xa43 ,28, 0x020c},
    {0x0010,0xa43 ,28, 0x0158},
    {0x0010,0xa43 ,28, 0x705a},
    {0x0010,0xa43 ,28, 0x071e},
    {0x0010,0xa43 ,28, 0x0219},
    {0x0010,0xa43 ,28, 0xd9ef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfd04},
    {0x0010,0xa43 ,28, 0xf8f9},
    {0x0010,0xa43 ,28, 0xfaef},
    {0x0010,0xa43 ,28, 0x69fa},
    {0x0010,0xa43 ,28, 0xfbbf},
    {0x0010,0xa43 ,28, 0x8567},
    {0x0010,0xa43 ,28, 0xd600},
    {0x0010,0xa43 ,28, 0x0002},
    {0x0010,0xa43 ,28, 0x856f},
    {0x0010,0xa43 ,28, 0xad50},
    {0x0010,0xa43 ,28, 0x02f7},
    {0x0010,0xa43 ,28, 0x40a9},
    {0x0010,0xa43 ,28, 0x856d},
    {0x0010,0xa43 ,28, 0x06a6},
    {0x0010,0xa43 ,28, 0x000f},
    {0x0010,0xa43 ,28, 0x0cae},
    {0x0010,0xa43 ,28, 0x0e0f},
    {0x0010,0xa43 ,28, 0x44d2},
    {0x0010,0xa43 ,28, 0x021a},
    {0x0010,0xa43 ,28, 0x920c},
    {0x0010,0xa43 ,28, 0x61ae},
    {0x0010,0xa43 ,28, 0xe40f},
    {0x0010,0xa43 ,28, 0x44ae},
    {0x0010,0xa43 ,28, 0xdaff},
    {0x0010,0xa43 ,28, 0xfeef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfdfc},
    {0x0010,0xa43 ,28, 0x043a},
    {0x0010,0xa43 ,28, 0x473a},
    {0x0010,0xa43 ,28, 0x4a3a},
    {0x0010,0xa43 ,28, 0x3e3a},
    {0x0010,0xa43 ,28, 0x41f8},
    {0x0010,0xa43 ,28, 0xf9fa},
    {0x0010,0xa43 ,28, 0xef69},
    {0x0010,0xa43 ,28, 0xda19},
    {0x0010,0xa43 ,28, 0xdbef},
    {0x0010,0xa43 ,28, 0x95ef},
    {0x0010,0xa43 ,28, 0x7959},
    {0x0010,0xa43 ,28, 0x0fef},
    {0x0010,0xa43 ,28, 0x2102},
    {0x0010,0xa43 ,28, 0x377a},
    {0x0010,0xa43 ,28, 0x1b21},
    {0x0010,0xa43 ,28, 0x9e0f},
    {0x0010,0xa43 ,28, 0xab03},
    {0x0010,0xa43 ,28, 0x81ae},
    {0x0010,0xa43 ,28, 0x0111},
    {0x0010,0xa43 ,28, 0xef97},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x3cf6},
    {0x0010,0xa43 ,28, 0x50ae},
    {0x0010,0xa43 ,28, 0x02f7},
    {0x0010,0xa43 ,28, 0x50ef},
    {0x0010,0xa43 ,28, 0x96fe},
    {0x0010,0xa43 ,28, 0xfdfc},
    {0x0010,0xa43 ,28, 0x04f8},
    {0x0010,0xa43 ,28, 0xf9fa},
    {0x0010,0xa43 ,28, 0xcefa},
    {0x0010,0xa43 ,28, 0xef69},
    {0x0010,0xa43 ,28, 0xfbbf},
    {0x0010,0xa43 ,28, 0x85c8},
    {0x0010,0xa43 ,28, 0xd700},
    {0x0010,0xa43 ,28, 0x04d8},
    {0x0010,0xa43 ,28, 0x19d9},
    {0x0010,0xa43 ,28, 0x19da},
    {0x0010,0xa43 ,28, 0x19db},
    {0x0010,0xa43 ,28, 0x1907},
    {0x0010,0xa43 ,28, 0xef95},
    {0x0010,0xa43 ,28, 0x0237},
    {0x0010,0xa43 ,28, 0x3c07},
    {0x0010,0xa43 ,28, 0x3f00},
    {0x0010,0xa43 ,28, 0x049f},
    {0x0010,0xa43 ,28, 0xecff},
    {0x0010,0xa43 ,28, 0xef96},
    {0x0010,0xa43 ,28, 0xfec6},
    {0x0010,0xa43 ,28, 0xfefd},
    {0x0010,0xa43 ,28, 0xfc04},
    {0x0010,0xa43 ,28, 0x001a},
    {0x0010,0xa43 ,28, 0x3b3d},
    {0x0010,0xa43 ,28, 0x00a4},
    {0x0010,0xa43 ,28, 0x4011},
    {0x0010,0xa43 ,28, 0xa440},
    {0x0010,0xa43 ,28, 0x22a4},
    {0x0010,0xa43 ,28, 0x6c11},
    {0x0010,0xa43 ,28, 0xa46c},
    {0x0010,0xa43 ,27, 0xb818},
    {0x0010,0xa43 ,28, 0x0e19},
    {0x0010,0xa43 ,27, 0xb81a},
    {0x0010,0xa43 ,28, 0x194c},
    {0x0010,0xa43 ,27, 0xb81c},
    {0x0010,0xa43 ,28, 0x0351},
    {0x0010,0xa43 ,27, 0xb81e},
    {0x0010,0xa43 ,28, 0x01dd},
    {0x0010,0xa43 ,27, 0xb820},
    {0x0010,0xa43 ,28, 0x000f},
    {0x0010,0xa43 ,27, 0x0000},
    {0x0010,0xa43 ,28, 0x0000},
    {0x0010,0xb82 ,23, 0x0000},
    {0x0010,0xa43 ,27, 0x8146},
    {0x0010,0xa43 ,28, 0x0000}
};
#endif

/*
 * Function Declaration
 */

int32 dal_apollomp_switch_phyPatch(void);

static int32
_dal_apollomp_switch_green_disable(void);

static int32
_dal_apollomp_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
        case RTK_PORT_UTP1:
            *pPortId = 1;
            break;
        case RTK_PORT_UTP2:
            *pPortId = 2;
            break;
        case RTK_PORT_UTP3:
            *pPortId = 3;
            break;
        case RTK_PORT_EXT0:
            *pPortId = 5;
            break;
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
        case RTK_PORT_PON:
            *pPortId = 4;
            break;
#else
        case RTK_PORT_UTP4:
            *pPortId = 4;
            break;
#endif
        case RTK_PORT_CPU:
            *pPortId = 6;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}




static int32
_dal_apollomp_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 1:
            *pPortName = RTK_PORT_UTP1;
            break;
        case 2:
            *pPortName = RTK_PORT_UTP2;
            break;
        case 3:
            *pPortName = RTK_PORT_UTP3;
            break;
        case 5:
            *pPortName = RTK_PORT_EXT0;
            break;
        case 4:
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
            *pPortName = RTK_PORT_PON;
#else
            *pPortName = RTK_PORT_UTP4;
#endif
            break;
        case 6:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}


static int32
_dal_apollomp_switch_tickToken_init(void)
{
    int ret;
    uint32 wData;

    /*meter pon-tick-token configuration*/
    wData = 53;
    if ((ret = reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 58;
    if ((ret = reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /*meter switch-tick-token configuration*/
    wData = 53;
    if ((ret = reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 58;
    if ((ret = reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32
_dal_apollomp_switch_green_disable(void)
{
    int ret;
    rtk_port_t port;

    /* === Green enable === */
    for(port = HAL_GET_MIN_GE_PORT(); port <= HAL_GET_MAX_GE_PORT(); port++)
    {
        if(HAL_IS_PORT_EXIST(port)&& !HAL_IS_PON_PORT(port) && !HAL_IS_RGMII_PORT(port) && !HAL_IS_CPU_PORT(port))
        {

            /*short bit [13:11] abiq_busy=3'b001; bit [10:8] abiq_idle=3'b001; bit [7:4]ldvbias_busy=4'b0001; bit [3:0]ldvbias_busy=4'b0001*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x809a)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0xA444)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*short bit [6]=1,  bit [5:4]=2'b00 bypass viterbi; bit [2:0] adc_ibset = 2'b000*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x809f)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B23)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*mid bit [13:11] abiq_busy=3'b010; bit [10:8] abiq_idle=3'b010; bit [7:4] ldvbias_busy=4'b0011; bit [3:0] ldvbias_busy=4'b0011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80a3)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0xA444)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*mid bit [6]=0,  bit [5:4]=2'b10 full viterbi; bit [2:0] adc_ibset = 2'b011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80a8)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B23)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*long bit [13:11] abiq_busy=3'b100; bit [10:8] abiq_idle=3'b100; bit [7:4] ldvbias_busy=4'b0100; bit [3:0] ldvbias_busy=4'b0100*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80ac)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0xA444)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*long bit [6]=0,  bit [5:4]=2'b10 full viterbi; bit [2:0] adc_ibset = 2'b011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80b1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B23)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
        }
    }
    return RT_ERR_OK;
}

static int32
_dal_apollomp_switch_green_enable(void)
{
    int ret;
    rtk_port_t port;

    /* === Green enable === */
    for(port = HAL_GET_MIN_GE_PORT(); port <= HAL_GET_MAX_GE_PORT(); port++)
    {
        if(HAL_IS_PORT_EXIST(port)&& !HAL_IS_PON_PORT(port) && !HAL_IS_RGMII_PORT(port) && !HAL_IS_CPU_PORT(port))
        {

            /*short bit [13:11] abiq_busy=3'b001; bit [10:8] abiq_idle=3'b001; bit [7:4]ldvbias_busy=4'b0001; bit [3:0]ldvbias_busy=4'b0001*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x809a)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x8911)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*short bit [6]=1,  bit [5:4]=2'b00 bypass viterbi; bit [2:0] adc_ibset = 2'b000*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x809f)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B00)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*mid bit [13:11] abiq_busy=3'b010; bit [10:8] abiq_idle=3'b010; bit [7:4] ldvbias_busy=4'b0011; bit [3:0] ldvbias_busy=4'b0011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80a3)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x9233)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*mid bit [6]=0,  bit [5:4]=2'b10 full viterbi; bit [2:0] adc_ibset = 2'b011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80a8)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B22)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*long bit [13:11] abiq_busy=3'b100; bit [10:8] abiq_idle=3'b100; bit [7:4] ldvbias_busy=4'b0100; bit [3:0] ldvbias_busy=4'b0100*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80ac)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0xA444)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }

            /*long bit [6]=0,  bit [5:4]=2'b10 full viterbi; bit [2:0] adc_ibset = 2'b011*/
            if ((ret = hal_miim_write(port, 0xa43, 27, 0x80b1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
            if ((ret = hal_miim_write(port, 0xa43, 28, 0x6B23)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;
            }
        }
    }
    return RT_ERR_OK;
}

static int32
_dal_apollomp_switch_eee_enable(void)
{
#if 1
    int ret;
    uint32 wData;
    uint16 phyData;
    uint32 i,phy;

    /* EEE_MISC */
    wData = 0xA0;
    if ((ret = reg_write(APOLLOMP_EEE_MISCr, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    for(i = 0; i < 5 ; i++)
    {
        /* MAC0~4(GPHY0) */
        wData = 0x3c0;
        if ((ret = reg_array_write(APOLLOMP_P_EEECFGr, i, REG_ARRAY_INDEX_NONE, &wData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
    }

    /* EEE_GIGA_CTRL0.EEE_TW_GIGA=0x11 */
    wData = 0x11;
    if ((ret = reg_field_write(APOLLOMP_EEE_GIGA_CTRL0r, APOLLOMP_EEE_TW_GIGAf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* EEE_100M_CTRL0.EEE_TW_100M=0x1c */
    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_EEE_100M_CTRL0r, APOLLOMP_EEE_TW_100Mf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    
    /*EEE_GIGA_CTRL1=0xC80A */
    wData = 0xC80A;
    if ((ret = reg_write(APOLLOMP_EEE_GIGA_CTRL1r, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    /*WAKELPI_SLOT_PRD.WAKE_LPI_SLOT_PRD = 0x13*/
    wData = 0x13;
    if ((ret = reg_field_write(APOLLOMP_WAKELPI_SLOT_PRDr, APOLLOMP_WAKE_LPI_SLOT_PRDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /*EEE_TX_THR_GIGA.TX_RATE_EEE_GIGA=0xd*/
    wData = 0xd;
    if ((ret = reg_field_write(APOLLOMP_EEE_TX_THR_GIGAr, APOLLOMP_TX_RATE_EEE_GIGAf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /* EEE_TX_THR_FE.TX_RATE_EEE_100M = 0xd */
    wData = 0xd;
    if ((ret = reg_field_write(APOLLOMP_EEE_TX_THR_FEr, APOLLOMP_TX_RATE_EEE_100Mf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*EEE_100M_CTRL1=0xC80A */
    wData = 0xC80A;
    if ((ret = reg_write(APOLLOMP_EEE_100M_CTRL1r, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* EEE_BURSTSIZE */
    wData = 0x1000;
    if ((ret = reg_write(APOLLOMP_EEE_BURSTSIZEr, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /* TRAP_PRI */
    wData = 0x0007;
    if ((ret = reg_write(APOLLOMP_EEELLDP_CTRL_1r, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /* WAKELPI_SLOT 0x29062080*/
    wData = 0x29062080;
    if ((ret = reg_write(APOLLOMP_WAKELPI_SLOTr, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* EEE_RXIDLE 0x10101*/
    wData = 0x10101;
    if ((ret = reg_write(APOLLOMP_EEE_RXIDLEr, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    /*page 0xbc0 reg 0x16 bit 8 set to 0*/
    for(phy = 0; phy < 5 ; phy++)
    {
        if((ret=apollomp_interPhy_read(phy,0xbc0,0x16,&phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }
        phyData = phyData & 0xFEFF;
        if((ret=apollomp_interPhy_write(phy,0xbc0,0x16,phyData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;

        }

    }

#endif
    return RT_ERR_OK;
}

static int32
_dal_apollomp_switch_powerSaving_init(void)
{
    int ret;

    /* === Green enable === */
    if ((ret = _dal_apollomp_switch_green_disable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === EEE enable === */
    if ((ret = _dal_apollomp_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



int32
dal_apollomp_switch_phyPatch(void)
{
    int i;
    int ret;
    uint16 phy;
    dal_apollomp_phy_data_t *patchArray;
    uint32   patchSize,wData, cnt = 0;
    uint16   rData;


    /*phy patch done*/
    wData = 0;
    if ((ret = reg_field_write(APOLLOMP_WRAP_GPHY_MISCr,APOLLOMP_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /* delay 10ms */
    osal_time_mdelay(10);

    patchArray = phyPatchArray;
    patchSize = sizeof(phyPatchArray)/sizeof(dal_apollomp_phy_data_t);


    /*set phy reset*/
    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr,APOLLOMP_CMD_GPHY_RST_PSf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    do
    {
        if((ret=apollomp_interPhy_read(4, 0xA46, 21, &rData))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
            return ret;
        }
        cnt++;
        if(cnt > 10000)
        {
            break;
        }
    }
    while( ((rData & 0x0700) >> 8) != 0x2 );

    if(cnt > 10000)
    {
        /* Can't polling PHY ready */
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*start patch phy*/
    for(i=0 ; i<patchSize ; i++)
    {
        if(patchArray[i].phy == DAL_APOLLOMP_BROADCAST_PHYID)
        {
            for(phy = 0; phy < 5 ; phy++)
            {
                if((ret=apollomp_interPhy_write(phy,
                                      patchArray[i].page,
                                      patchArray[i].reg,
                                      patchArray[i].data))!=RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                    return ret;

                }
            }
        }
        else
        {
            if((ret=apollomp_interPhy_write(patchArray[i].phy,
                                  patchArray[i].page,
                                  patchArray[i].reg,
                                  patchArray[i].data))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
                return ret;

            }
        }
    }

    return RT_ERR_OK;
}


static int32 _dal_apollomp_get_chip_version(void)
{
    int32   ret;    
    uint32  value;
    if (drv_swcore_cid_get(&chipId, &chipRev) != RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    value = 0xb;
    if ((ret = reg_field_write(APOLLOMP_BOND_INFOr,APOLLOMP_BOND_INFO_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_BOND_INFOr,APOLLOMP_BOND_CHIP_MODEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    chipSubtype = value & 0x1f;

    value = 0x0;
    if ((ret = reg_field_write(APOLLOMP_BOND_INFOr,APOLLOMP_BOND_INFO_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
}

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_apollomp_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_apollomp_switch_init(void)
{
    int32   ret;
    uint32 wData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

    /*get chip id*/
    if((ret = _dal_apollomp_get_chip_version())!=RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*flow control threshold and set buffer mode*/
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
    if((ret = apollomp_raw_flowctrl_patch(FLOWCTRL_PATCH_35M_GPON)) != RT_ERR_OK)
#else
    if((ret = apollomp_raw_flowctrl_patch(FLOWCTRL_PATCH_20M)) != RT_ERR_OK)
#endif
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*meter tick-token configuration*/
    if((ret = _dal_apollomp_switch_tickToken_init())!=RT_ERR_OK)
    {
 	 RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

#if defined(FPGA_DEFINED)

#else
    /*CFG_PHY_POLL_CMD set to 0x1100*/
    wData = 0x1100;
    if ((ret = reg_write(APOLLOMP_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    /*phy patch*/
    if((ret=dal_apollomp_switch_phyPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }

    /* power saving feature */
    if((ret=_dal_apollomp_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_WRAP_GPHY_MISCr,APOLLOMP_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }


    /*patch port 4 power up*/
    if((ret=apollomp_interPhy_write(4,0,0,0x1340))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*CFG_PHY_POLL_CMD set to 0x1110*/
    wData = 0x1110;
    if ((ret = reg_write(APOLLOMP_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
    
    wData = 0;
    if ((ret = reg_field_write(APOLLOMP_L34_GLB_CFGr, APOLLOMP_CFG_CPU_ORG_OPf, (uint32 *)&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_apollomp_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
dal_apollomp_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_apollomp_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = APOLLOMP_ASIC_PON;

    switch(asicType)
    {
        case APOLLOMP_ASIC_PON:
        default:
            return _dal_apollomp_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_phyPortId_get */



/* Function Name:
 *      dal_apollomp_switch_logicalPort_get
 * Description:
 *      Get logical port name from physical port id
 * Input:
 *      portId  - physical port id
 * Output:
 *      pPortName - pointer to logical port name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_apollomp_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_apollomp_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = APOLLOMP_ASIC_PON;

    switch(asicType)
    {
        case APOLLOMP_ASIC_PON:
        default:
            return _dal_apollomp_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_logicalPort_get */



/* Function Name:
 *      dal_apollomp_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_apollomp_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_port2PortMask_set */



/* Function Name:
 *      dal_apollomp_switch_port2PortMask_clear
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_apollomp_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_port2PortMask_clear */



/* Function Name:
 *      dal_apollomp_switch_portIdInMask_check
 * Description:
 *      Check if given port is in port list
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_apollomp_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_apollomp_switch_portIdInMask_check */


/* Function Name:
  *      dal_apollomp_switch_maxPktLenLinkSpeed_get
  * Description:
  *      Get the max packet length setting of the specific speed type
  * Input:
  *      speed - speed type
  * Output:
  *      pLen  - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_apollomp_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG0r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG0f;
    }
    else
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG1r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG1f;
    }

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_apollomp_switch_maxPktLenLinkSpeed_set
  * Description:
  *      Set the max packet length of the specific speed type
  * Input:
  *      speed - speed type
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_apollomp_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    rtk_port_t  port, max_port;
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;
    uint32 portField;
    uint32 index;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG0r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG0f;
        portField = APOLLOMP_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
	    regAddr   = APOLLOMP_MAX_LENGTH_CFG1r;
        fieldIdx  = APOLLOMP_ACCEPT_MAX_LENTH_CFG1f;
        portField = APOLLOMP_MAX_LENGTH_GIGAf;
        index = 1;
    }

    if ((ret = reg_field_write(regAddr, fieldIdx, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    /*set all port index to currect length index*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        if ((ret = reg_array_field_write(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      dal_apollomp_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      None
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_apollomp_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = apollomp_raw_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_apollomp_switch_chip_reset
 * Description:
 *      Reset switch chip
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr,APOLLOMP_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_apollomp_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
dal_apollomp_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pChipId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRev), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSubtype), RT_ERR_NULL_POINTER);

    /* function body */
    *pChipId = chipId;
    *pRev = chipRev;
    *pSubtype = chipSubtype;

    return RT_ERR_OK;
}   /* end of dal_apollomp_switch_version_get */

