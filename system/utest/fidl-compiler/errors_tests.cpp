// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unittest/unittest.h>

#include "test_library.h"

namespace {

bool GoodError() {
    BEGIN_TEST;

    TestLibrary library(R"FIDL(
library example;
interface Example {
    Method() -> (int32 flub) error int32;
};
)FIDL");
    ASSERT_TRUE(library.Compile());

    auto methods = &library.LookupInterface("Example")->methods;
    ASSERT_EQ(methods->size(), 1);
    auto method = &methods->at(0);
    auto error_type = method->maybe_error.get();
    ASSERT_NE(error_type, nullptr);
    ASSERT_EQ(error_type->kind, fidl::flat::Type::Kind::kPrimitive);
    auto primitive_type = static_cast<fidl::flat::PrimitiveType*>(error_type);
    ASSERT_EQ(primitive_type->subtype, fidl::types::PrimitiveSubtype::kInt32);

    END_TEST;
}

bool BadErrorMissingType() {
    BEGIN_TEST;

    TestLibrary library(R"FIDL(
library example;
interface Example {
    Method() -> (int32 flub) error;
};
)FIDL");
    ASSERT_FALSE(library.Compile());
    auto errors = library.errors();
    ASSERT_EQ(errors.size(), 1);
    END_TEST;
}

bool BadErrorNotAType() {
    BEGIN_TEST;

    TestLibrary library(R"FIDL(
library example;
interface Example {
    Method() -> (int32 flub) error "hello";
};
)FIDL");
    ASSERT_FALSE(library.Compile());
    auto errors = library.errors();
    ASSERT_EQ(errors.size(), 1);
    END_TEST;
}

bool BadErrorNoResponse() {
    BEGIN_TEST;

    TestLibrary library(R"FIDL(
library example;
interface Example {
    Method() -> error int32;
};
)FIDL");
    ASSERT_FALSE(library.Compile());
    auto errors = library.errors();
    ASSERT_EQ(errors.size(), 1);
    END_TEST;
}
} // namespace

BEGIN_TEST_CASE(errors_tests);

RUN_TEST(GoodError);
RUN_TEST(BadErrorMissingType);
RUN_TEST(BadErrorNotAType);
RUN_TEST(BadErrorNoResponse);

END_TEST_CASE(errors_tests);
