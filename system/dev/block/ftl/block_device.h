// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <threads.h>

#include <memory>

#include <ddk/protocol/badblock.h>
#include <ddk/protocol/block.h>
#include <ddk/protocol/nand.h>
#include <ddktl/device.h>
#include <ddktl/protocol/badblock.h>
#include <ddktl/protocol/block.h>
#include <fbl/macros.h>
#include <fbl/mutex.h>
#include <lib/ftl/volume.h>
#include <lib/sync/completion.h>
#include <zircon/boot/image.h>
#include <zircon/listnode.h>
#include <zircon/thread_annotations.h>
#include <zircon/types.h>

namespace ftl {

struct BlockParams {
    uint64_t GetSize() const {
        return static_cast<uint64_t>(page_size) * num_pages;
    }

    uint32_t page_size;
    uint32_t num_pages;
};

struct FtlOp;

class BlockDevice;
using DeviceType = ddk::Device<BlockDevice, ddk::GetSizable, ddk::Unbindable, ddk::Ioctlable>;

// Provides the bulk of the functionality for a FTL-backed block device.
class BlockDevice : public DeviceType, public ddk::BlockImplProtocol<BlockDevice>,
                    public ftl::FtlInstance  {
  public:
    explicit BlockDevice(zx_device_t* parent = nullptr) : DeviceType(parent) {}
    ~BlockDevice();

    zx_status_t Bind();
    void DdkRelease() { delete this; }
    void DdkUnbind();

    // Performs the object initialization.
    zx_status_t Init();

    // Device protocol implementation.
    zx_off_t DdkGetSize() { return params_.GetSize(); }
    zx_status_t DdkIoctl(uint32_t op, const void* in_buf, size_t in_len,
                         void* out_buf, size_t out_len, size_t* out_actual);

    // Block protocol implementation.
    void BlockImplQuery(block_info_t* info_out, size_t* block_op_size_out);
    void BlockImplQueue(block_op_t* operation, block_impl_queue_callback completion_cb,
                        void* cookie);

    // FtlInstance interface.
    bool OnVolumeAdded(uint32_t page_size, uint32_t num_pages) final;

    void SetVolumeForTest(std::unique_ptr<ftl::Volume> volume) {
        volume_ = std::move(volume);
    }

    DISALLOW_COPY_ASSIGN_AND_MOVE(BlockDevice);

  private:
    bool InitFtl();
    void Kill();
    bool AddToList(FtlOp* operation);
    bool RemoveFromList(FtlOp** operation);
    int WorkerThread();
    static int WorkerThreadStub(void* arg);

    // Implementation of the actual commands.
    zx_status_t ReadWriteData(block_op_t* operation);
    zx_status_t Flush();

    BlockParams params_ = {};

    fbl::Mutex lock_;
    list_node_t txn_list_ TA_GUARDED(lock_) = {};
    bool dead_ TA_GUARDED(lock_) = false;

    bool thread_created_ = false;
    bool pending_flush_ = false;

    sync_completion_t wake_signal_;
    thrd_t worker_;

    nand_protocol_t parent_ = {};
    bad_block_protocol_t bad_block_ = {};
    std::unique_ptr<ftl::Volume> volume_;
    uint8_t guid_[ZBI_PARTITION_GUID_LEN] = {};
};

}  // namespace ftl.
