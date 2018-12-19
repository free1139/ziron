// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZIRCON_SYSTEM_HOST_FIDL_INCLUDE_FIDL_VIRTUAL_SOURCE_FILE_H_
#define ZIRCON_SYSTEM_HOST_FIDL_INCLUDE_FIDL_VIRTUAL_SOURCE_FILE_H_

#include <string>
#include <utility>
#include <vector>

#include "source_file.h"

namespace fidl {

class VirtualSourceFile : public SourceFile {
public:
    VirtualSourceFile(std::string filename) : SourceFile(filename, "") {}
    virtual ~VirtualSourceFile() = default;

    virtual StringView LineContaining(StringView view, Position* position_out) const;

    StringView AddLine(const std::string& line);

private:
    std::vector<std::string> lines_;
};

} // namespace fidl

#endif // ZIRCON_SYSTEM_HOST_FIDL_INCLUDE_FIDL_VIRTUAL_SOURCE_FILE_H_
