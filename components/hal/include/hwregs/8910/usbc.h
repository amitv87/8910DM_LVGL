/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _USBC_H_
#define _USBC_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_USBC_BASE (0x09040000)

typedef volatile struct
{
    uint32_t gotgctl;    // 0x00000000
    uint32_t gotgint;    // 0x00000004
    uint32_t gahbcfg;    // 0x00000008
    uint32_t gusbcfg;    // 0x0000000c
    uint32_t grstctl;    // 0x00000010
    uint32_t gintsts;    // 0x00000014
    uint32_t gintmsk;    // 0x00000018
    uint32_t grxstsr;    // 0x0000001c
    uint32_t grxstsp;    // 0x00000020
    uint32_t grxfsiz;    // 0x00000024
    uint32_t gnptxfsiz;  // 0x00000028
    uint32_t gnptxsts;   // 0x0000002c
    uint32_t __48[2];    // 0x00000030
    uint32_t ggpio;      // 0x00000038
    uint32_t guid;       // 0x0000003c
    uint32_t gsnpsid;    // 0x00000040
    uint32_t ghwcfg1;    // 0x00000044
    uint32_t ghwcfg2;    // 0x00000048
    uint32_t ghwcfg3;    // 0x0000004c
    uint32_t ghwcfg4;    // 0x00000050
    uint32_t __84[1];    // 0x00000054
    uint32_t gpwrdn;     // 0x00000058
    uint32_t gdfifocfg;  // 0x0000005c
    uint32_t gadpctl;    // 0x00000060
    uint32_t __100[39];  // 0x00000064
    uint32_t hptxfsiz;   // 0x00000100
    uint32_t dieptxf1;   // 0x00000104
    uint32_t dieptxf2;   // 0x00000108
    uint32_t dieptxf3;   // 0x0000010c
    uint32_t dieptxf4;   // 0x00000110
    uint32_t dieptxf5;   // 0x00000114
    uint32_t dieptxf6;   // 0x00000118
    uint32_t dieptxf7;   // 0x0000011c
    uint32_t dieptxf8;   // 0x00000120
    uint32_t __292[183]; // 0x00000124
    uint32_t hcfg;       // 0x00000400
    uint32_t hfir;       // 0x00000404
    uint32_t hfnum;      // 0x00000408
    uint32_t __1036[1];  // 0x0000040c
    uint32_t hptxsts;    // 0x00000410
    uint32_t haint;      // 0x00000414
    uint32_t haintmsk;   // 0x00000418
    uint32_t hflbaddr;   // 0x0000041c
    uint32_t __1056[8];  // 0x00000420
    uint32_t hprt;       // 0x00000440
    uint32_t __1092[47]; // 0x00000444
    uint32_t hcchar0;    // 0x00000500
    uint32_t hcsplt0;    // 0x00000504
    uint32_t hcint0;     // 0x00000508
    uint32_t hcintmsk0;  // 0x0000050c
    uint32_t hctsiz0;    // 0x00000510
    uint32_t hcdma0;     // 0x00000514
    uint32_t __1304[1];  // 0x00000518
    uint32_t hcdmab0;    // 0x0000051c
    uint32_t hcchar1;    // 0x00000520
    uint32_t hcsplt1;    // 0x00000524
    uint32_t hcint1;     // 0x00000528
    uint32_t hcintmsk1;  // 0x0000052c
    uint32_t hctsiz1;    // 0x00000530
    uint32_t hcdma1;     // 0x00000534
    uint32_t __1336[1];  // 0x00000538
    uint32_t hcdmab1;    // 0x0000053c
    uint32_t hcchar2;    // 0x00000540
    uint32_t hcsplt2;    // 0x00000544
    uint32_t hcint2;     // 0x00000548
    uint32_t hcintmsk2;  // 0x0000054c
    uint32_t hctsiz2;    // 0x00000550
    uint32_t hcdma2;     // 0x00000554
    uint32_t __1368[1];  // 0x00000558
    uint32_t hcdmab2;    // 0x0000055c
    uint32_t hcchar3;    // 0x00000560
    uint32_t hcsplt3;    // 0x00000564
    uint32_t hcint3;     // 0x00000568
    uint32_t hcintmsk3;  // 0x0000056c
    uint32_t hctsiz3;    // 0x00000570
    uint32_t hcdma3;     // 0x00000574
    uint32_t __1400[1];  // 0x00000578
    uint32_t hcdmab3;    // 0x0000057c
    uint32_t hcchar4;    // 0x00000580
    uint32_t hcsplt4;    // 0x00000584
    uint32_t hcint4;     // 0x00000588
    uint32_t hcintmsk4;  // 0x0000058c
    uint32_t hctsiz4;    // 0x00000590
    uint32_t hcdma4;     // 0x00000594
    uint32_t __1432[1];  // 0x00000598
    uint32_t hcdmab4;    // 0x0000059c
    uint32_t hcchar5;    // 0x000005a0
    uint32_t hcsplt5;    // 0x000005a4
    uint32_t hcint5;     // 0x000005a8
    uint32_t hcintmsk5;  // 0x000005ac
    uint32_t hctsiz5;    // 0x000005b0
    uint32_t hcdma5;     // 0x000005b4
    uint32_t __1464[1];  // 0x000005b8
    uint32_t hcdmab5;    // 0x000005bc
    uint32_t hcchar6;    // 0x000005c0
    uint32_t hcsplt6;    // 0x000005c4
    uint32_t hcint6;     // 0x000005c8
    uint32_t hcintmsk6;  // 0x000005cc
    uint32_t hctsiz6;    // 0x000005d0
    uint32_t hcdma6;     // 0x000005d4
    uint32_t __1496[1];  // 0x000005d8
    uint32_t hcdmab6;    // 0x000005dc
    uint32_t hcchar7;    // 0x000005e0
    uint32_t hcsplt7;    // 0x000005e4
    uint32_t hcint7;     // 0x000005e8
    uint32_t hcintmsk7;  // 0x000005ec
    uint32_t hctsiz7;    // 0x000005f0
    uint32_t hcdma7;     // 0x000005f4
    uint32_t __1528[1];  // 0x000005f8
    uint32_t hcdmab7;    // 0x000005fc
    uint32_t hcchar8;    // 0x00000600
    uint32_t hcsplt8;    // 0x00000604
    uint32_t hcint8;     // 0x00000608
    uint32_t hcintmsk8;  // 0x0000060c
    uint32_t hctsiz8;    // 0x00000610
    uint32_t hcdma8;     // 0x00000614
    uint32_t __1560[1];  // 0x00000618
    uint32_t hcdmab8;    // 0x0000061c
    uint32_t hcchar9;    // 0x00000620
    uint32_t hcsplt9;    // 0x00000624
    uint32_t hcint9;     // 0x00000628
    uint32_t hcintmsk9;  // 0x0000062c
    uint32_t hctsiz9;    // 0x00000630
    uint32_t hcdma9;     // 0x00000634
    uint32_t __1592[1];  // 0x00000638
    uint32_t hcdmab9;    // 0x0000063c
    uint32_t hcchar10;   // 0x00000640
    uint32_t hcsplt10;   // 0x00000644
    uint32_t hcint10;    // 0x00000648
    uint32_t hcintmsk10; // 0x0000064c
    uint32_t hctsiz10;   // 0x00000650
    uint32_t hcdma10;    // 0x00000654
    uint32_t __1624[1];  // 0x00000658
    uint32_t hcdmab10;   // 0x0000065c
    uint32_t hcchar11;   // 0x00000660
    uint32_t hcsplt11;   // 0x00000664
    uint32_t hcint11;    // 0x00000668
    uint32_t hcintmsk11; // 0x0000066c
    uint32_t hctsiz11;   // 0x00000670
    uint32_t hcdma11;    // 0x00000674
    uint32_t __1656[1];  // 0x00000678
    uint32_t hcdmab11;   // 0x0000067c
    uint32_t hcchar12;   // 0x00000680
    uint32_t hcsplt12;   // 0x00000684
    uint32_t hcint12;    // 0x00000688
    uint32_t hcintmsk12; // 0x0000068c
    uint32_t hctsiz12;   // 0x00000690
    uint32_t hcdma12;    // 0x00000694
    uint32_t __1688[1];  // 0x00000698
    uint32_t hcdmab12;   // 0x0000069c
    uint32_t hcchar13;   // 0x000006a0
    uint32_t hcsplt13;   // 0x000006a4
    uint32_t hcint13;    // 0x000006a8
    uint32_t hcintmsk13; // 0x000006ac
    uint32_t hctsiz13;   // 0x000006b0
    uint32_t hcdma13;    // 0x000006b4
    uint32_t __1720[1];  // 0x000006b8
    uint32_t hcdmab13;   // 0x000006bc
    uint32_t hcchar14;   // 0x000006c0
    uint32_t hcsplt14;   // 0x000006c4
    uint32_t hcint14;    // 0x000006c8
    uint32_t hcintmsk14; // 0x000006cc
    uint32_t hctsiz14;   // 0x000006d0
    uint32_t hcdma14;    // 0x000006d4
    uint32_t __1752[1];  // 0x000006d8
    uint32_t hcdmab14;   // 0x000006dc
    uint32_t hcchar15;   // 0x000006e0
    uint32_t hcsplt15;   // 0x000006e4
    uint32_t hcint15;    // 0x000006e8
    uint32_t hcintmsk15; // 0x000006ec
    uint32_t hctsiz15;   // 0x000006f0
    uint32_t hcdma15;    // 0x000006f4
    uint32_t __1784[1];  // 0x000006f8
    uint32_t hcdmab15;   // 0x000006fc
    uint32_t __1792[64]; // 0x00000700
    uint32_t dcfg;       // 0x00000800
    uint32_t dctl;       // 0x00000804
    uint32_t dsts;       // 0x00000808
    uint32_t __2060[1];  // 0x0000080c
    uint32_t diepmsk;    // 0x00000810
    uint32_t doepmsk;    // 0x00000814
    uint32_t daint;      // 0x00000818
    uint32_t daintmsk;   // 0x0000081c
    uint32_t __2080[2];  // 0x00000820
    uint32_t dvbusdis;   // 0x00000828
    uint32_t dvbuspulse; // 0x0000082c
    uint32_t dthrctl;    // 0x00000830
    uint32_t diepempmsk; // 0x00000834
    uint32_t __2104[50]; // 0x00000838
    uint32_t diepctl0;   // 0x00000900
    uint32_t __2308[1];  // 0x00000904
    uint32_t diepint0;   // 0x00000908
    uint32_t __2316[1];  // 0x0000090c
    uint32_t dieptsiz0;  // 0x00000910
    uint32_t diepdma0;   // 0x00000914
    uint32_t dtxfsts0;   // 0x00000918
    uint32_t diepdmab0;  // 0x0000091c
    uint32_t diepctl1;   // 0x00000920
    uint32_t __2340[1];  // 0x00000924
    uint32_t diepint1;   // 0x00000928
    uint32_t __2348[1];  // 0x0000092c
    uint32_t dieptsiz1;  // 0x00000930
    uint32_t diepdma1;   // 0x00000934
    uint32_t dtxfsts1;   // 0x00000938
    uint32_t diepdmab1;  // 0x0000093c
    uint32_t diepctl2;   // 0x00000940
    uint32_t __2372[1];  // 0x00000944
    uint32_t diepint2;   // 0x00000948
    uint32_t __2380[1];  // 0x0000094c
    uint32_t dieptsiz2;  // 0x00000950
    uint32_t diepdma2;   // 0x00000954
    uint32_t dtxfsts2;   // 0x00000958
    uint32_t diepdmab2;  // 0x0000095c
    uint32_t diepctl3;   // 0x00000960
    uint32_t __2404[1];  // 0x00000964
    uint32_t diepint3;   // 0x00000968
    uint32_t __2412[1];  // 0x0000096c
    uint32_t dieptsiz3;  // 0x00000970
    uint32_t diepdma3;   // 0x00000974
    uint32_t dtxfsts3;   // 0x00000978
    uint32_t diepdmab3;  // 0x0000097c
    uint32_t diepctl4;   // 0x00000980
    uint32_t __2436[1];  // 0x00000984
    uint32_t diepint4;   // 0x00000988
    uint32_t __2444[1];  // 0x0000098c
    uint32_t dieptsiz4;  // 0x00000990
    uint32_t diepdma4;   // 0x00000994
    uint32_t dtxfsts4;   // 0x00000998
    uint32_t diepdmab4;  // 0x0000099c
    uint32_t diepctl5;   // 0x000009a0
    uint32_t __2468[1];  // 0x000009a4
    uint32_t diepint5;   // 0x000009a8
    uint32_t __2476[1];  // 0x000009ac
    uint32_t dieptsiz5;  // 0x000009b0
    uint32_t diepdma5;   // 0x000009b4
    uint32_t dtxfsts5;   // 0x000009b8
    uint32_t diepdmab5;  // 0x000009bc
    uint32_t diepctl6;   // 0x000009c0
    uint32_t __2500[1];  // 0x000009c4
    uint32_t diepint6;   // 0x000009c8
    uint32_t __2508[1];  // 0x000009cc
    uint32_t dieptsiz6;  // 0x000009d0
    uint32_t diepdma6;   // 0x000009d4
    uint32_t dtxfsts6;   // 0x000009d8
    uint32_t diepdmab6;  // 0x000009dc
    uint32_t diepctl7;   // 0x000009e0
    uint32_t __2532[1];  // 0x000009e4
    uint32_t diepint7;   // 0x000009e8
    uint32_t __2540[1];  // 0x000009ec
    uint32_t dieptsiz7;  // 0x000009f0
    uint32_t diepdma7;   // 0x000009f4
    uint32_t dtxfsts7;   // 0x000009f8
    uint32_t diepdmab7;  // 0x000009fc
    uint32_t diepctl8;   // 0x00000a00
    uint32_t __2564[1];  // 0x00000a04
    uint32_t diepint8;   // 0x00000a08
    uint32_t __2572[1];  // 0x00000a0c
    uint32_t dieptsiz8;  // 0x00000a10
    uint32_t diepdma8;   // 0x00000a14
    uint32_t dtxfsts8;   // 0x00000a18
    uint32_t diepdmab8;  // 0x00000a1c
    uint32_t diepctl9;   // 0x00000a20
    uint32_t __2596[1];  // 0x00000a24
    uint32_t diepint9;   // 0x00000a28
    uint32_t __2604[1];  // 0x00000a2c
    uint32_t dieptsiz9;  // 0x00000a30
    uint32_t diepdma9;   // 0x00000a34
    uint32_t dtxfsts9;   // 0x00000a38
    uint32_t diepdmab9;  // 0x00000a3c
    uint32_t diepctl10;  // 0x00000a40
    uint32_t __2628[1];  // 0x00000a44
    uint32_t diepint10;  // 0x00000a48
    uint32_t __2636[1];  // 0x00000a4c
    uint32_t dieptsiz10; // 0x00000a50
    uint32_t diepdma10;  // 0x00000a54
    uint32_t dtxfsts10;  // 0x00000a58
    uint32_t diepdmab10; // 0x00000a5c
    uint32_t diepctl11;  // 0x00000a60
    uint32_t __2660[1];  // 0x00000a64
    uint32_t diepint11;  // 0x00000a68
    uint32_t __2668[1];  // 0x00000a6c
    uint32_t dieptsiz11; // 0x00000a70
    uint32_t diepdma11;  // 0x00000a74
    uint32_t dtxfsts11;  // 0x00000a78
    uint32_t diepdmab11; // 0x00000a7c
    uint32_t diepctl12;  // 0x00000a80
    uint32_t __2692[1];  // 0x00000a84
    uint32_t diepint12;  // 0x00000a88
    uint32_t __2700[1];  // 0x00000a8c
    uint32_t dieptsiz12; // 0x00000a90
    uint32_t diepdma12;  // 0x00000a94
    uint32_t dtxfsts12;  // 0x00000a98
    uint32_t diepdmab12; // 0x00000a9c
    uint32_t __2720[24]; // 0x00000aa0
    uint32_t doepctl0;   // 0x00000b00
    uint32_t __2820[1];  // 0x00000b04
    uint32_t doepint0;   // 0x00000b08
    uint32_t __2828[1];  // 0x00000b0c
    uint32_t doeptsiz0;  // 0x00000b10
    uint32_t doepdma0;   // 0x00000b14
    uint32_t __2840[1];  // 0x00000b18
    uint32_t doepdmab0;  // 0x00000b1c
    uint32_t doepctl1;   // 0x00000b20
    uint32_t __2852[1];  // 0x00000b24
    uint32_t doepint1;   // 0x00000b28
    uint32_t __2860[1];  // 0x00000b2c
    uint32_t doeptsiz1;  // 0x00000b30
    uint32_t doepdma1;   // 0x00000b34
    uint32_t __2872[1];  // 0x00000b38
    uint32_t doepdmab1;  // 0x00000b3c
    uint32_t doepctl2;   // 0x00000b40
    uint32_t __2884[1];  // 0x00000b44
    uint32_t doepint2;   // 0x00000b48
    uint32_t __2892[1];  // 0x00000b4c
    uint32_t doeptsiz2;  // 0x00000b50
    uint32_t doepdma2;   // 0x00000b54
    uint32_t __2904[1];  // 0x00000b58
    uint32_t doepdmab2;  // 0x00000b5c
    uint32_t doepctl3;   // 0x00000b60
    uint32_t __2916[1];  // 0x00000b64
    uint32_t doepint3;   // 0x00000b68
    uint32_t __2924[1];  // 0x00000b6c
    uint32_t doeptsiz3;  // 0x00000b70
    uint32_t doepdma3;   // 0x00000b74
    uint32_t __2936[1];  // 0x00000b78
    uint32_t doepdmab3;  // 0x00000b7c
    uint32_t doepctl4;   // 0x00000b80
    uint32_t __2948[1];  // 0x00000b84
    uint32_t doepint4;   // 0x00000b88
    uint32_t __2956[1];  // 0x00000b8c
    uint32_t doeptsiz4;  // 0x00000b90
    uint32_t doepdma4;   // 0x00000b94
    uint32_t __2968[1];  // 0x00000b98
    uint32_t doepdmab4;  // 0x00000b9c
    uint32_t doepctl5;   // 0x00000ba0
    uint32_t __2980[1];  // 0x00000ba4
    uint32_t doepint5;   // 0x00000ba8
    uint32_t __2988[1];  // 0x00000bac
    uint32_t doeptsiz5;  // 0x00000bb0
    uint32_t doepdma5;   // 0x00000bb4
    uint32_t __3000[1];  // 0x00000bb8
    uint32_t doepdmab5;  // 0x00000bbc
    uint32_t doepctl6;   // 0x00000bc0
    uint32_t __3012[1];  // 0x00000bc4
    uint32_t doepint6;   // 0x00000bc8
    uint32_t __3020[1];  // 0x00000bcc
    uint32_t doeptsiz6;  // 0x00000bd0
    uint32_t doepdma6;   // 0x00000bd4
    uint32_t __3032[1];  // 0x00000bd8
    uint32_t doepdmab6;  // 0x00000bdc
    uint32_t doepctl7;   // 0x00000be0
    uint32_t __3044[1];  // 0x00000be4
    uint32_t doepint7;   // 0x00000be8
    uint32_t __3052[1];  // 0x00000bec
    uint32_t doeptsiz7;  // 0x00000bf0
    uint32_t doepdma7;   // 0x00000bf4
    uint32_t __3064[1];  // 0x00000bf8
    uint32_t doepdmab7;  // 0x00000bfc
    uint32_t doepctl8;   // 0x00000c00
    uint32_t __3076[1];  // 0x00000c04
    uint32_t doepint8;   // 0x00000c08
    uint32_t __3084[1];  // 0x00000c0c
    uint32_t doeptsiz8;  // 0x00000c10
    uint32_t doepdma8;   // 0x00000c14
    uint32_t __3096[1];  // 0x00000c18
    uint32_t doepdmab8;  // 0x00000c1c
    uint32_t doepctl9;   // 0x00000c20
    uint32_t __3108[1];  // 0x00000c24
    uint32_t doepint9;   // 0x00000c28
    uint32_t __3116[1];  // 0x00000c2c
    uint32_t doeptsiz9;  // 0x00000c30
    uint32_t doepdma9;   // 0x00000c34
    uint32_t __3128[1];  // 0x00000c38
    uint32_t doepdmab9;  // 0x00000c3c
    uint32_t doepctl10;  // 0x00000c40
    uint32_t __3140[1];  // 0x00000c44
    uint32_t doepint10;  // 0x00000c48
    uint32_t __3148[1];  // 0x00000c4c
    uint32_t doeptsiz10; // 0x00000c50
    uint32_t doepdma10;  // 0x00000c54
    uint32_t __3160[1];  // 0x00000c58
    uint32_t doepdmab10; // 0x00000c5c
    uint32_t doepctl11;  // 0x00000c60
    uint32_t __3172[1];  // 0x00000c64
    uint32_t doepint11;  // 0x00000c68
    uint32_t __3180[1];  // 0x00000c6c
    uint32_t doeptsiz11; // 0x00000c70
    uint32_t doepdma11;  // 0x00000c74
    uint32_t __3192[1];  // 0x00000c78
    uint32_t doepdmab11; // 0x00000c7c
    uint32_t doepctl12;  // 0x00000c80
    uint32_t __3204[1];  // 0x00000c84
    uint32_t doepint12;  // 0x00000c88
    uint32_t __3212[1];  // 0x00000c8c
    uint32_t doeptsiz12; // 0x00000c90
    uint32_t doepdma12;  // 0x00000c94
    uint32_t __3224[1];  // 0x00000c98
    uint32_t doepdmab12; // 0x00000c9c
    uint32_t __3232[88]; // 0x00000ca0
    uint32_t pcgcctl;    // 0x00000e00
} HWP_USBC_T;

#define hwp_usbc ((HWP_USBC_T *)REG_ACCESS_ADDRESS(REG_USBC_BASE))

// gotgctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sesreqscs : 1;       // [0], read only
        uint32_t sesreq : 1;          // [1]
        uint32_t vbvalidoven : 1;     // [2]
        uint32_t vbvalidovval : 1;    // [3]
        uint32_t avalidoven : 1;      // [4]
        uint32_t avalidovval : 1;     // [5]
        uint32_t bvalidoven : 1;      // [6]
        uint32_t bvalidovval : 1;     // [7]
        uint32_t hstnegscs : 1;       // [8], read only
        uint32_t hnpreq : 1;          // [9]
        uint32_t hstsethnpen : 1;     // [10]
        uint32_t devhnpen : 1;        // [11]
        uint32_t ehen : 1;            // [12]
        uint32_t __14_13 : 2;         // [14:13]
        uint32_t dbncefltrbypass : 1; // [15]
        uint32_t conidsts : 1;        // [16], read only
        uint32_t dbnctime : 1;        // [17], read only
        uint32_t asesvld : 1;         // [18]
        uint32_t bsesvld : 1;         // [19], read only
        uint32_t otgver : 1;          // [20]
        uint32_t curmod : 1;          // [21], read only
        uint32_t __31_22 : 10;        // [31:22]
    } b;
} REG_USBC_GOTGCTL_T;

// gotgint
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __1_0 : 2;            // [1:0]
        uint32_t sesenddet : 1;        // [2], write clear
        uint32_t __7_3 : 5;            // [7:3]
        uint32_t sesreqsucstschng : 1; // [8], write clear
        uint32_t hstnegsucstschng : 1; // [9], write clear
        uint32_t __16_10 : 7;          // [16:10]
        uint32_t hstnegdet : 1;        // [17], write clear
        uint32_t adevtoutchg : 1;      // [18], write clear
        uint32_t dbncedone : 1;        // [19], write clear
        uint32_t __31_20 : 12;         // [31:20]
    } b;
} REG_USBC_GOTGINT_T;

// gahbcfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t glblintrmsk : 1;      // [0]
        uint32_t hbstlen : 4;          // [4:1]
        uint32_t dmaen : 1;            // [5]
        uint32_t __6_6 : 1;            // [6]
        uint32_t nptxfemplvl : 1;      // [7]
        uint32_t ptxfemplvl : 1;       // [8]
        uint32_t __20_9 : 12;          // [20:9]
        uint32_t remmemsupp : 1;       // [21]
        uint32_t notialldmawrit : 1;   // [22]
        uint32_t ahbsingle : 1;        // [23]
        uint32_t invdescendianess : 1; // [24]
        uint32_t __31_25 : 7;          // [31:25]
    } b;
} REG_USBC_GAHBCFG_T;

// gusbcfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t toutcal : 3;        // [2:0]
        uint32_t phyif : 1;          // [3], read only
        uint32_t ulpi_utmi_sel : 1;  // [4]
        uint32_t fsintf : 1;         // [5]
        uint32_t physel : 1;         // [6]
        uint32_t __7_7 : 1;          // [7]
        uint32_t srpcap : 1;         // [8]
        uint32_t hnpcap : 1;         // [9]
        uint32_t usbtrdtim : 4;      // [13:10]
        uint32_t __14_14 : 1;        // [14]
        uint32_t phylpwrclksel : 1;  // [15]
        uint32_t __21_16 : 6;        // [21:16]
        uint32_t termseldlpulse : 1; // [22]
        uint32_t __25_23 : 3;        // [25:23]
        uint32_t ic_usbcap : 1;      // [26], read only
        uint32_t __27_27 : 1;        // [27]
        uint32_t txenddelay : 1;     // [28]
        uint32_t forcehstmode : 1;   // [29]
        uint32_t forcedevmode : 1;   // [30]
        uint32_t corrupttxpkt : 1;   // [31]
    } b;
} REG_USBC_GUSBCFG_T;

// grstctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t csftrst : 1;     // [0]
        uint32_t piufssftrst : 1; // [1]
        uint32_t frmcntrrst : 1;  // [2]
        uint32_t __3_3 : 1;       // [3]
        uint32_t rxfflsh : 1;     // [4]
        uint32_t txfflsh : 1;     // [5]
        uint32_t txfnum : 5;      // [10:6]
        uint32_t __29_11 : 19;    // [29:11]
        uint32_t dmareq : 1;      // [30], read only
        uint32_t ahbidle : 1;     // [31], read only
    } b;
} REG_USBC_GRSTCTL_T;

// gintsts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t curmod : 1;       // [0], read only
        uint32_t modemis : 1;      // [1], write clear
        uint32_t otgint : 1;       // [2]
        uint32_t sof : 1;          // [3]
        uint32_t rxflvl : 1;       // [4], read only
        uint32_t nptxfemp : 1;     // [5], read only
        uint32_t ginnakeff : 1;    // [6], read only
        uint32_t goutnakeff : 1;   // [7], read only
        uint32_t __9_8 : 2;        // [9:8]
        uint32_t erlysusp : 1;     // [10], write clear
        uint32_t usbsusp : 1;      // [11], write clear
        uint32_t usbrst : 1;       // [12], write clear
        uint32_t enumdone : 1;     // [13], write clear
        uint32_t isooutdrop : 1;   // [14], write clear
        uint32_t eopf : 1;         // [15], write clear
        uint32_t __16_16 : 1;      // [16]
        uint32_t epmis : 1;        // [17], write clear
        uint32_t iepint : 1;       // [18], read only
        uint32_t oepint : 1;       // [19], read only
        uint32_t incompisoin : 1;  // [20], write clear
        uint32_t incomplp : 1;     // [21], write clear
        uint32_t fetsusp : 1;      // [22], write clear
        uint32_t resetdet : 1;     // [23], write clear
        uint32_t prtint : 1;       // [24], read only
        uint32_t hchint : 1;       // [25], read only
        uint32_t ptxfemp : 1;      // [26]
        uint32_t __27_27 : 1;      // [27]
        uint32_t conidstschng : 1; // [28]
        uint32_t disconnint : 1;   // [29], write clear
        uint32_t sessreqint : 1;   // [30], write clear
        uint32_t wkupint : 1;      // [31], write clear
    } b;
} REG_USBC_GINTSTS_T;

// gintmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __0_0 : 1;           // [0]
        uint32_t modemismsk : 1;      // [1]
        uint32_t otgintmsk : 1;       // [2]
        uint32_t sofmsk : 1;          // [3]
        uint32_t rxflvlmsk : 1;       // [4]
        uint32_t nptxfempmsk : 1;     // [5]
        uint32_t ginnakeffmsk : 1;    // [6]
        uint32_t goutnakeffmsk : 1;   // [7]
        uint32_t __9_8 : 2;           // [9:8]
        uint32_t erlysuspmsk : 1;     // [10]
        uint32_t usbsuspmsk : 1;      // [11]
        uint32_t usbrstmsk : 1;       // [12]
        uint32_t enumdonemsk : 1;     // [13]
        uint32_t isooutdropmsk : 1;   // [14]
        uint32_t eopfmsk : 1;         // [15]
        uint32_t __16_16 : 1;         // [16]
        uint32_t epmismsk : 1;        // [17]
        uint32_t iepintmsk : 1;       // [18]
        uint32_t oepintmsk : 1;       // [19]
        uint32_t __20_20 : 1;         // [20]
        uint32_t incomplpmsk : 1;     // [21]
        uint32_t fetsuspmsk : 1;      // [22]
        uint32_t resetdetmsk : 1;     // [23]
        uint32_t prtintmsk : 1;       // [24]
        uint32_t hchintmsk : 1;       // [25]
        uint32_t ptxfempmsk : 1;      // [26]
        uint32_t __27_27 : 1;         // [27]
        uint32_t conidstschngmsk : 1; // [28]
        uint32_t disconnintmsk : 1;   // [29]
        uint32_t sessreqintmsk : 1;   // [30]
        uint32_t wkupintmsk : 1;      // [31]
    } b;
} REG_USBC_GINTMSK_T;

// grxstsr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chnum : 4;   // [3:0]
        uint32_t bcnt : 11;   // [14:4], read only
        uint32_t dpid : 2;    // [16:15], read only
        uint32_t pktsts : 4;  // [20:17]
        uint32_t fn : 4;      // [24:21], read only
        uint32_t __31_25 : 7; // [31:25]
    } b;
} REG_USBC_GRXSTSR_T;

// grxstsp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chnum : 4;   // [3:0]
        uint32_t bcnt : 11;   // [14:4], read only
        uint32_t dpid : 2;    // [16:15], read only
        uint32_t pktsts : 4;  // [20:17]
        uint32_t fn : 4;      // [24:21], read only
        uint32_t __31_25 : 7; // [31:25]
    } b;
} REG_USBC_GRXSTSP_T;

// grxfsiz
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rxfdep : 12;  // [11:0]
        uint32_t __31_12 : 20; // [31:12]
    } b;
} REG_USBC_GRXFSIZ_T;

// gnptxfsiz
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nptxfstaddr : 12; // [11:0]
        uint32_t __15_12 : 4;      // [15:12]
        uint32_t nptxfdep : 12;    // [27:16]
        uint32_t __31_28 : 4;      // [31:28]
    } b;
} REG_USBC_GNPTXFSIZ_T;

// gnptxsts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nptxfspcavail : 16; // [15:0], read only
        uint32_t nptxqspcavail : 8;  // [23:16], read only
        uint32_t nptxqtop : 7;       // [30:24], read only
        uint32_t __31_31 : 1;        // [31]
    } b;
} REG_USBC_GNPTXSTS_T;

// ggpio
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpi : 16; // [15:0], read only
        uint32_t gpo : 16; // [31:16]
    } b;
} REG_USBC_GGPIO_T;

// ghwcfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t otgmode : 3;         // [2:0], read only
        uint32_t otgarch : 2;         // [4:3], read only
        uint32_t singpnt : 1;         // [5], read only
        uint32_t hsphytype : 2;       // [7:6], read only
        uint32_t fsphytype : 2;       // [9:8], read only
        uint32_t numdeveps : 4;       // [13:10], read only
        uint32_t numhstchnl : 4;      // [17:14], read only
        uint32_t periosupport : 1;    // [18], read only
        uint32_t dynfifosizing : 1;   // [19], read only
        uint32_t multiprocintrpt : 1; // [20], read only
        uint32_t __21_21 : 1;         // [21]
        uint32_t nptxqdepth : 2;      // [23:22], read only
        uint32_t ptxqdepth : 2;       // [25:24], read only
        uint32_t tknqdepth : 5;       // [30:26], read only
        uint32_t __31_31 : 1;         // [31]
    } b;
} REG_USBC_GHWCFG2_T;

// ghwcfg3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersizewidth : 4; // [3:0], read only
        uint32_t pktsizewidth : 3;  // [6:4], read only
        uint32_t otgen : 1;         // [7], read only
        uint32_t i2cintsel : 1;     // [8], read only
        uint32_t vndctlsupt : 1;    // [9], read only
        uint32_t optfeature : 1;    // [10], read only
        uint32_t rsttype : 1;       // [11], read only
        uint32_t adpsupport : 1;    // [12], read only
        uint32_t hsicmode : 1;      // [13], read only
        uint32_t bcsupport : 1;     // [14], read only
        uint32_t lpmmode : 1;       // [15], read only
        uint32_t dfifodepth : 16;   // [31:16], read only
    } b;
} REG_USBC_GHWCFG3_T;

// ghwcfg4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t numdevperioeps : 4;      // [3:0], read only
        uint32_t partialpwrdn : 1;        // [4], read only
        uint32_t ahbfreq : 1;             // [5], read only
        uint32_t hibernation : 1;         // [6], read only
        uint32_t extendedhibernation : 1; // [7], read only
        uint32_t __11_8 : 4;              // [11:8]
        uint32_t acgsupt : 1;             // [12], read only
        uint32_t enhancedlpmsupt : 1;     // [13], read only
        uint32_t phydatawidth : 2;        // [15:14], read only
        uint32_t numctleps : 4;           // [19:16], read only
        uint32_t iddgfltr : 1;            // [20], read only
        uint32_t vbusvalidfltr : 1;       // [21], read only
        uint32_t avalidfltr : 1;          // [22], read only
        uint32_t bvalidfltr : 1;          // [23], read only
        uint32_t sessendfltr : 1;         // [24], read only
        uint32_t dedfifomode : 1;         // [25], read only
        uint32_t ineps : 4;               // [29:26], read only
        uint32_t descdmaenabled : 1;      // [30], read only
        uint32_t descdma : 1;             // [31], read only
    } b;
} REG_USBC_GHWCFG4_T;

// gpwrdn
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pmuintsel : 1;     // [0]
        uint32_t pmuactv : 1;       // [1]
        uint32_t __2_2 : 1;         // [2]
        uint32_t pwrdnclmp : 1;     // [3]
        uint32_t pwrdnrst_n : 1;    // [4]
        uint32_t pwrdnswtch : 1;    // [5]
        uint32_t disablevbus : 1;   // [6]
        uint32_t __14_7 : 8;        // [14:7]
        uint32_t srpdetect : 1;     // [15], write clear
        uint32_t srpdetectmsk : 1;  // [16]
        uint32_t stschngint : 1;    // [17], write clear
        uint32_t stschngintmsk : 1; // [18]
        uint32_t linestate : 2;     // [20:19], read only
        uint32_t iddig : 1;         // [21], read only
        uint32_t bsessvld : 1;      // [22], read only
        uint32_t adpint : 1;        // [23], write clear
        uint32_t __31_24 : 8;       // [31:24]
    } b;
} REG_USBC_GPWRDN_T;

// gdfifocfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gdfifocfg : 16;      // [15:0]
        uint32_t epinfobaseaddr : 16; // [31:16]
    } b;
} REG_USBC_GDFIFOCFG_T;

// gadpctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prbdschg : 2;     // [1:0]
        uint32_t prbdelta : 2;     // [3:2]
        uint32_t prbper : 2;       // [5:4]
        uint32_t rtim : 11;        // [16:6], read only
        uint32_t enaprb : 1;       // [17]
        uint32_t enasns : 1;       // [18]
        uint32_t adpres : 1;       // [19]
        uint32_t adpen : 1;        // [20]
        uint32_t adpprbint : 1;    // [21], write clear
        uint32_t adpsnsint : 1;    // [22], write clear
        uint32_t adptoutint : 1;   // [23], write clear
        uint32_t adpprbintmsk : 1; // [24]
        uint32_t adpsnsintmsk : 1; // [25]
        uint32_t adptoutmsk : 1;   // [26]
        uint32_t ar : 2;           // [28:27]
        uint32_t __31_29 : 3;      // [31:29]
    } b;
} REG_USBC_GADPCTL_T;

// hptxfsiz
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ptxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;     // [15:13]
        uint32_t ptxfsize : 12;   // [27:16]
        uint32_t __31_28 : 4;     // [31:28]
    } b;
} REG_USBC_HPTXFSIZ_T;

// dieptxf1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 12; // [11:0]
        uint32_t __15_12 : 4;         // [15:12]
        uint32_t inepntxfdep : 10;    // [25:16]
        uint32_t __31_26 : 6;         // [31:26]
    } b;
} REG_USBC_DIEPTXF1_T;

// dieptxf2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 10;    // [25:16]
        uint32_t __31_26 : 6;         // [31:26]
    } b;
} REG_USBC_DIEPTXF2_T;

// dieptxf3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF3_T;

// dieptxf4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF4_T;

// dieptxf5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF5_T;

// dieptxf6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF6_T;

// dieptxf7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF7_T;

// dieptxf8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepntxfstaddr : 13; // [12:0]
        uint32_t __15_13 : 3;         // [15:13]
        uint32_t inepntxfdep : 9;     // [24:16]
        uint32_t __31_25 : 7;         // [31:25]
    } b;
} REG_USBC_DIEPTXF8_T;

// hcfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t fslspclksel : 2; // [1:0]
        uint32_t fslssupp : 1;    // [2]
        uint32_t __6_3 : 4;       // [6:3]
        uint32_t ena32khzs : 1;   // [7]
        uint32_t resvalid : 8;    // [15:8]
        uint32_t __22_16 : 7;     // [22:16]
        uint32_t descdma : 1;     // [23]
        uint32_t frlisten : 2;    // [25:24]
        uint32_t perschedena : 1; // [26]
        uint32_t __30_27 : 4;     // [30:27]
        uint32_t modechtimen : 1; // [31]
    } b;
} REG_USBC_HCFG_T;

// hfir
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frint : 16;      // [15:0]
        uint32_t hfirrldctrl : 1; // [16]
        uint32_t __31_17 : 15;    // [31:17]
    } b;
} REG_USBC_HFIR_T;

// hfnum
typedef union {
    uint32_t v;
    struct
    {
        uint32_t frnum : 16; // [15:0], read only
        uint32_t frrem : 16; // [31:16], read only
    } b;
} REG_USBC_HFNUM_T;

// hptxsts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ptxfspcavail : 16; // [15:0], read only
        uint32_t ptxqspcavail : 8;  // [23:16], read only
        uint32_t ptxqtop : 8;       // [31:24], read only
    } b;
} REG_USBC_HPTXSTS_T;

// haint
typedef union {
    uint32_t v;
    struct
    {
        uint32_t haint : 16;   // [15:0], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_USBC_HAINT_T;

// haintmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t haintmsk : 16; // [15:0]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_USBC_HAINTMSK_T;

// hprt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtconnsts : 1;     // [0], read only
        uint32_t prtconndet : 1;     // [1], write clear
        uint32_t prtena : 1;         // [2], write clear
        uint32_t prtenchng : 1;      // [3], write clear
        uint32_t prtovrcurract : 1;  // [4], read only
        uint32_t prtovrcurrchng : 1; // [5], write clear
        uint32_t prtres : 1;         // [6]
        uint32_t prtsusp : 1;        // [7]
        uint32_t prtrst : 1;         // [8]
        uint32_t __9_9 : 1;          // [9]
        uint32_t prtlnsts : 2;       // [11:10], read only
        uint32_t prtpwr : 1;         // [12]
        uint32_t prttstctl : 4;      // [16:13]
        uint32_t prtspd : 2;         // [18:17], read only
        uint32_t __31_19 : 13;       // [31:19]
    } b;
} REG_USBC_HPRT_T;

// hcchar0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR0_T;

// hcsplt0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT0_T;

// hcint0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT0_T;

// hcintmsk0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK0_T;

// hctsiz0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ0_T;

// hcchar1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR1_T;

// hcsplt1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT1_T;

// hcint1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT1_T;

// hcintmsk1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK1_T;

// hctsiz1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ1_T;

// hcchar2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR2_T;

// hcsplt2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT2_T;

// hcint2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT2_T;

// hcintmsk2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK2_T;

// hctsiz2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ2_T;

// hcchar3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR3_T;

// hcsplt3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT3_T;

// hcint3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT3_T;

// hcintmsk3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK3_T;

// hctsiz3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ3_T;

// hcchar4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR4_T;

// hcsplt4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT4_T;

// hcint4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT4_T;

// hcintmsk4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK4_T;

// hctsiz4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ4_T;

// hcchar5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR5_T;

// hcsplt5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT5_T;

// hcint5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT5_T;

// hcintmsk5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK5_T;

// hctsiz5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ5_T;

// hcchar6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR6_T;

// hcsplt6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT6_T;

// hcint6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT6_T;

// hcintmsk6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK6_T;

// hctsiz6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ6_T;

// hcchar7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR7_T;

// hcsplt7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT7_T;

// hcint7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT7_T;

// hcintmsk7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK7_T;

// hctsiz7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ7_T;

// hcchar8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR8_T;

// hcsplt8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT8_T;

// hcint8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT8_T;

// hcintmsk8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK8_T;

// hctsiz8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ8_T;

// hcchar9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR9_T;

// hcsplt9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT9_T;

// hcint9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT9_T;

// hcintmsk9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK9_T;

// hctsiz9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ9_T;

// hcchar10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR10_T;

// hcsplt10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT10_T;

// hcint10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT10_T;

// hcintmsk10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK10_T;

// hctsiz10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ10_T;

// hcchar11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR11_T;

// hcsplt11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT11_T;

// hcint11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT11_T;

// hcintmsk11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK11_T;

// hctsiz11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ11_T;

// hcchar12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR12_T;

// hcsplt12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT12_T;

// hcint12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT12_T;

// hcintmsk12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK12_T;

// hctsiz12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ12_T;

// hcchar13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR13_T;

// hcsplt13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT13_T;

// hcint13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT13_T;

// hcintmsk13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK13_T;

// hctsiz13
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ13_T;

// hcchar14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR14_T;

// hcsplt14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT14_T;

// hcint14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT14_T;

// hcintmsk14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK14_T;

// hctsiz14
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ14_T;

// hcchar15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;    // [10:0]
        uint32_t epnum : 4;   // [14:11]
        uint32_t epdir : 1;   // [15]
        uint32_t __16_16 : 1; // [16]
        uint32_t lspddev : 1; // [17]
        uint32_t eptype : 2;  // [19:18]
        uint32_t ec : 2;      // [21:20]
        uint32_t devaddr : 7; // [28:22]
        uint32_t oddfrm : 1;  // [29]
        uint32_t chdis : 1;   // [30]
        uint32_t chena : 1;   // [31]
    } b;
} REG_USBC_HCCHAR15_T;

// hcsplt15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t prtaddr : 7;  // [6:0]
        uint32_t hubaddr : 7;  // [13:7]
        uint32_t xactpos : 2;  // [15:14]
        uint32_t compsplt : 1; // [16]
        uint32_t __30_17 : 14; // [30:17]
        uint32_t spltena : 1;  // [31]
    } b;
} REG_USBC_HCSPLT15_T;

// hcint15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;         // [0], write clear
        uint32_t chhltd : 1;            // [1], write clear
        uint32_t ahberr : 1;            // [2], write clear
        uint32_t stall : 1;             // [3], write clear
        uint32_t nak : 1;               // [4], write clear
        uint32_t ack : 1;               // [5], write clear
        uint32_t nyet : 1;              // [6], write clear
        uint32_t xacterr : 1;           // [7], write clear
        uint32_t bblerr : 1;            // [8], write clear
        uint32_t frmovrun : 1;          // [9], write clear
        uint32_t datatglerr : 1;        // [10], write clear
        uint32_t bnaintr : 1;           // [11], write clear
        uint32_t xcs_xact_err : 1;      // [12], write clear
        uint32_t desc_lst_rollintr : 1; // [13], write clear
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_USBC_HCINT15_T;

// hcintmsk15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;         // [0]
        uint32_t chhltdmsk : 1;            // [1]
        uint32_t ahberrmsk : 1;            // [2]
        uint32_t __10_3 : 8;               // [10:3]
        uint32_t bnaintrmsk : 1;           // [11]
        uint32_t __12_12 : 1;              // [12]
        uint32_t desc_lst_rollintrmsk : 1; // [13]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_USBC_HCINTMSK15_T;

// hctsiz15
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t pid : 2;       // [30:29]
        uint32_t dopng : 1;     // [31]
    } b;
} REG_USBC_HCTSIZ15_T;

// dcfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t devspd : 2;        // [1:0]
        uint32_t nzstsouthshk : 1;  // [2]
        uint32_t ena32khzsusp : 1;  // [3]
        uint32_t devaddr : 7;       // [10:4]
        uint32_t perfrint : 2;      // [12:11]
        uint32_t endevoutnak : 1;   // [13]
        uint32_t xcvrdly : 1;       // [14]
        uint32_t erraticintmsk : 1; // [15]
        uint32_t __22_16 : 7;       // [22:16]
        uint32_t descdma : 1;       // [23]
        uint32_t perschintvl : 2;   // [25:24]
        uint32_t resvalid : 6;      // [31:26]
    } b;
} REG_USBC_DCFG_T;

// dctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rmtwkupsig : 1;   // [0]
        uint32_t sftdiscon : 1;    // [1]
        uint32_t gnpinnaksts : 1;  // [2], read only
        uint32_t goutnaksts : 1;   // [3], read only
        uint32_t tstctl : 3;       // [6:4]
        uint32_t sgnpinnak : 1;    // [7]
        uint32_t cgnpinnak : 1;    // [8]
        uint32_t sgoutnak : 1;     // [9]
        uint32_t cgoutnak : 1;     // [10]
        uint32_t pwronprgdone : 1; // [11]
        uint32_t __12_12 : 1;      // [12]
        uint32_t gmc : 2;          // [14:13]
        uint32_t ignrfrmnum : 1;   // [15]
        uint32_t nakonbble : 1;    // [16]
        uint32_t encontonbna : 1;  // [17]
        uint32_t __31_18 : 14;     // [31:18]
    } b;
} REG_USBC_DCTL_T;

// dsts
typedef union {
    uint32_t v;
    struct
    {
        uint32_t suspsts : 1;   // [0], read only
        uint32_t enumspd : 2;   // [2:1], read only
        uint32_t errticerr : 1; // [3], read only
        uint32_t __7_4 : 4;     // [7:4]
        uint32_t soffn : 14;    // [21:8]
        uint32_t devlnsts : 2;  // [23:22], read only
        uint32_t __31_24 : 8;   // [31:24]
    } b;
} REG_USBC_DSTS_T;

// diepmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;   // [0]
        uint32_t epdisbldmsk : 1;    // [1]
        uint32_t ahberrmsk : 1;      // [2]
        uint32_t timeoutmsk : 1;     // [3]
        uint32_t intkntxfempmsk : 1; // [4]
        uint32_t intknepmismsk : 1;  // [5]
        uint32_t inepnakeffmsk : 1;  // [6]
        uint32_t __7_7 : 1;          // [7]
        uint32_t txfifoundrnmsk : 1; // [8]
        uint32_t bnainintrmsk : 1;   // [9]
        uint32_t __12_10 : 3;        // [12:10]
        uint32_t nakmsk : 1;         // [13]
        uint32_t __31_14 : 18;       // [31:14]
    } b;
} REG_USBC_DIEPMSK_T;

// doepmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercomplmsk : 1;   // [0]
        uint32_t epdisbldmsk : 1;    // [1]
        uint32_t ahberrmsk : 1;      // [2]
        uint32_t setupmsk : 1;       // [3]
        uint32_t outtknepdismsk : 1; // [4]
        uint32_t stsphsercvdmsk : 1; // [5]
        uint32_t back2backsetup : 1; // [6]
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterrmsk : 1;   // [8]
        uint32_t bnaoutintrmsk : 1;  // [9]
        uint32_t __11_10 : 2;        // [11:10]
        uint32_t bbleerrmsk : 1;     // [12]
        uint32_t nakmsk : 1;         // [13]
        uint32_t nyetmsk : 1;        // [14]
        uint32_t __31_15 : 17;       // [31:15]
    } b;
} REG_USBC_DOEPMSK_T;

// daint
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepint0 : 1;   // [0], read only
        uint32_t inepint1 : 1;   // [1], read only
        uint32_t inepint2 : 1;   // [2], read only
        uint32_t inepint3 : 1;   // [3], read only
        uint32_t inepint4 : 1;   // [4], read only
        uint32_t inepint5 : 1;   // [5], read only
        uint32_t inepint6 : 1;   // [6], read only
        uint32_t inepint7 : 1;   // [7], read only
        uint32_t inepint8 : 1;   // [8], read only
        uint32_t inepint9 : 1;   // [9], read only
        uint32_t inepint10 : 1;  // [10], read only
        uint32_t inepint11 : 1;  // [11], read only
        uint32_t inepint12 : 1;  // [12], read only
        uint32_t __15_13 : 3;    // [15:13]
        uint32_t outepint0 : 1;  // [16], read only
        uint32_t outepint1 : 1;  // [17], read only
        uint32_t outepint2 : 1;  // [18], read only
        uint32_t outepint3 : 1;  // [19], read only
        uint32_t outepint4 : 1;  // [20], read only
        uint32_t outepint5 : 1;  // [21], read only
        uint32_t outepint6 : 1;  // [22], read only
        uint32_t outepint7 : 1;  // [23], read only
        uint32_t outepint8 : 1;  // [24], read only
        uint32_t outepint9 : 1;  // [25], read only
        uint32_t outepint10 : 1; // [26], read only
        uint32_t outepint11 : 1; // [27], read only
        uint32_t outepint12 : 1; // [28], read only
        uint32_t __31_29 : 3;    // [31:29]
    } b;
} REG_USBC_DAINT_T;

// daintmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t inepmsk0 : 1;   // [0]
        uint32_t inepmsk1 : 1;   // [1]
        uint32_t inepmsk2 : 1;   // [2]
        uint32_t inepmsk3 : 1;   // [3]
        uint32_t inepmsk4 : 1;   // [4]
        uint32_t inepmsk5 : 1;   // [5]
        uint32_t inepmsk6 : 1;   // [6]
        uint32_t inepmsk7 : 1;   // [7]
        uint32_t inepmsk8 : 1;   // [8]
        uint32_t inepmsk9 : 1;   // [9]
        uint32_t inepmsk10 : 1;  // [10]
        uint32_t inepmsk11 : 1;  // [11]
        uint32_t inepmsk12 : 1;  // [12]
        uint32_t __15_13 : 3;    // [15:13]
        uint32_t outepmsk0 : 1;  // [16]
        uint32_t outepmsk1 : 1;  // [17]
        uint32_t outepmsk2 : 1;  // [18]
        uint32_t outepmsk3 : 1;  // [19]
        uint32_t outepmsk4 : 1;  // [20]
        uint32_t outepmsk5 : 1;  // [21]
        uint32_t outepmsk6 : 1;  // [22]
        uint32_t outepmsk7 : 1;  // [23]
        uint32_t outepmsk8 : 1;  // [24]
        uint32_t outepmsk9 : 1;  // [25]
        uint32_t outepmsk10 : 1; // [26]
        uint32_t outepmsk11 : 1; // [27]
        uint32_t outepmsk12 : 1; // [28]
        uint32_t __31_29 : 3;    // [31:29]
    } b;
} REG_USBC_DAINTMSK_T;

// dvbusdis
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dvbusdis : 16; // [15:0]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_USBC_DVBUSDIS_T;

// dvbuspulse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dvbuspulse : 12; // [11:0]
        uint32_t __31_12 : 20;    // [31:12]
    } b;
} REG_USBC_DVBUSPULSE_T;

// dthrctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nonisothren : 1; // [0]
        uint32_t isothren : 1;    // [1]
        uint32_t txthrlen : 9;    // [10:2]
        uint32_t ahbthrratio : 2; // [12:11]
        uint32_t __15_13 : 3;     // [15:13]
        uint32_t rxthren : 1;     // [16]
        uint32_t rxthrlen : 9;    // [25:17]
        uint32_t __26_26 : 1;     // [26]
        uint32_t arbprken : 1;    // [27]
        uint32_t __31_28 : 4;     // [31:28]
    } b;
} REG_USBC_DTHRCTL_T;

// diepempmsk
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfempmsk : 16; // [15:0]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DIEPEMPMSK_T;

// diepctl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 2;      // [1:0]
        uint32_t __14_2 : 13;  // [14:2]
        uint32_t usbactep : 1; // [15], read only
        uint32_t __16_16 : 1;  // [16]
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18], read only
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t __29_28 : 2;  // [29:28]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL0_T;

// diepint0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT0_T;

// dieptsiz0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 7; // [6:0]
        uint32_t __18_7 : 12;  // [18:7]
        uint32_t pktcnt : 2;   // [20:19]
        uint32_t __31_21 : 11; // [31:21]
    } b;
} REG_USBC_DIEPTSIZ0_T;

// dtxfsts0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS0_T;

// diepctl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL1_T;

// diepint1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT1_T;

// dieptsiz1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ1_T;

// dtxfsts1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS1_T;

// diepctl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL2_T;

// diepint2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT2_T;

// dieptsiz2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ2_T;

// dtxfsts2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS2_T;

// diepctl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL3_T;

// diepint3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT3_T;

// dieptsiz3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ3_T;

// dtxfsts3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS3_T;

// diepctl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL4_T;

// diepint4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT4_T;

// dieptsiz4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ4_T;

// dtxfsts4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS4_T;

// diepctl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL5_T;

// diepint5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT5_T;

// dieptsiz5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ5_T;

// dtxfsts5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS5_T;

// diepctl6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL6_T;

// diepint6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT6_T;

// dieptsiz6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ6_T;

// dtxfsts6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS6_T;

// diepctl7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL7_T;

// diepint7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT7_T;

// dieptsiz7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ7_T;

// dtxfsts7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS7_T;

// diepctl8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL8_T;

// diepint8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT8_T;

// dieptsiz8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ8_T;

// dtxfsts8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS8_T;

// diepctl9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL9_T;

// diepint9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT9_T;

// dieptsiz9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ9_T;

// dtxfsts9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS9_T;

// diepctl10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL10_T;

// diepint10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT10_T;

// dieptsiz10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ10_T;

// dtxfsts10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS10_T;

// diepctl11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL11_T;

// diepint11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT11_T;

// dieptsiz11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ11_T;

// dtxfsts11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS11_T;

// diepctl12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t __20_20 : 1;  // [20]
        uint32_t stall : 1;    // [21]
        uint32_t txfnum : 4;   // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DIEPCTL12_T;

// diepint12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;   // [0], write clear
        uint32_t epdisbld : 1;    // [1], write clear
        uint32_t ahberr : 1;      // [2], write clear
        uint32_t timeout : 1;     // [3], write clear
        uint32_t intkntxfemp : 1; // [4], write clear
        uint32_t intknepmis : 1;  // [5], write clear
        uint32_t inepnakeff : 1;  // [6], write clear
        uint32_t txfemp : 1;      // [7], read only
        uint32_t txfifoundrn : 1; // [8], write clear
        uint32_t bnaintr : 1;     // [9], write clear
        uint32_t __10_10 : 1;     // [10]
        uint32_t pktdrpsts : 1;   // [11], write clear
        uint32_t bbleerr : 1;     // [12], write clear
        uint32_t nakintrpt : 1;   // [13], write clear
        uint32_t nyetintrpt : 1;  // [14], write clear
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_USBC_DIEPINT12_T;

// dieptsiz12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t mc : 2;        // [30:29]
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DIEPTSIZ12_T;

// dtxfsts12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ineptxfspcavail : 16; // [15:0], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_USBC_DTXFSTS12_T;

// doepctl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 2;      // [1:0], read only
        uint32_t __14_2 : 13;  // [14:2]
        uint32_t usbactep : 1; // [15], read only
        uint32_t __16_16 : 1;  // [16]
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18], read only
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t __29_28 : 2;  // [29:28]
        uint32_t epdis : 1;    // [30], read only
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL0_T;

// doepint0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT0_T;

// doeptsiz0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 7; // [6:0]
        uint32_t __18_7 : 12;  // [18:7]
        uint32_t pktcnt : 1;   // [19]
        uint32_t __28_20 : 9;  // [28:20]
        uint32_t supcnt : 2;   // [30:29]
        uint32_t __31_31 : 1;  // [31]
    } b;
} REG_USBC_DOEPTSIZ0_T;

// doepctl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL1_T;

// doepint1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT1_T;

// doeptsiz1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ1_T;

// doepctl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL2_T;

// doepint2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT2_T;

// doeptsiz2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ2_T;

// doepctl3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL3_T;

// doepint3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT3_T;

// doeptsiz3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ3_T;

// doepctl4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL4_T;

// doepint4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT4_T;

// doeptsiz4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ4_T;

// doepctl5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL5_T;

// doepint5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT5_T;

// doeptsiz5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ5_T;

// doepctl6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL6_T;

// doepint6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT6_T;

// doeptsiz6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ6_T;

// doepctl7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL7_T;

// doepint7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT7_T;

// doeptsiz7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ7_T;

// doepctl8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL8_T;

// doepint8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT8_T;

// doeptsiz8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ8_T;

// doepctl9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL9_T;

// doepint9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT9_T;

// doeptsiz9
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ9_T;

// doepctl10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL10_T;

// doepint10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT10_T;

// doeptsiz10
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ10_T;

// doepctl11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL11_T;

// doepint11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT11_T;

// doeptsiz11
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ11_T;

// doepctl12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mps : 11;     // [10:0]
        uint32_t __14_11 : 4;  // [14:11]
        uint32_t usbactep : 1; // [15]
        uint32_t dpid : 1;     // [16], read only
        uint32_t naksts : 1;   // [17], read only
        uint32_t eptype : 2;   // [19:18]
        uint32_t snp : 1;      // [20]
        uint32_t stall : 1;    // [21]
        uint32_t __25_22 : 4;  // [25:22]
        uint32_t cnak : 1;     // [26]
        uint32_t snak : 1;     // [27]
        uint32_t setd0pid : 1; // [28]
        uint32_t setd1pid : 1; // [29]
        uint32_t epdis : 1;    // [30]
        uint32_t epena : 1;    // [31]
    } b;
} REG_USBC_DOEPCTL12_T;

// doepint12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfercompl : 1;      // [0], write clear
        uint32_t epdisbld : 1;       // [1], write clear
        uint32_t ahberr : 1;         // [2], write clear
        uint32_t setup : 1;          // [3], write clear
        uint32_t outtknepdis : 1;    // [4], write clear
        uint32_t stsphsercvd : 1;    // [5], write clear
        uint32_t back2backsetup : 1; // [6], write clear
        uint32_t __7_7 : 1;          // [7]
        uint32_t outpkterr : 1;      // [8], write clear
        uint32_t bnaintr : 1;        // [9], write clear
        uint32_t __10_10 : 1;        // [10]
        uint32_t pktdrpsts : 1;      // [11], write clear
        uint32_t bbleerr : 1;        // [12], write clear
        uint32_t nakintrpt : 1;      // [13], write clear
        uint32_t nyetintrpt : 1;     // [14], write clear
        uint32_t stuppktrcvd : 1;    // [15], write clear
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_USBC_DOEPINT12_T;

// doeptsiz12
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xfersize : 19; // [18:0]
        uint32_t pktcnt : 10;   // [28:19]
        uint32_t rxdpid : 2;    // [30:29], read only
        uint32_t __31_31 : 1;   // [31]
    } b;
} REG_USBC_DOEPTSIZ12_T;

// pcgcctl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t stoppclk : 1;      // [0]
        uint32_t __2_1 : 2;         // [2:1]
        uint32_t rstpdwnmodule : 1; // [3]
        uint32_t __5_4 : 2;         // [5:4]
        uint32_t physleep : 1;      // [6], read only
        uint32_t l1suspended : 1;   // [7], read only
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_USBC_PCGCCTL_T;

// gotgctl
#define USBC_SESREQSCS (1 << 0)
#define USBC_SESREQ (1 << 1)
#define USBC_VBVALIDOVEN (1 << 2)
#define USBC_VBVALIDOVVAL (1 << 3)
#define USBC_AVALIDOVEN (1 << 4)
#define USBC_AVALIDOVVAL (1 << 5)
#define USBC_BVALIDOVEN (1 << 6)
#define USBC_BVALIDOVVAL (1 << 7)
#define USBC_HSTNEGSCS (1 << 8)
#define USBC_HNPREQ (1 << 9)
#define USBC_HSTSETHNPEN (1 << 10)
#define USBC_DEVHNPEN (1 << 11)
#define USBC_EHEN (1 << 12)
#define USBC_DBNCEFLTRBYPASS (1 << 15)
#define USBC_CONIDSTS (1 << 16)
#define USBC_DBNCTIME (1 << 17)
#define USBC_ASESVLD (1 << 18)
#define USBC_BSESVLD (1 << 19)
#define USBC_OTGVER (1 << 20)
#define USBC_GOTGCTL_CURMOD (1 << 21)

// gotgint
#define USBC_SESENDDET (1 << 2)
#define USBC_SESREQSUCSTSCHNG (1 << 8)
#define USBC_HSTNEGSUCSTSCHNG (1 << 9)
#define USBC_HSTNEGDET (1 << 17)
#define USBC_ADEVTOUTCHG (1 << 18)
#define USBC_DBNCEDONE (1 << 19)

// gahbcfg
#define USBC_GLBLINTRMSK (1 << 0)
#define USBC_HBSTLEN(n) (((n)&0xf) << 1)
#define USBC_DMAEN (1 << 5)
#define USBC_NPTXFEMPLVL (1 << 7)
#define USBC_PTXFEMPLVL (1 << 8)
#define USBC_REMMEMSUPP (1 << 21)
#define USBC_NOTIALLDMAWRIT (1 << 22)
#define USBC_AHBSINGLE (1 << 23)
#define USBC_INVDESCENDIANESS (1 << 24)

// gusbcfg
#define USBC_TOUTCAL(n) (((n)&0x7) << 0)
#define USBC_PHYIF (1 << 3)
#define USBC_ULPI_UTMI_SEL (1 << 4)
#define USBC_FSINTF (1 << 5)
#define USBC_PHYSEL (1 << 6)
#define USBC_SRPCAP (1 << 8)
#define USBC_HNPCAP (1 << 9)
#define USBC_USBTRDTIM(n) (((n)&0xf) << 10)
#define USBC_PHYLPWRCLKSEL (1 << 15)
#define USBC_TERMSELDLPULSE (1 << 22)
#define USBC_IC_USBCAP (1 << 26)
#define USBC_TXENDDELAY (1 << 28)
#define USBC_FORCEHSTMODE (1 << 29)
#define USBC_FORCEDEVMODE (1 << 30)
#define USBC_CORRUPTTXPKT (1 << 31)

// grstctl
#define USBC_CSFTRST (1 << 0)
#define USBC_PIUFSSFTRST (1 << 1)
#define USBC_FRMCNTRRST (1 << 2)
#define USBC_RXFFLSH (1 << 4)
#define USBC_TXFFLSH (1 << 5)
#define USBC_GRSTCTL_TXFNUM(n) (((n)&0x1f) << 6)
#define USBC_DMAREQ (1 << 30)
#define USBC_AHBIDLE (1 << 31)

// gintsts
#define USBC_GINTSTS_CURMOD (1 << 0)
#define USBC_MODEMIS (1 << 1)
#define USBC_OTGINT (1 << 2)
#define USBC_SOF (1 << 3)
#define USBC_RXFLVL (1 << 4)
#define USBC_NPTXFEMP (1 << 5)
#define USBC_GINNAKEFF (1 << 6)
#define USBC_GOUTNAKEFF (1 << 7)
#define USBC_ERLYSUSP (1 << 10)
#define USBC_USBSUSP (1 << 11)
#define USBC_USBRST (1 << 12)
#define USBC_ENUMDONE (1 << 13)
#define USBC_ISOOUTDROP (1 << 14)
#define USBC_EOPF (1 << 15)
#define USBC_EPMIS (1 << 17)
#define USBC_IEPINT (1 << 18)
#define USBC_OEPINT (1 << 19)
#define USBC_INCOMPISOIN (1 << 20)
#define USBC_INCOMPLP (1 << 21)
#define USBC_FETSUSP (1 << 22)
#define USBC_RESETDET (1 << 23)
#define USBC_PRTINT (1 << 24)
#define USBC_HCHINT (1 << 25)
#define USBC_PTXFEMP (1 << 26)
#define USBC_CONIDSTSCHNG (1 << 28)
#define USBC_DISCONNINT (1 << 29)
#define USBC_SESSREQINT (1 << 30)
#define USBC_WKUPINT (1 << 31)

// gintmsk
#define USBC_MODEMISMSK (1 << 1)
#define USBC_OTGINTMSK (1 << 2)
#define USBC_SOFMSK (1 << 3)
#define USBC_RXFLVLMSK (1 << 4)
#define USBC_NPTXFEMPMSK (1 << 5)
#define USBC_GINNAKEFFMSK (1 << 6)
#define USBC_GOUTNAKEFFMSK (1 << 7)
#define USBC_ERLYSUSPMSK (1 << 10)
#define USBC_USBSUSPMSK (1 << 11)
#define USBC_USBRSTMSK (1 << 12)
#define USBC_ENUMDONEMSK (1 << 13)
#define USBC_ISOOUTDROPMSK (1 << 14)
#define USBC_EOPFMSK (1 << 15)
#define USBC_EPMISMSK (1 << 17)
#define USBC_IEPINTMSK (1 << 18)
#define USBC_OEPINTMSK (1 << 19)
#define USBC_INCOMPLPMSK (1 << 21)
#define USBC_FETSUSPMSK (1 << 22)
#define USBC_RESETDETMSK (1 << 23)
#define USBC_PRTINTMSK (1 << 24)
#define USBC_HCHINTMSK (1 << 25)
#define USBC_PTXFEMPMSK (1 << 26)
#define USBC_CONIDSTSCHNGMSK (1 << 28)
#define USBC_DISCONNINTMSK (1 << 29)
#define USBC_SESSREQINTMSK (1 << 30)
#define USBC_WKUPINTMSK (1 << 31)

// grxstsr
#define USBC_CHNUM(n) (((n)&0xf) << 0)
#define USBC_BCNT(n) (((n)&0x7ff) << 4)
#define USBC_GRXSTSR_DPID(n) (((n)&0x3) << 15)
#define USBC_PKTSTS(n) (((n)&0xf) << 17)
#define USBC_FN(n) (((n)&0xf) << 21)

// grxstsp
#define USBC_CHNUM(n) (((n)&0xf) << 0)
#define USBC_BCNT(n) (((n)&0x7ff) << 4)
#define USBC_GRXSTSP_DPID(n) (((n)&0x3) << 15)
#define USBC_PKTSTS(n) (((n)&0xf) << 17)
#define USBC_FN(n) (((n)&0xf) << 21)

// grxfsiz
#define USBC_RXFDEP(n) (((n)&0xfff) << 0)

// gnptxfsiz
#define USBC_NPTXFSTADDR(n) (((n)&0xfff) << 0)
#define USBC_NPTXFDEP(n) (((n)&0xfff) << 16)

// gnptxsts
#define USBC_NPTXFSPCAVAIL(n) (((n)&0xffff) << 0)
#define USBC_NPTXQSPCAVAIL(n) (((n)&0xff) << 16)
#define USBC_NPTXQTOP(n) (((n)&0x7f) << 24)

// ggpio
#define USBC_GPI(n) (((n)&0xffff) << 0)
#define USBC_GPO(n) (((n)&0xffff) << 16)

// guid
#define USBC_GUID(n) (((n)&0xffffffff) << 0)

// gsnpsid
#define USBC_SYNOPSYSID(n) (((n)&0xffffffff) << 0)

// ghwcfg1
#define USBC_GHWCFG1_EPDIR(n) (((n)&0xffffffff) << 0)

// ghwcfg2
#define USBC_OTGMODE(n) (((n)&0x7) << 0)
#define USBC_OTGARCH(n) (((n)&0x3) << 3)
#define USBC_SINGPNT (1 << 5)
#define USBC_HSPHYTYPE(n) (((n)&0x3) << 6)
#define USBC_FSPHYTYPE(n) (((n)&0x3) << 8)
#define USBC_NUMDEVEPS(n) (((n)&0xf) << 10)
#define USBC_NUMHSTCHNL(n) (((n)&0xf) << 14)
#define USBC_PERIOSUPPORT (1 << 18)
#define USBC_DYNFIFOSIZING (1 << 19)
#define USBC_MULTIPROCINTRPT (1 << 20)
#define USBC_NPTXQDEPTH(n) (((n)&0x3) << 22)
#define USBC_PTXQDEPTH(n) (((n)&0x3) << 24)
#define USBC_TKNQDEPTH(n) (((n)&0x1f) << 26)

// ghwcfg3
#define USBC_XFERSIZEWIDTH(n) (((n)&0xf) << 0)
#define USBC_PKTSIZEWIDTH(n) (((n)&0x7) << 4)
#define USBC_OTGEN (1 << 7)
#define USBC_I2CINTSEL (1 << 8)
#define USBC_VNDCTLSUPT (1 << 9)
#define USBC_OPTFEATURE (1 << 10)
#define USBC_RSTTYPE (1 << 11)
#define USBC_ADPSUPPORT (1 << 12)
#define USBC_HSICMODE (1 << 13)
#define USBC_BCSUPPORT (1 << 14)
#define USBC_LPMMODE (1 << 15)
#define USBC_DFIFODEPTH(n) (((n)&0xffff) << 16)

// ghwcfg4
#define USBC_NUMDEVPERIOEPS(n) (((n)&0xf) << 0)
#define USBC_PARTIALPWRDN (1 << 4)
#define USBC_AHBFREQ (1 << 5)
#define USBC_HIBERNATION (1 << 6)
#define USBC_EXTENDEDHIBERNATION (1 << 7)
#define USBC_ACGSUPT (1 << 12)
#define USBC_ENHANCEDLPMSUPT (1 << 13)
#define USBC_PHYDATAWIDTH(n) (((n)&0x3) << 14)
#define USBC_NUMCTLEPS(n) (((n)&0xf) << 16)
#define USBC_IDDGFLTR (1 << 20)
#define USBC_VBUSVALIDFLTR (1 << 21)
#define USBC_AVALIDFLTR (1 << 22)
#define USBC_BVALIDFLTR (1 << 23)
#define USBC_SESSENDFLTR (1 << 24)
#define USBC_DEDFIFOMODE (1 << 25)
#define USBC_INEPS(n) (((n)&0xf) << 26)
#define USBC_DESCDMAENABLED (1 << 30)
#define USBC_GHWCFG4_DESCDMA (1 << 31)

// gpwrdn
#define USBC_PMUINTSEL (1 << 0)
#define USBC_PMUACTV (1 << 1)
#define USBC_PWRDNCLMP (1 << 3)
#define USBC_PWRDNRST_N (1 << 4)
#define USBC_PWRDNSWTCH (1 << 5)
#define USBC_DISABLEVBUS (1 << 6)
#define USBC_SRPDETECT (1 << 15)
#define USBC_SRPDETECTMSK (1 << 16)
#define USBC_STSCHNGINT (1 << 17)
#define USBC_STSCHNGINTMSK (1 << 18)
#define USBC_LINESTATE(n) (((n)&0x3) << 19)
#define USBC_IDDIG (1 << 21)
#define USBC_BSESSVLD (1 << 22)
#define USBC_ADPINT (1 << 23)

// gdfifocfg
#define USBC_GDFIFOCFG(n) (((n)&0xffff) << 0)
#define USBC_EPINFOBASEADDR(n) (((n)&0xffff) << 16)

// gadpctl
#define USBC_PRBDSCHG(n) (((n)&0x3) << 0)
#define USBC_PRBDELTA(n) (((n)&0x3) << 2)
#define USBC_PRBPER(n) (((n)&0x3) << 4)
#define USBC_RTIM(n) (((n)&0x7ff) << 6)
#define USBC_ENAPRB (1 << 17)
#define USBC_ENASNS (1 << 18)
#define USBC_ADPRES (1 << 19)
#define USBC_ADPEN (1 << 20)
#define USBC_ADPPRBINT (1 << 21)
#define USBC_ADPSNSINT (1 << 22)
#define USBC_ADPTOUTINT (1 << 23)
#define USBC_ADPPRBINTMSK (1 << 24)
#define USBC_ADPSNSINTMSK (1 << 25)
#define USBC_ADPTOUTMSK (1 << 26)
#define USBC_AR(n) (((n)&0x3) << 27)

// hptxfsiz
#define USBC_PTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_PTXFSIZE(n) (((n)&0xfff) << 16)

// dieptxf1
#define USBC_DIEPTXF1_INEPNTXFSTADDR(n) (((n)&0xfff) << 0)
#define USBC_DIEPTXF1_INEPNTXFDEP(n) (((n)&0x3ff) << 16)

// dieptxf2
#define USBC_DIEPTXF2_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF2_INEPNTXFDEP(n) (((n)&0x3ff) << 16)

// dieptxf3
#define USBC_DIEPTXF3_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF3_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// dieptxf4
#define USBC_DIEPTXF4_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF4_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// dieptxf5
#define USBC_DIEPTXF5_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF5_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// dieptxf6
#define USBC_DIEPTXF6_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF6_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// dieptxf7
#define USBC_DIEPTXF7_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF7_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// dieptxf8
#define USBC_DIEPTXF8_INEPNTXFSTADDR(n) (((n)&0x1fff) << 0)
#define USBC_DIEPTXF8_INEPNTXFDEP(n) (((n)&0x1ff) << 16)

// hcfg
#define USBC_FSLSPCLKSEL(n) (((n)&0x3) << 0)
#define USBC_FSLSSUPP (1 << 2)
#define USBC_ENA32KHZS (1 << 7)
#define USBC_HCFG_RESVALID(n) (((n)&0xff) << 8)
#define USBC_HCFG_DESCDMA (1 << 23)
#define USBC_FRLISTEN(n) (((n)&0x3) << 24)
#define USBC_PERSCHEDENA (1 << 26)
#define USBC_MODECHTIMEN (1 << 31)

// hfir
#define USBC_FRINT(n) (((n)&0xffff) << 0)
#define USBC_HFIRRLDCTRL (1 << 16)

// hfnum
#define USBC_FRNUM(n) (((n)&0xffff) << 0)
#define USBC_FRREM(n) (((n)&0xffff) << 16)

// hptxsts
#define USBC_PTXFSPCAVAIL(n) (((n)&0xffff) << 0)
#define USBC_PTXQSPCAVAIL(n) (((n)&0xff) << 16)
#define USBC_PTXQTOP(n) (((n)&0xff) << 24)

// haint
#define USBC_HAINT(n) (((n)&0xffff) << 0)

// haintmsk
#define USBC_HAINTMSK(n) (((n)&0xffff) << 0)

// hflbaddr
#define USBC_HFLBADDR(n) (((n)&0xffffffff) << 0)

// hprt
#define USBC_PRTCONNSTS (1 << 0)
#define USBC_PRTCONNDET (1 << 1)
#define USBC_PRTENA (1 << 2)
#define USBC_PRTENCHNG (1 << 3)
#define USBC_PRTOVRCURRACT (1 << 4)
#define USBC_PRTOVRCURRCHNG (1 << 5)
#define USBC_PRTRES (1 << 6)
#define USBC_PRTSUSP (1 << 7)
#define USBC_PRTRST (1 << 8)
#define USBC_PRTLNSTS(n) (((n)&0x3) << 10)
#define USBC_PRTPWR (1 << 12)
#define USBC_PRTTSTCTL(n) (((n)&0xf) << 13)
#define USBC_PRTSPD(n) (((n)&0x3) << 17)

// hcchar0
#define USBC_HCCHAR0_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR0_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR0_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt0
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint0
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT0_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT0_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk0
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz0
#define USBC_HCTSIZ0_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ0_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma0
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab0
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar1
#define USBC_HCCHAR1_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR1_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR1_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt1
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint1
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT1_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT1_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk1
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz1
#define USBC_HCTSIZ1_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ1_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma1
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab1
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar2
#define USBC_HCCHAR2_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR2_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR2_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt2
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint2
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT2_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT2_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk2
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz2
#define USBC_HCTSIZ2_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ2_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma2
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab2
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar3
#define USBC_HCCHAR3_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR3_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR3_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt3
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint3
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT3_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT3_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk3
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz3
#define USBC_HCTSIZ3_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ3_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma3
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab3
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar4
#define USBC_HCCHAR4_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR4_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR4_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt4
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint4
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT4_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT4_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk4
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz4
#define USBC_HCTSIZ4_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ4_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma4
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab4
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar5
#define USBC_HCCHAR5_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR5_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR5_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt5
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint5
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT5_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT5_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk5
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz5
#define USBC_HCTSIZ5_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ5_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma5
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab5
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar6
#define USBC_HCCHAR6_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR6_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR6_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt6
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint6
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT6_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT6_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk6
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz6
#define USBC_HCTSIZ6_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ6_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma6
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab6
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar7
#define USBC_HCCHAR7_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR7_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR7_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt7
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint7
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT7_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT7_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk7
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz7
#define USBC_HCTSIZ7_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ7_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma7
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab7
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar8
#define USBC_HCCHAR8_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR8_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR8_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt8
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint8
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT8_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT8_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk8
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz8
#define USBC_HCTSIZ8_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ8_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma8
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab8
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar9
#define USBC_HCCHAR9_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR9_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR9_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt9
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint9
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT9_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT9_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk9
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz9
#define USBC_HCTSIZ9_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ9_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma9
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab9
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar10
#define USBC_HCCHAR10_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR10_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR10_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt10
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint10
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT10_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT10_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk10
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz10
#define USBC_HCTSIZ10_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ10_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma10
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab10
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar11
#define USBC_HCCHAR11_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR11_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR11_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt11
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint11
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT11_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT11_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk11
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz11
#define USBC_HCTSIZ11_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ11_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma11
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab11
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar12
#define USBC_HCCHAR12_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR12_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR12_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt12
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint12
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT12_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT12_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk12
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz12
#define USBC_HCTSIZ12_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ12_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma12
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab12
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar13
#define USBC_HCCHAR13_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR13_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR13_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt13
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint13
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT13_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT13_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk13
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz13
#define USBC_HCTSIZ13_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ13_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma13
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab13
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar14
#define USBC_HCCHAR14_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR14_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR14_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt14
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint14
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT14_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT14_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk14
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz14
#define USBC_HCTSIZ14_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ14_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma14
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab14
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// hcchar15
#define USBC_HCCHAR15_MPS(n) (((n)&0x7ff) << 0)
#define USBC_EPNUM(n) (((n)&0xf) << 11)
#define USBC_HCCHAR15_EPDIR (1 << 15)
#define USBC_LSPDDEV (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_EC(n) (((n)&0x3) << 20)
#define USBC_HCCHAR15_DEVADDR(n) (((n)&0x7f) << 22)
#define USBC_ODDFRM (1 << 29)
#define USBC_CHDIS (1 << 30)
#define USBC_CHENA (1 << 31)

// hcsplt15
#define USBC_PRTADDR(n) (((n)&0x7f) << 0)
#define USBC_HUBADDR(n) (((n)&0x7f) << 7)
#define USBC_XACTPOS(n) (((n)&0x3) << 14)
#define USBC_COMPSPLT (1 << 16)
#define USBC_SPLTENA (1 << 31)

// hcint15
#define USBC_XFERCOMPL (1 << 0)
#define USBC_CHHLTD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_HCINT15_STALL (1 << 3)
#define USBC_NAK (1 << 4)
#define USBC_ACK (1 << 5)
#define USBC_NYET (1 << 6)
#define USBC_XACTERR (1 << 7)
#define USBC_BBLERR (1 << 8)
#define USBC_FRMOVRUN (1 << 9)
#define USBC_DATATGLERR (1 << 10)
#define USBC_HCINT15_BNAINTR (1 << 11)
#define USBC_XCS_XACT_ERR (1 << 12)
#define USBC_DESC_LST_ROLLINTR (1 << 13)

// hcintmsk15
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_CHHLTDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_BNAINTRMSK (1 << 11)
#define USBC_DESC_LST_ROLLINTRMSK (1 << 13)

// hctsiz15
#define USBC_HCTSIZ15_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_HCTSIZ15_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_PID(n) (((n)&0x3) << 29)
#define USBC_DOPNG (1 << 31)

// hcdma15
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// hcdmab15
#define USBC_HCDMAB(n) (((n)&0xffffffff) << 0)

// dcfg
#define USBC_DEVSPD(n) (((n)&0x3) << 0)
#define USBC_NZSTSOUTHSHK (1 << 2)
#define USBC_ENA32KHZSUSP (1 << 3)
#define USBC_DCFG_DEVADDR(n) (((n)&0x7f) << 4)
#define USBC_PERFRINT(n) (((n)&0x3) << 11)
#define USBC_ENDEVOUTNAK (1 << 13)
#define USBC_XCVRDLY (1 << 14)
#define USBC_ERRATICINTMSK (1 << 15)
#define USBC_DCFG_DESCDMA (1 << 23)
#define USBC_PERSCHINTVL(n) (((n)&0x3) << 24)
#define USBC_DCFG_RESVALID(n) (((n)&0x3f) << 26)

// dctl
#define USBC_RMTWKUPSIG (1 << 0)
#define USBC_SFTDISCON (1 << 1)
#define USBC_GNPINNAKSTS (1 << 2)
#define USBC_GOUTNAKSTS (1 << 3)
#define USBC_TSTCTL(n) (((n)&0x7) << 4)
#define USBC_SGNPINNAK (1 << 7)
#define USBC_CGNPINNAK (1 << 8)
#define USBC_SGOUTNAK (1 << 9)
#define USBC_CGOUTNAK (1 << 10)
#define USBC_PWRONPRGDONE (1 << 11)
#define USBC_GMC(n) (((n)&0x3) << 13)
#define USBC_IGNRFRMNUM (1 << 15)
#define USBC_NAKONBBLE (1 << 16)
#define USBC_ENCONTONBNA (1 << 17)

// dsts
#define USBC_SUSPSTS (1 << 0)
#define USBC_ENUMSPD(n) (((n)&0x3) << 1)
#define USBC_ERRTICERR (1 << 3)
#define USBC_SOFFN(n) (((n)&0x3fff) << 8)
#define USBC_DEVLNSTS(n) (((n)&0x3) << 22)

// diepmsk
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_EPDISBLDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_TIMEOUTMSK (1 << 3)
#define USBC_INTKNTXFEMPMSK (1 << 4)
#define USBC_INTKNEPMISMSK (1 << 5)
#define USBC_INEPNAKEFFMSK (1 << 6)
#define USBC_TXFIFOUNDRNMSK (1 << 8)
#define USBC_BNAININTRMSK (1 << 9)
#define USBC_NAKMSK (1 << 13)

// doepmsk
#define USBC_XFERCOMPLMSK (1 << 0)
#define USBC_EPDISBLDMSK (1 << 1)
#define USBC_AHBERRMSK (1 << 2)
#define USBC_SETUPMSK (1 << 3)
#define USBC_OUTTKNEPDISMSK (1 << 4)
#define USBC_STSPHSERCVDMSK (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERRMSK (1 << 8)
#define USBC_BNAOUTINTRMSK (1 << 9)
#define USBC_BBLEERRMSK (1 << 12)
#define USBC_NAKMSK (1 << 13)
#define USBC_NYETMSK (1 << 14)

// daint
#define USBC_INEPINT0 (1 << 0)
#define USBC_INEPINT1 (1 << 1)
#define USBC_INEPINT2 (1 << 2)
#define USBC_INEPINT3 (1 << 3)
#define USBC_INEPINT4 (1 << 4)
#define USBC_INEPINT5 (1 << 5)
#define USBC_INEPINT6 (1 << 6)
#define USBC_INEPINT7 (1 << 7)
#define USBC_INEPINT8 (1 << 8)
#define USBC_INEPINT9 (1 << 9)
#define USBC_INEPINT10 (1 << 10)
#define USBC_INEPINT11 (1 << 11)
#define USBC_INEPINT12 (1 << 12)
#define USBC_OUTEPINT0 (1 << 16)
#define USBC_OUTEPINT1 (1 << 17)
#define USBC_OUTEPINT2 (1 << 18)
#define USBC_OUTEPINT3 (1 << 19)
#define USBC_OUTEPINT4 (1 << 20)
#define USBC_OUTEPINT5 (1 << 21)
#define USBC_OUTEPINT6 (1 << 22)
#define USBC_OUTEPINT7 (1 << 23)
#define USBC_OUTEPINT8 (1 << 24)
#define USBC_OUTEPINT9 (1 << 25)
#define USBC_OUTEPINT10 (1 << 26)
#define USBC_OUTEPINT11 (1 << 27)
#define USBC_OUTEPINT12 (1 << 28)

// daintmsk
#define USBC_INEPMSK0 (1 << 0)
#define USBC_INEPMSK1 (1 << 1)
#define USBC_INEPMSK2 (1 << 2)
#define USBC_INEPMSK3 (1 << 3)
#define USBC_INEPMSK4 (1 << 4)
#define USBC_INEPMSK5 (1 << 5)
#define USBC_INEPMSK6 (1 << 6)
#define USBC_INEPMSK7 (1 << 7)
#define USBC_INEPMSK8 (1 << 8)
#define USBC_INEPMSK9 (1 << 9)
#define USBC_INEPMSK10 (1 << 10)
#define USBC_INEPMSK11 (1 << 11)
#define USBC_INEPMSK12 (1 << 12)
#define USBC_OUTEPMSK0 (1 << 16)
#define USBC_OUTEPMSK1 (1 << 17)
#define USBC_OUTEPMSK2 (1 << 18)
#define USBC_OUTEPMSK3 (1 << 19)
#define USBC_OUTEPMSK4 (1 << 20)
#define USBC_OUTEPMSK5 (1 << 21)
#define USBC_OUTEPMSK6 (1 << 22)
#define USBC_OUTEPMSK7 (1 << 23)
#define USBC_OUTEPMSK8 (1 << 24)
#define USBC_OUTEPMSK9 (1 << 25)
#define USBC_OUTEPMSK10 (1 << 26)
#define USBC_OUTEPMSK11 (1 << 27)
#define USBC_OUTEPMSK12 (1 << 28)

// dvbusdis
#define USBC_DVBUSDIS(n) (((n)&0xffff) << 0)

// dvbuspulse
#define USBC_DVBUSPULSE(n) (((n)&0xfff) << 0)

// dthrctl
#define USBC_NONISOTHREN (1 << 0)
#define USBC_ISOTHREN (1 << 1)
#define USBC_TXTHRLEN(n) (((n)&0x1ff) << 2)
#define USBC_AHBTHRRATIO(n) (((n)&0x3) << 11)
#define USBC_RXTHREN (1 << 16)
#define USBC_RXTHRLEN(n) (((n)&0x1ff) << 17)
#define USBC_ARBPRKEN (1 << 27)

// diepempmsk
#define USBC_INEPTXFEMPMSK(n) (((n)&0xffff) << 0)

// diepctl0
#define USBC_DIEPCTL0_MPS(n) (((n)&0x3) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL0_STALL (1 << 21)
#define USBC_DIEPCTL0_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint0
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT0_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz0
#define USBC_DIEPTSIZ0_XFERSIZE(n) (((n)&0x7f) << 0)
#define USBC_DIEPTSIZ0_PKTCNT(n) (((n)&0x3) << 19)

// diepdma0
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts0
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab0
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl1
#define USBC_DIEPCTL1_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL1_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL1_STALL (1 << 21)
#define USBC_DIEPCTL1_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint1
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT1_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz1
#define USBC_DIEPTSIZ1_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ1_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma1
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts1
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab1
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl2
#define USBC_DIEPCTL2_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL2_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL2_STALL (1 << 21)
#define USBC_DIEPCTL2_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint2
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT2_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz2
#define USBC_DIEPTSIZ2_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ2_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma2
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts2
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab2
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl3
#define USBC_DIEPCTL3_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL3_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL3_STALL (1 << 21)
#define USBC_DIEPCTL3_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint3
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT3_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz3
#define USBC_DIEPTSIZ3_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ3_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma3
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts3
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab3
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl4
#define USBC_DIEPCTL4_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL4_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL4_STALL (1 << 21)
#define USBC_DIEPCTL4_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint4
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT4_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz4
#define USBC_DIEPTSIZ4_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ4_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma4
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts4
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab4
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl5
#define USBC_DIEPCTL5_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL5_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL5_STALL (1 << 21)
#define USBC_DIEPCTL5_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint5
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT5_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz5
#define USBC_DIEPTSIZ5_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ5_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma5
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts5
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab5
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl6
#define USBC_DIEPCTL6_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL6_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL6_STALL (1 << 21)
#define USBC_DIEPCTL6_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint6
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT6_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz6
#define USBC_DIEPTSIZ6_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ6_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma6
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts6
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab6
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl7
#define USBC_DIEPCTL7_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL7_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL7_STALL (1 << 21)
#define USBC_DIEPCTL7_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint7
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT7_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz7
#define USBC_DIEPTSIZ7_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ7_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma7
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts7
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab7
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl8
#define USBC_DIEPCTL8_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL8_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL8_STALL (1 << 21)
#define USBC_DIEPCTL8_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint8
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT8_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz8
#define USBC_DIEPTSIZ8_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ8_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma8
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts8
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab8
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl9
#define USBC_DIEPCTL9_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL9_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL9_STALL (1 << 21)
#define USBC_DIEPCTL9_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint9
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT9_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz9
#define USBC_DIEPTSIZ9_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ9_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma9
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts9
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab9
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl10
#define USBC_DIEPCTL10_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL10_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL10_STALL (1 << 21)
#define USBC_DIEPCTL10_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint10
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT10_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz10
#define USBC_DIEPTSIZ10_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ10_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma10
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts10
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab10
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl11
#define USBC_DIEPCTL11_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL11_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL11_STALL (1 << 21)
#define USBC_DIEPCTL11_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint11
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT11_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz11
#define USBC_DIEPTSIZ11_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ11_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma11
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts11
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab11
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// diepctl12
#define USBC_DIEPCTL12_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DIEPCTL12_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_DIEPCTL12_STALL (1 << 21)
#define USBC_DIEPCTL12_TXFNUM(n) (((n)&0xf) << 22)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// diepint12
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_TIMEOUT (1 << 3)
#define USBC_INTKNTXFEMP (1 << 4)
#define USBC_INTKNEPMIS (1 << 5)
#define USBC_INEPNAKEFF (1 << 6)
#define USBC_TXFEMP (1 << 7)
#define USBC_TXFIFOUNDRN (1 << 8)
#define USBC_DIEPINT12_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)

// dieptsiz12
#define USBC_DIEPTSIZ12_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DIEPTSIZ12_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_MC(n) (((n)&0x3) << 29)

// diepdma12
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// dtxfsts12
#define USBC_INEPTXFSPCAVAIL(n) (((n)&0xffff) << 0)

// diepdmab12
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl0
#define USBC_DOEPCTL0_MPS(n) (((n)&0x3) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL0_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint0
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT0_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz0
#define USBC_DOEPTSIZ0_XFERSIZE(n) (((n)&0x7f) << 0)
#define USBC_DOEPTSIZ0_PKTCNT (1 << 19)
#define USBC_SUPCNT(n) (((n)&0x3) << 29)

// doepdma0
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab0
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl1
#define USBC_DOEPCTL1_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL1_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL1_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint1
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT1_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz1
#define USBC_DOEPTSIZ1_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ1_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma1
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab1
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl2
#define USBC_DOEPCTL2_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL2_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL2_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint2
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT2_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz2
#define USBC_DOEPTSIZ2_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ2_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma2
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab2
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl3
#define USBC_DOEPCTL3_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL3_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL3_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint3
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT3_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz3
#define USBC_DOEPTSIZ3_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ3_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma3
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab3
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl4
#define USBC_DOEPCTL4_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL4_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL4_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint4
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT4_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz4
#define USBC_DOEPTSIZ4_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ4_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma4
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab4
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl5
#define USBC_DOEPCTL5_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL5_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL5_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint5
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT5_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz5
#define USBC_DOEPTSIZ5_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ5_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma5
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab5
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl6
#define USBC_DOEPCTL6_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL6_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL6_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint6
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT6_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz6
#define USBC_DOEPTSIZ6_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ6_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma6
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab6
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl7
#define USBC_DOEPCTL7_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL7_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL7_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint7
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT7_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz7
#define USBC_DOEPTSIZ7_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ7_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma7
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab7
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl8
#define USBC_DOEPCTL8_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL8_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL8_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint8
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT8_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz8
#define USBC_DOEPTSIZ8_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ8_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma8
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab8
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl9
#define USBC_DOEPCTL9_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL9_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL9_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint9
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT9_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz9
#define USBC_DOEPTSIZ9_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ9_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma9
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab9
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl10
#define USBC_DOEPCTL10_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL10_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL10_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint10
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT10_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz10
#define USBC_DOEPTSIZ10_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ10_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma10
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab10
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl11
#define USBC_DOEPCTL11_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL11_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL11_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint11
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT11_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz11
#define USBC_DOEPTSIZ11_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ11_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma11
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab11
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// doepctl12
#define USBC_DOEPCTL12_MPS(n) (((n)&0x7ff) << 0)
#define USBC_USBACTEP (1 << 15)
#define USBC_DOEPCTL12_DPID (1 << 16)
#define USBC_NAKSTS (1 << 17)
#define USBC_EPTYPE(n) (((n)&0x3) << 18)
#define USBC_SNP (1 << 20)
#define USBC_DOEPCTL12_STALL (1 << 21)
#define USBC_CNAK (1 << 26)
#define USBC_SNAK (1 << 27)
#define USBC_SETD0PID (1 << 28)
#define USBC_SETD1PID (1 << 29)
#define USBC_EPDIS (1 << 30)
#define USBC_EPENA (1 << 31)

// doepint12
#define USBC_XFERCOMPL (1 << 0)
#define USBC_EPDISBLD (1 << 1)
#define USBC_AHBERR (1 << 2)
#define USBC_SETUP (1 << 3)
#define USBC_OUTTKNEPDIS (1 << 4)
#define USBC_STSPHSERCVD (1 << 5)
#define USBC_BACK2BACKSETUP (1 << 6)
#define USBC_OUTPKTERR (1 << 8)
#define USBC_DOEPINT12_BNAINTR (1 << 9)
#define USBC_PKTDRPSTS (1 << 11)
#define USBC_BBLEERR (1 << 12)
#define USBC_NAKINTRPT (1 << 13)
#define USBC_NYETINTRPT (1 << 14)
#define USBC_STUPPKTRCVD (1 << 15)

// doeptsiz12
#define USBC_DOEPTSIZ12_XFERSIZE(n) (((n)&0x7ffff) << 0)
#define USBC_DOEPTSIZ12_PKTCNT(n) (((n)&0x3ff) << 19)
#define USBC_RXDPID(n) (((n)&0x3) << 29)

// doepdma12
#define USBC_DMAADDR(n) (((n)&0xffffffff) << 0)

// doepdmab12
#define USBC_DMABUFFERADDR(n) (((n)&0xffffffff) << 0)

// pcgcctl
#define USBC_STOPPCLK (1 << 0)
#define USBC_RSTPDWNMODULE (1 << 3)
#define USBC_PHYSLEEP (1 << 6)
#define USBC_L1SUSPENDED (1 << 7)

#endif // _USBC_H_
