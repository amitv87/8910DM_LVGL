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

#ifndef _CP_MAILBOX_H_
#define _CP_MAILBOX_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_MAILBOX_BASE (0x05104000)
#else
#define REG_MAILBOX_BASE (0x50104000)
#endif

typedef volatile struct
{
    uint32_t intgr0;     // 0x00000000
    uint32_t intstr0;    // 0x00000004
    uint32_t intcr0;     // 0x00000008
    uint32_t intmr0;     // 0x0000000c
    uint32_t intsr0;     // 0x00000010
    uint32_t intmsr0;    // 0x00000014
    uint32_t __24[2];    // 0x00000018
    uint32_t intgr1;     // 0x00000020
    uint32_t intstr1;    // 0x00000024
    uint32_t intcr1;     // 0x00000028
    uint32_t intmr1;     // 0x0000002c
    uint32_t intsr1;     // 0x00000030
    uint32_t intmsr1;    // 0x00000034
    uint32_t __56[2];    // 0x00000038
    uint32_t intgr2;     // 0x00000040
    uint32_t intstr2;    // 0x00000044
    uint32_t intcr2;     // 0x00000048
    uint32_t intmr2;     // 0x0000004c
    uint32_t intsr2;     // 0x00000050
    uint32_t intmsr2;    // 0x00000054
    uint32_t __88[2];    // 0x00000058
    uint32_t intgr3;     // 0x00000060
    uint32_t intstr3;    // 0x00000064
    uint32_t intcr3;     // 0x00000068
    uint32_t intmr3;     // 0x0000006c
    uint32_t intsr3;     // 0x00000070
    uint32_t intmsr3;    // 0x00000074
    uint32_t __120[2];   // 0x00000078
    uint32_t intgr4;     // 0x00000080
    uint32_t intstr4;    // 0x00000084
    uint32_t intcr4;     // 0x00000088
    uint32_t intmr4;     // 0x0000008c
    uint32_t intsr4;     // 0x00000090
    uint32_t intmsr4;    // 0x00000094
    uint32_t __152[2];   // 0x00000098
    uint32_t intgr5;     // 0x000000a0
    uint32_t intstr5;    // 0x000000a4
    uint32_t intcr5;     // 0x000000a8
    uint32_t intmr5;     // 0x000000ac
    uint32_t intsr5;     // 0x000000b0
    uint32_t intmsr5;    // 0x000000b4
    uint32_t __184[18];  // 0x000000b8
    uint32_t sysmail0;   // 0x00000100
    uint32_t sysmail1;   // 0x00000104
    uint32_t sysmail2;   // 0x00000108
    uint32_t sysmail3;   // 0x0000010c
    uint32_t sysmail4;   // 0x00000110
    uint32_t sysmail5;   // 0x00000114
    uint32_t sysmail6;   // 0x00000118
    uint32_t sysmail7;   // 0x0000011c
    uint32_t sysmail8;   // 0x00000120
    uint32_t sysmail9;   // 0x00000124
    uint32_t sysmail10;  // 0x00000128
    uint32_t sysmail11;  // 0x0000012c
    uint32_t sysmail12;  // 0x00000130
    uint32_t sysmail13;  // 0x00000134
    uint32_t sysmail14;  // 0x00000138
    uint32_t sysmail15;  // 0x0000013c
    uint32_t sysmail16;  // 0x00000140
    uint32_t sysmail17;  // 0x00000144
    uint32_t sysmail18;  // 0x00000148
    uint32_t sysmail19;  // 0x0000014c
    uint32_t sysmail20;  // 0x00000150
    uint32_t sysmail21;  // 0x00000154
    uint32_t sysmail22;  // 0x00000158
    uint32_t sysmail23;  // 0x0000015c
    uint32_t sysmail24;  // 0x00000160
    uint32_t sysmail25;  // 0x00000164
    uint32_t sysmail26;  // 0x00000168
    uint32_t sysmail27;  // 0x0000016c
    uint32_t sysmail28;  // 0x00000170
    uint32_t sysmail29;  // 0x00000174
    uint32_t sysmail30;  // 0x00000178
    uint32_t sysmail31;  // 0x0000017c
    uint32_t __384[32];  // 0x00000180
    uint32_t sysmail32;  // 0x00000200
    uint32_t sysmail33;  // 0x00000204
    uint32_t sysmail34;  // 0x00000208
    uint32_t sysmail35;  // 0x0000020c
    uint32_t sysmail36;  // 0x00000210
    uint32_t sysmail37;  // 0x00000214
    uint32_t sysmail38;  // 0x00000218
    uint32_t sysmail39;  // 0x0000021c
    uint32_t sysmail40;  // 0x00000220
    uint32_t sysmail41;  // 0x00000224
    uint32_t sysmail42;  // 0x00000228
    uint32_t sysmail43;  // 0x0000022c
    uint32_t sysmail44;  // 0x00000230
    uint32_t sysmail45;  // 0x00000234
    uint32_t sysmail46;  // 0x00000238
    uint32_t sysmail47;  // 0x0000023c
    uint32_t sysmail48;  // 0x00000240
    uint32_t sysmail49;  // 0x00000244
    uint32_t sysmail50;  // 0x00000248
    uint32_t sysmail51;  // 0x0000024c
    uint32_t sysmail52;  // 0x00000250
    uint32_t sysmail53;  // 0x00000254
    uint32_t sysmail54;  // 0x00000258
    uint32_t sysmail55;  // 0x0000025c
    uint32_t sysmail56;  // 0x00000260
    uint32_t sysmail57;  // 0x00000264
    uint32_t sysmail58;  // 0x00000268
    uint32_t sysmail59;  // 0x0000026c
    uint32_t sysmail60;  // 0x00000270
    uint32_t sysmail61;  // 0x00000274
    uint32_t sysmail62;  // 0x00000278
    uint32_t sysmail63;  // 0x0000027c
    uint32_t __640[32];  // 0x00000280
    uint32_t sysmail64;  // 0x00000300
    uint32_t sysmail65;  // 0x00000304
    uint32_t sysmail66;  // 0x00000308
    uint32_t sysmail67;  // 0x0000030c
    uint32_t sysmail68;  // 0x00000310
    uint32_t sysmail69;  // 0x00000314
    uint32_t sysmail70;  // 0x00000318
    uint32_t sysmail71;  // 0x0000031c
    uint32_t sysmail72;  // 0x00000320
    uint32_t sysmail73;  // 0x00000324
    uint32_t sysmail74;  // 0x00000328
    uint32_t sysmail75;  // 0x0000032c
    uint32_t sysmail76;  // 0x00000330
    uint32_t sysmail77;  // 0x00000334
    uint32_t sysmail78;  // 0x00000338
    uint32_t sysmail79;  // 0x0000033c
    uint32_t sysmail80;  // 0x00000340
    uint32_t sysmail81;  // 0x00000344
    uint32_t sysmail82;  // 0x00000348
    uint32_t sysmail83;  // 0x0000034c
    uint32_t sysmail84;  // 0x00000350
    uint32_t sysmail85;  // 0x00000354
    uint32_t sysmail86;  // 0x00000358
    uint32_t sysmail87;  // 0x0000035c
    uint32_t sysmail88;  // 0x00000360
    uint32_t sysmail89;  // 0x00000364
    uint32_t sysmail90;  // 0x00000368
    uint32_t sysmail91;  // 0x0000036c
    uint32_t sysmail92;  // 0x00000370
    uint32_t sysmail93;  // 0x00000374
    uint32_t sysmail94;  // 0x00000378
    uint32_t sysmail95;  // 0x0000037c
    uint32_t __896[32];  // 0x00000380
    uint32_t sysmail96;  // 0x00000400
    uint32_t sysmail97;  // 0x00000404
    uint32_t sysmail98;  // 0x00000408
    uint32_t sysmail99;  // 0x0000040c
    uint32_t sysmail100; // 0x00000410
    uint32_t sysmail101; // 0x00000414
    uint32_t sysmail102; // 0x00000418
    uint32_t sysmail103; // 0x0000041c
    uint32_t sysmail104; // 0x00000420
    uint32_t sysmail105; // 0x00000424
    uint32_t sysmail106; // 0x00000428
    uint32_t sysmail107; // 0x0000042c
    uint32_t sysmail108; // 0x00000430
    uint32_t sysmail109; // 0x00000434
    uint32_t sysmail110; // 0x00000438
    uint32_t sysmail111; // 0x0000043c
    uint32_t sysmail112; // 0x00000440
    uint32_t sysmail113; // 0x00000444
    uint32_t sysmail114; // 0x00000448
    uint32_t sysmail115; // 0x0000044c
    uint32_t sysmail116; // 0x00000450
    uint32_t sysmail117; // 0x00000454
    uint32_t sysmail118; // 0x00000458
    uint32_t sysmail119; // 0x0000045c
    uint32_t sysmail120; // 0x00000460
    uint32_t sysmail121; // 0x00000464
    uint32_t sysmail122; // 0x00000468
    uint32_t sysmail123; // 0x0000046c
    uint32_t sysmail124; // 0x00000470
    uint32_t sysmail125; // 0x00000474
    uint32_t sysmail126; // 0x00000478
    uint32_t sysmail127; // 0x0000047c
    uint32_t __1152[32]; // 0x00000480
    uint32_t sysmail128; // 0x00000500
    uint32_t sysmail129; // 0x00000504
    uint32_t sysmail130; // 0x00000508
    uint32_t sysmail131; // 0x0000050c
    uint32_t sysmail132; // 0x00000510
    uint32_t sysmail133; // 0x00000514
    uint32_t sysmail134; // 0x00000518
    uint32_t sysmail135; // 0x0000051c
    uint32_t sysmail136; // 0x00000520
    uint32_t sysmail137; // 0x00000524
    uint32_t sysmail138; // 0x00000528
    uint32_t sysmail139; // 0x0000052c
    uint32_t sysmail140; // 0x00000530
    uint32_t sysmail141; // 0x00000534
    uint32_t sysmail142; // 0x00000538
    uint32_t sysmail143; // 0x0000053c
    uint32_t sysmail144; // 0x00000540
    uint32_t sysmail145; // 0x00000544
    uint32_t sysmail146; // 0x00000548
    uint32_t sysmail147; // 0x0000054c
    uint32_t sysmail148; // 0x00000550
    uint32_t sysmail149; // 0x00000554
    uint32_t sysmail150; // 0x00000558
    uint32_t sysmail151; // 0x0000055c
    uint32_t sysmail152; // 0x00000560
    uint32_t sysmail153; // 0x00000564
    uint32_t sysmail154; // 0x00000568
    uint32_t sysmail155; // 0x0000056c
    uint32_t sysmail156; // 0x00000570
    uint32_t sysmail157; // 0x00000574
    uint32_t sysmail158; // 0x00000578
    uint32_t sysmail159; // 0x0000057c
    uint32_t __1408[32]; // 0x00000580
    uint32_t sysmail160; // 0x00000600
    uint32_t sysmail161; // 0x00000604
    uint32_t sysmail162; // 0x00000608
    uint32_t sysmail163; // 0x0000060c
    uint32_t sysmail164; // 0x00000610
    uint32_t sysmail165; // 0x00000614
    uint32_t sysmail166; // 0x00000618
    uint32_t sysmail167; // 0x0000061c
    uint32_t sysmail168; // 0x00000620
    uint32_t sysmail169; // 0x00000624
    uint32_t sysmail170; // 0x00000628
    uint32_t sysmail171; // 0x0000062c
    uint32_t sysmail172; // 0x00000630
    uint32_t sysmail173; // 0x00000634
    uint32_t sysmail174; // 0x00000638
    uint32_t sysmail175; // 0x0000063c
    uint32_t sysmail176; // 0x00000640
    uint32_t sysmail177; // 0x00000644
    uint32_t sysmail178; // 0x00000648
    uint32_t sysmail179; // 0x0000064c
    uint32_t sysmail180; // 0x00000650
    uint32_t sysmail181; // 0x00000654
    uint32_t sysmail182; // 0x00000658
    uint32_t sysmail183; // 0x0000065c
    uint32_t sysmail184; // 0x00000660
    uint32_t sysmail185; // 0x00000664
    uint32_t sysmail186; // 0x00000668
    uint32_t sysmail187; // 0x0000066c
    uint32_t sysmail188; // 0x00000670
    uint32_t sysmail189; // 0x00000674
    uint32_t sysmail190; // 0x00000678
    uint32_t sysmail191; // 0x0000067c
} HWP_CP_MAILBOX_T;

#define hwp_mailbox ((HWP_CP_MAILBOX_T *)REG_ACCESS_ADDRESS(REG_MAILBOX_BASE))

// intgr0
#define CP_MAILBOX_INTERRUPT0GENERATE(n) (((n)&0xffffffff) << 0)

// intstr0
#define CP_MAILBOX_INTERRUPT0BITSET(n) (((n)&0xffffffff) << 0)

// intcr0
#define CP_MAILBOX_INTERRUPT0BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr0
#define CP_MAILBOX_INTERRUPT0MASK(n) (((n)&0xffffffff) << 0)

// intsr0
#define CP_MAILBOX_INTERRUPT0STATUS(n) (((n)&0xffffffff) << 0)

// intmsr0
#define CP_MAILBOX_INTERRUPT0MASKSTATUS(n) (((n)&0xffffffff) << 0)

// intgr1
#define CP_MAILBOX_INTERRUPT1GENERATE(n) (((n)&0xffffffff) << 0)

// intstr1
#define CP_MAILBOX_INTERRUPT1BITSET(n) (((n)&0xffffffff) << 0)

// intcr1
#define CP_MAILBOX_INTERRUPT1BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr1
#define CP_MAILBOX_INTERRUPT1MASK(n) (((n)&0xffffffff) << 0)

// intsr1
#define CP_MAILBOX_INTERRUPT1STATUS(n) (((n)&0xffffffff) << 0)

// intmsr1
#define CP_MAILBOX_INTERRUPT1MASKSTATUS(n) (((n)&0xffffffff) << 0)

// intgr2
#define CP_MAILBOX_INTERRUPT2GENERATE(n) (((n)&0xffffffff) << 0)

// intstr2
#define CP_MAILBOX_INTERRUPT2BITSET(n) (((n)&0xffffffff) << 0)

// intcr2
#define CP_MAILBOX_INTERRUPT2BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr2
#define CP_MAILBOX_INTERRUPT2MASK(n) (((n)&0xffffffff) << 0)

// intsr2
#define CP_MAILBOX_INTERRUPT2STATUS(n) (((n)&0xffffffff) << 0)

// intmsr2
#define CP_MAILBOX_INTERRUPT2MASKSTATUS(n) (((n)&0xffffffff) << 0)

// intgr3
#define CP_MAILBOX_INTERRUPT3GENERATE(n) (((n)&0xffffffff) << 0)

// intstr3
#define CP_MAILBOX_INTERRUPT3BITSET(n) (((n)&0xffffffff) << 0)

// intcr3
#define CP_MAILBOX_INTERRUPT3BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr3
#define CP_MAILBOX_INTERRUPT3MASK(n) (((n)&0xffffffff) << 0)

// intsr3
#define CP_MAILBOX_INTERRUPT3STATUS(n) (((n)&0xffffffff) << 0)

// intmsr3
#define CP_MAILBOX_INTERRUPT3MASKSTATUS(n) (((n)&0xffffffff) << 0)

// intgr4
#define CP_MAILBOX_INTERRUPT4GENERATE(n) (((n)&0xffffffff) << 0)

// intstr4
#define CP_MAILBOX_INTERRUPT4BITSET(n) (((n)&0xffffffff) << 0)

// intcr4
#define CP_MAILBOX_INTERRUPT4BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr4
#define CP_MAILBOX_INTERRUPT4MASK(n) (((n)&0xffffffff) << 0)

// intsr4
#define CP_MAILBOX_INTERRUPT4STATUS(n) (((n)&0xffffffff) << 0)

// intmsr4
#define CP_MAILBOX_INTERRUPT4MASKSTATUS(n) (((n)&0xffffffff) << 0)

// intgr5
#define CP_MAILBOX_INTERRUPT5GENERATE(n) (((n)&0xffffffff) << 0)

// intstr5
#define CP_MAILBOX_INTERRUPT5BITSET(n) (((n)&0xffffffff) << 0)

// intcr5
#define CP_MAILBOX_INTERRUPT5BITCLEAN(n) (((n)&0xffffffff) << 0)

// intmr5
#define CP_MAILBOX_INTERRUPT5MASK(n) (((n)&0xffffffff) << 0)

// intsr5
#define CP_MAILBOX_INTERRUPT5STATUS(n) (((n)&0xffffffff) << 0)

// intmsr5
#define CP_MAILBOX_INTERRUPT5MASKSTATUS(n) (((n)&0xffffffff) << 0)

// sysmail0
#define CP_MAILBOX_SYSMAIL0(n) (((n)&0xffffffff) << 0)

// sysmail1
#define CP_MAILBOX_SYSMAIL1(n) (((n)&0xffffffff) << 0)

// sysmail2
#define CP_MAILBOX_SYSMAIL2(n) (((n)&0xffffffff) << 0)

// sysmail3
#define CP_MAILBOX_SYSMAIL3(n) (((n)&0xffffffff) << 0)

// sysmail4
#define CP_MAILBOX_SYSMAIL4(n) (((n)&0xffffffff) << 0)

// sysmail5
#define CP_MAILBOX_SYSMAIL5(n) (((n)&0xffffffff) << 0)

// sysmail6
#define CP_MAILBOX_SYSMAIL6(n) (((n)&0xffffffff) << 0)

// sysmail7
#define CP_MAILBOX_SYSMAIL7(n) (((n)&0xffffffff) << 0)

// sysmail8
#define CP_MAILBOX_SYSMAIL8(n) (((n)&0xffffffff) << 0)

// sysmail9
#define CP_MAILBOX_SYSMAIL9(n) (((n)&0xffffffff) << 0)

// sysmail10
#define CP_MAILBOX_SYSMAIL10(n) (((n)&0xffffffff) << 0)

// sysmail11
#define CP_MAILBOX_SYSMAIL11(n) (((n)&0xffffffff) << 0)

// sysmail12
#define CP_MAILBOX_SYSMAIL12(n) (((n)&0xffffffff) << 0)

// sysmail13
#define CP_MAILBOX_SYSMAIL13(n) (((n)&0xffffffff) << 0)

// sysmail14
#define CP_MAILBOX_SYSMAIL14(n) (((n)&0xffffffff) << 0)

// sysmail15
#define CP_MAILBOX_SYSMAIL15(n) (((n)&0xffffffff) << 0)

// sysmail16
#define CP_MAILBOX_SYSMAIL16(n) (((n)&0xffffffff) << 0)

// sysmail17
#define CP_MAILBOX_SYSMAIL17(n) (((n)&0xffffffff) << 0)

// sysmail18
#define CP_MAILBOX_SYSMAIL18(n) (((n)&0xffffffff) << 0)

// sysmail19
#define CP_MAILBOX_SYSMAIL19(n) (((n)&0xffffffff) << 0)

// sysmail20
#define CP_MAILBOX_SYSMAIL20(n) (((n)&0xffffffff) << 0)

// sysmail21
#define CP_MAILBOX_SYSMAIL21(n) (((n)&0xffffffff) << 0)

// sysmail22
#define CP_MAILBOX_SYSMAIL22(n) (((n)&0xffffffff) << 0)

// sysmail23
#define CP_MAILBOX_SYSMAIL23(n) (((n)&0xffffffff) << 0)

// sysmail24
#define CP_MAILBOX_SYSMAIL24(n) (((n)&0xffffffff) << 0)

// sysmail25
#define CP_MAILBOX_SYSMAIL25(n) (((n)&0xffffffff) << 0)

// sysmail26
#define CP_MAILBOX_SYSMAIL26(n) (((n)&0xffffffff) << 0)

// sysmail27
#define CP_MAILBOX_SYSMAIL27(n) (((n)&0xffffffff) << 0)

// sysmail28
#define CP_MAILBOX_SYSMAIL28(n) (((n)&0xffffffff) << 0)

// sysmail29
#define CP_MAILBOX_SYSMAIL29(n) (((n)&0xffffffff) << 0)

// sysmail30
#define CP_MAILBOX_SYSMAIL30(n) (((n)&0xffffffff) << 0)

// sysmail31
#define CP_MAILBOX_SYSMAIL31(n) (((n)&0xffffffff) << 0)

// sysmail32
#define CP_MAILBOX_SYSMAIL32(n) (((n)&0xffffffff) << 0)

// sysmail33
#define CP_MAILBOX_SYSMAIL33(n) (((n)&0xffffffff) << 0)

// sysmail34
#define CP_MAILBOX_SYSMAIL34(n) (((n)&0xffffffff) << 0)

// sysmail35
#define CP_MAILBOX_SYSMAIL35(n) (((n)&0xffffffff) << 0)

// sysmail36
#define CP_MAILBOX_SYSMAIL36(n) (((n)&0xffffffff) << 0)

// sysmail37
#define CP_MAILBOX_SYSMAIL37(n) (((n)&0xffffffff) << 0)

// sysmail38
#define CP_MAILBOX_SYSMAIL38(n) (((n)&0xffffffff) << 0)

// sysmail39
#define CP_MAILBOX_SYSMAIL39(n) (((n)&0xffffffff) << 0)

// sysmail40
#define CP_MAILBOX_SYSMAIL40(n) (((n)&0xffffffff) << 0)

// sysmail41
#define CP_MAILBOX_SYSMAIL41(n) (((n)&0xffffffff) << 0)

// sysmail42
#define CP_MAILBOX_SYSMAIL42(n) (((n)&0xffffffff) << 0)

// sysmail43
#define CP_MAILBOX_SYSMAIL43(n) (((n)&0xffffffff) << 0)

// sysmail44
#define CP_MAILBOX_SYSMAIL44(n) (((n)&0xffffffff) << 0)

// sysmail45
#define CP_MAILBOX_SYSMAIL45(n) (((n)&0xffffffff) << 0)

// sysmail46
#define CP_MAILBOX_SYSMAIL46(n) (((n)&0xffffffff) << 0)

// sysmail47
#define CP_MAILBOX_SYSMAIL47(n) (((n)&0xffffffff) << 0)

// sysmail48
#define CP_MAILBOX_SYSMAIL48(n) (((n)&0xffffffff) << 0)

// sysmail49
#define CP_MAILBOX_SYSMAIL49(n) (((n)&0xffffffff) << 0)

// sysmail50
#define CP_MAILBOX_SYSMAIL50(n) (((n)&0xffffffff) << 0)

// sysmail51
#define CP_MAILBOX_SYSMAIL51(n) (((n)&0xffffffff) << 0)

// sysmail52
#define CP_MAILBOX_SYSMAIL52(n) (((n)&0xffffffff) << 0)

// sysmail53
#define CP_MAILBOX_SYSMAIL53(n) (((n)&0xffffffff) << 0)

// sysmail54
#define CP_MAILBOX_SYSMAIL54(n) (((n)&0xffffffff) << 0)

// sysmail55
#define CP_MAILBOX_SYSMAIL55(n) (((n)&0xffffffff) << 0)

// sysmail56
#define CP_MAILBOX_SYSMAIL56(n) (((n)&0xffffffff) << 0)

// sysmail57
#define CP_MAILBOX_SYSMAIL57(n) (((n)&0xffffffff) << 0)

// sysmail58
#define CP_MAILBOX_SYSMAIL58(n) (((n)&0xffffffff) << 0)

// sysmail59
#define CP_MAILBOX_SYSMAIL59(n) (((n)&0xffffffff) << 0)

// sysmail60
#define CP_MAILBOX_SYSMAIL60(n) (((n)&0xffffffff) << 0)

// sysmail61
#define CP_MAILBOX_SYSMAIL61(n) (((n)&0xffffffff) << 0)

// sysmail62
#define CP_MAILBOX_SYSMAIL62(n) (((n)&0xffffffff) << 0)

// sysmail63
#define CP_MAILBOX_SYSMAIL63(n) (((n)&0xffffffff) << 0)

// sysmail64
#define CP_MAILBOX_SYSMAIL64(n) (((n)&0xffffffff) << 0)

// sysmail65
#define CP_MAILBOX_SYSMAIL65(n) (((n)&0xffffffff) << 0)

// sysmail66
#define CP_MAILBOX_SYSMAIL66(n) (((n)&0xffffffff) << 0)

// sysmail67
#define CP_MAILBOX_SYSMAIL67(n) (((n)&0xffffffff) << 0)

// sysmail68
#define CP_MAILBOX_SYSMAIL68(n) (((n)&0xffffffff) << 0)

// sysmail69
#define CP_MAILBOX_SYSMAIL69(n) (((n)&0xffffffff) << 0)

// sysmail70
#define CP_MAILBOX_SYSMAIL70(n) (((n)&0xffffffff) << 0)

// sysmail71
#define CP_MAILBOX_SYSMAIL71(n) (((n)&0xffffffff) << 0)

// sysmail72
#define CP_MAILBOX_SYSMAIL72(n) (((n)&0xffffffff) << 0)

// sysmail73
#define CP_MAILBOX_SYSMAIL73(n) (((n)&0xffffffff) << 0)

// sysmail74
#define CP_MAILBOX_SYSMAIL74(n) (((n)&0xffffffff) << 0)

// sysmail75
#define CP_MAILBOX_SYSMAIL75(n) (((n)&0xffffffff) << 0)

// sysmail76
#define CP_MAILBOX_SYSMAIL76(n) (((n)&0xffffffff) << 0)

// sysmail77
#define CP_MAILBOX_SYSMAIL77(n) (((n)&0xffffffff) << 0)

// sysmail78
#define CP_MAILBOX_SYSMAIL78(n) (((n)&0xffffffff) << 0)

// sysmail79
#define CP_MAILBOX_SYSMAIL79(n) (((n)&0xffffffff) << 0)

// sysmail80
#define CP_MAILBOX_SYSMAIL80(n) (((n)&0xffffffff) << 0)

// sysmail81
#define CP_MAILBOX_SYSMAIL81(n) (((n)&0xffffffff) << 0)

// sysmail82
#define CP_MAILBOX_SYSMAIL82(n) (((n)&0xffffffff) << 0)

// sysmail83
#define CP_MAILBOX_SYSMAIL83(n) (((n)&0xffffffff) << 0)

// sysmail84
#define CP_MAILBOX_SYSMAIL84(n) (((n)&0xffffffff) << 0)

// sysmail85
#define CP_MAILBOX_SYSMAIL85(n) (((n)&0xffffffff) << 0)

// sysmail86
#define CP_MAILBOX_SYSMAIL86(n) (((n)&0xffffffff) << 0)

// sysmail87
#define CP_MAILBOX_SYSMAIL87(n) (((n)&0xffffffff) << 0)

// sysmail88
#define CP_MAILBOX_SYSMAIL88(n) (((n)&0xffffffff) << 0)

// sysmail89
#define CP_MAILBOX_SYSMAIL89(n) (((n)&0xffffffff) << 0)

// sysmail90
#define CP_MAILBOX_SYSMAIL90(n) (((n)&0xffffffff) << 0)

// sysmail91
#define CP_MAILBOX_SYSMAIL91(n) (((n)&0xffffffff) << 0)

// sysmail92
#define CP_MAILBOX_SYSMAIL92(n) (((n)&0xffffffff) << 0)

// sysmail93
#define CP_MAILBOX_SYSMAIL93(n) (((n)&0xffffffff) << 0)

// sysmail94
#define CP_MAILBOX_SYSMAIL94(n) (((n)&0xffffffff) << 0)

// sysmail95
#define CP_MAILBOX_SYSMAIL95(n) (((n)&0xffffffff) << 0)

// sysmail96
#define CP_MAILBOX_SYSMAIL96(n) (((n)&0xffffffff) << 0)

// sysmail97
#define CP_MAILBOX_SYSMAIL97(n) (((n)&0xffffffff) << 0)

// sysmail98
#define CP_MAILBOX_SYSMAIL98(n) (((n)&0xffffffff) << 0)

// sysmail99
#define CP_MAILBOX_SYSMAIL99(n) (((n)&0xffffffff) << 0)

// sysmail100
#define CP_MAILBOX_SYSMAIL100(n) (((n)&0xffffffff) << 0)

// sysmail101
#define CP_MAILBOX_SYSMAIL101(n) (((n)&0xffffffff) << 0)

// sysmail102
#define CP_MAILBOX_SYSMAIL102(n) (((n)&0xffffffff) << 0)

// sysmail103
#define CP_MAILBOX_SYSMAIL103(n) (((n)&0xffffffff) << 0)

// sysmail104
#define CP_MAILBOX_SYSMAIL104(n) (((n)&0xffffffff) << 0)

// sysmail105
#define CP_MAILBOX_SYSMAIL105(n) (((n)&0xffffffff) << 0)

// sysmail106
#define CP_MAILBOX_SYSMAIL106(n) (((n)&0xffffffff) << 0)

// sysmail107
#define CP_MAILBOX_SYSMAIL107(n) (((n)&0xffffffff) << 0)

// sysmail108
#define CP_MAILBOX_SYSMAIL108(n) (((n)&0xffffffff) << 0)

// sysmail109
#define CP_MAILBOX_SYSMAIL109(n) (((n)&0xffffffff) << 0)

// sysmail110
#define CP_MAILBOX_SYSMAIL110(n) (((n)&0xffffffff) << 0)

// sysmail111
#define CP_MAILBOX_SYSMAIL111(n) (((n)&0xffffffff) << 0)

// sysmail112
#define CP_MAILBOX_SYSMAIL112(n) (((n)&0xffffffff) << 0)

// sysmail113
#define CP_MAILBOX_SYSMAIL113(n) (((n)&0xffffffff) << 0)

// sysmail114
#define CP_MAILBOX_SYSMAIL114(n) (((n)&0xffffffff) << 0)

// sysmail115
#define CP_MAILBOX_SYSMAIL115(n) (((n)&0xffffffff) << 0)

// sysmail116
#define CP_MAILBOX_SYSMAIL116(n) (((n)&0xffffffff) << 0)

// sysmail117
#define CP_MAILBOX_SYSMAIL117(n) (((n)&0xffffffff) << 0)

// sysmail118
#define CP_MAILBOX_SYSMAIL118(n) (((n)&0xffffffff) << 0)

// sysmail119
#define CP_MAILBOX_SYSMAIL119(n) (((n)&0xffffffff) << 0)

// sysmail120
#define CP_MAILBOX_SYSMAIL120(n) (((n)&0xffffffff) << 0)

// sysmail121
#define CP_MAILBOX_SYSMAIL121(n) (((n)&0xffffffff) << 0)

// sysmail122
#define CP_MAILBOX_SYSMAIL122(n) (((n)&0xffffffff) << 0)

// sysmail123
#define CP_MAILBOX_SYSMAIL123(n) (((n)&0xffffffff) << 0)

// sysmail124
#define CP_MAILBOX_SYSMAIL124(n) (((n)&0xffffffff) << 0)

// sysmail125
#define CP_MAILBOX_SYSMAIL125(n) (((n)&0xffffffff) << 0)

// sysmail126
#define CP_MAILBOX_SYSMAIL126(n) (((n)&0xffffffff) << 0)

// sysmail127
#define CP_MAILBOX_SYSMAIL127(n) (((n)&0xffffffff) << 0)

// sysmail128
#define CP_MAILBOX_SYSMAIL128(n) (((n)&0xffffffff) << 0)

// sysmail129
#define CP_MAILBOX_SYSMAIL129(n) (((n)&0xffffffff) << 0)

// sysmail130
#define CP_MAILBOX_SYSMAIL130(n) (((n)&0xffffffff) << 0)

// sysmail131
#define CP_MAILBOX_SYSMAIL131(n) (((n)&0xffffffff) << 0)

// sysmail132
#define CP_MAILBOX_SYSMAIL132(n) (((n)&0xffffffff) << 0)

// sysmail133
#define CP_MAILBOX_SYSMAIL133(n) (((n)&0xffffffff) << 0)

// sysmail134
#define CP_MAILBOX_SYSMAIL134(n) (((n)&0xffffffff) << 0)

// sysmail135
#define CP_MAILBOX_SYSMAIL135(n) (((n)&0xffffffff) << 0)

// sysmail136
#define CP_MAILBOX_SYSMAIL136(n) (((n)&0xffffffff) << 0)

// sysmail137
#define CP_MAILBOX_SYSMAIL137(n) (((n)&0xffffffff) << 0)

// sysmail138
#define CP_MAILBOX_SYSMAIL138(n) (((n)&0xffffffff) << 0)

// sysmail139
#define CP_MAILBOX_SYSMAIL139(n) (((n)&0xffffffff) << 0)

// sysmail140
#define CP_MAILBOX_SYSMAIL140(n) (((n)&0xffffffff) << 0)

// sysmail141
#define CP_MAILBOX_SYSMAIL141(n) (((n)&0xffffffff) << 0)

// sysmail142
#define CP_MAILBOX_SYSMAIL142(n) (((n)&0xffffffff) << 0)

// sysmail143
#define CP_MAILBOX_SYSMAIL143(n) (((n)&0xffffffff) << 0)

// sysmail144
#define CP_MAILBOX_SYSMAIL144(n) (((n)&0xffffffff) << 0)

// sysmail145
#define CP_MAILBOX_SYSMAIL145(n) (((n)&0xffffffff) << 0)

// sysmail146
#define CP_MAILBOX_SYSMAIL146(n) (((n)&0xffffffff) << 0)

// sysmail147
#define CP_MAILBOX_SYSMAIL147(n) (((n)&0xffffffff) << 0)

// sysmail148
#define CP_MAILBOX_SYSMAIL148(n) (((n)&0xffffffff) << 0)

// sysmail149
#define CP_MAILBOX_SYSMAIL149(n) (((n)&0xffffffff) << 0)

// sysmail150
#define CP_MAILBOX_SYSMAIL150(n) (((n)&0xffffffff) << 0)

// sysmail151
#define CP_MAILBOX_SYSMAIL151(n) (((n)&0xffffffff) << 0)

// sysmail152
#define CP_MAILBOX_SYSMAIL152(n) (((n)&0xffffffff) << 0)

// sysmail153
#define CP_MAILBOX_SYSMAIL153(n) (((n)&0xffffffff) << 0)

// sysmail154
#define CP_MAILBOX_SYSMAIL154(n) (((n)&0xffffffff) << 0)

// sysmail155
#define CP_MAILBOX_SYSMAIL155(n) (((n)&0xffffffff) << 0)

// sysmail156
#define CP_MAILBOX_SYSMAIL156(n) (((n)&0xffffffff) << 0)

// sysmail157
#define CP_MAILBOX_SYSMAIL157(n) (((n)&0xffffffff) << 0)

// sysmail158
#define CP_MAILBOX_SYSMAIL158(n) (((n)&0xffffffff) << 0)

// sysmail159
#define CP_MAILBOX_SYSMAIL159(n) (((n)&0xffffffff) << 0)

// sysmail160
#define CP_MAILBOX_SYSMAIL160(n) (((n)&0xffffffff) << 0)

// sysmail161
#define CP_MAILBOX_SYSMAIL161(n) (((n)&0xffffffff) << 0)

// sysmail162
#define CP_MAILBOX_SYSMAIL162(n) (((n)&0xffffffff) << 0)

// sysmail163
#define CP_MAILBOX_SYSMAIL163(n) (((n)&0xffffffff) << 0)

// sysmail164
#define CP_MAILBOX_SYSMAIL164(n) (((n)&0xffffffff) << 0)

// sysmail165
#define CP_MAILBOX_SYSMAIL165(n) (((n)&0xffffffff) << 0)

// sysmail166
#define CP_MAILBOX_SYSMAIL166(n) (((n)&0xffffffff) << 0)

// sysmail167
#define CP_MAILBOX_SYSMAIL167(n) (((n)&0xffffffff) << 0)

// sysmail168
#define CP_MAILBOX_SYSMAIL168(n) (((n)&0xffffffff) << 0)

// sysmail169
#define CP_MAILBOX_SYSMAIL169(n) (((n)&0xffffffff) << 0)

// sysmail170
#define CP_MAILBOX_SYSMAIL170(n) (((n)&0xffffffff) << 0)

// sysmail171
#define CP_MAILBOX_SYSMAIL171(n) (((n)&0xffffffff) << 0)

// sysmail172
#define CP_MAILBOX_SYSMAIL172(n) (((n)&0xffffffff) << 0)

// sysmail173
#define CP_MAILBOX_SYSMAIL173(n) (((n)&0xffffffff) << 0)

// sysmail174
#define CP_MAILBOX_SYSMAIL174(n) (((n)&0xffffffff) << 0)

// sysmail175
#define CP_MAILBOX_SYSMAIL175(n) (((n)&0xffffffff) << 0)

// sysmail176
#define CP_MAILBOX_SYSMAIL176(n) (((n)&0xffffffff) << 0)

// sysmail177
#define CP_MAILBOX_SYSMAIL177(n) (((n)&0xffffffff) << 0)

// sysmail178
#define CP_MAILBOX_SYSMAIL178(n) (((n)&0xffffffff) << 0)

// sysmail179
#define CP_MAILBOX_SYSMAIL179(n) (((n)&0xffffffff) << 0)

// sysmail180
#define CP_MAILBOX_SYSMAIL180(n) (((n)&0xffffffff) << 0)

// sysmail181
#define CP_MAILBOX_SYSMAIL181(n) (((n)&0xffffffff) << 0)

// sysmail182
#define CP_MAILBOX_SYSMAIL182(n) (((n)&0xffffffff) << 0)

// sysmail183
#define CP_MAILBOX_SYSMAIL183(n) (((n)&0xffffffff) << 0)

// sysmail184
#define CP_MAILBOX_SYSMAIL184(n) (((n)&0xffffffff) << 0)

// sysmail185
#define CP_MAILBOX_SYSMAIL185(n) (((n)&0xffffffff) << 0)

// sysmail186
#define CP_MAILBOX_SYSMAIL186(n) (((n)&0xffffffff) << 0)

// sysmail187
#define CP_MAILBOX_SYSMAIL187(n) (((n)&0xffffffff) << 0)

// sysmail188
#define CP_MAILBOX_SYSMAIL188(n) (((n)&0xffffffff) << 0)

// sysmail189
#define CP_MAILBOX_SYSMAIL189(n) (((n)&0xffffffff) << 0)

// sysmail190
#define CP_MAILBOX_SYSMAIL190(n) (((n)&0xffffffff) << 0)

// sysmail191
#define CP_MAILBOX_SYSMAIL191(n) (((n)&0xffffffff) << 0)

#endif // _CP_MAILBOX_H_
