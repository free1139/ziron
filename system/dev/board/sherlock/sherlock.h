// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <threads.h>

#include <ddk/device.h>
#include <ddk/protocol/gpioimpl.h>

#include <ddktl/device.h>
#include <ddktl/protocol/iommu.h>
#include <ddktl/protocol/platform/bus.h>

#include <fbl/macros.h>

#include <soc/aml-t931/t931-hw.h>

namespace sherlock {

// BTI IDs for our devices
enum {
    BTI_BOARD,
    BTI_USB_XHCI,
    BTI_EMMC,
    BTI_SDIO,
    BTI_MALI,
    BTI_CANVAS,
    BTI_VIDEO,
    BTI_CAMERA,
    BTI_DISPLAY,
    BTI_AUDIO_OUT,
    BTI_AUDIO_IN,
};

// MAC address metadata indices
enum {
    MACADDR_WIFI = 0,
    MACADDR_BLUETOOTH = 1,
};

// These should match the mmio table defined in sherlock-i2c.c
enum {
    SHERLOCK_I2C_A0_0,
    SHERLOCK_I2C_2,
    SHERLOCK_I2C_3,
};

class Sherlock;
using SherlockType = ddk::Device<Sherlock>;

// This is the main class for the platform bus driver.
class Sherlock : public SherlockType {
public:
    explicit Sherlock(zx_device_t* parent, pbus_protocol_t* pbus, iommu_protocol_t* iommu)
        : SherlockType(parent), pbus_(pbus), iommu_(iommu) {}

    static zx_status_t Create(zx_device_t* parent);

    // Device protocol implementation.
    void DdkRelease();

private:
    DISALLOW_COPY_ASSIGN_AND_MOVE(Sherlock);

    zx_status_t Start();
    zx_status_t GpioInit();
    zx_status_t CanvasInit();
    zx_status_t I2cInit();
    zx_status_t UsbInit();
    zx_status_t EmmcInit();
    zx_status_t BCM43458LpoClockInit(); // required for BCM43458 wifi/bluetooth chip.
    zx_status_t SdioInit();
    zx_status_t BluetoothInit();
    zx_status_t ClkInit();
    zx_status_t CameraInit();
    zx_status_t MaliInit();
    zx_status_t VideoInit();
    zx_status_t BacklightInit();
    zx_status_t ButtonsInit();
    zx_status_t DisplayInit();
    zx_status_t AudioInit();
    int Thread();

    ddk::PBusProtocolProxy pbus_;
    ddk::IommuProtocolProxy iommu_;
    gpio_impl_protocol_t gpio_impl_;
    thrd_t thread_;
};

} // namespace sherlock

__BEGIN_CDECLS
zx_status_t sherlock_bind(void* ctx, zx_device_t* parent);
__END_CDECLS
