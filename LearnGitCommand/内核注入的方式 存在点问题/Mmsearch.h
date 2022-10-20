#pragma once

#define      NON              0x3f

#define      END              0x00

#define      WIN7               7601
#define      WIN8_1             9600
#define      WIN10_1607         14393
#define      WIN10_1703         15063
#define      WIN10_1709         16299
#define      WIN10_1803         17134
#define      WIN10_1809         17763
#define      WIN10_1903         18362
#define      WIN10_1909         18363
#define      WIN10_2004         19041
#define      WIN10_20H2         19042
#define      WIN10_21H1         19043
#define      WIN10_21H2         19044
#define      WIN11_21H2         22000

PVOID       AsdGetModuleExport(IN PVOID pBase, IN PCCHAR name_ord);
ULONG64     AsdGetProcessMoudleBase(PEPROCESS   pep, PWCHAR  buffer);
