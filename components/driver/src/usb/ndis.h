/*
 * ndis.h
 *
 * ntddndis.h modified by Benedikt Spranger <b.spranger@pengutronix.de>
 *
 * Thanks to the cygwin development team,
 * espacially to Casper S. Hornstrup <chorns@users.sourceforge.net>
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 */

#ifndef _NDIS_H
#define _NDIS_H

enum NDIS_DEVICE_POWER_STATE
{
    NdisDeviceStateUnspecified = 0,
    NdisDeviceStateD0,
    NdisDeviceStateD1,
    NdisDeviceStateD2,
    NdisDeviceStateD3,
    NdisDeviceStateMaximum
};

struct NDIS_PM_WAKE_UP_CAPABILITIES
{
    enum NDIS_DEVICE_POWER_STATE MinMagicPacketWakeUp;
    enum NDIS_DEVICE_POWER_STATE MinPatternWakeUp;
    enum NDIS_DEVICE_POWER_STATE MinLinkChangeWakeUp;
};

struct NDIS_PNP_CAPABILITIES
{
    uint32_t Flags;
    struct NDIS_PM_WAKE_UP_CAPABILITIES WakeUpCapabilities;
};

struct NDIS_PM_PACKET_PATTERN
{
    uint32_t Priority;
    uint32_t Reserved;
    uint32_t MaskSize;
    uint32_t PatternOffset;
    uint32_t PatternSize;
    uint32_t PatternFlags;
};

#endif /* _LINUX_NDIS_H */
