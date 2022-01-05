// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

// Copyright (c) 2008-2013, Dave Benson.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Modified to implement C code by Dave Benson.

#include <protoc-c/c_string_field.h>
#include <protoc-c/c_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace c {

using internal::WireFormat;

void SetStringVariables(const FieldDescriptor* descriptor,
                        std::map<string, string>* variables) {
  (*variables)["name"] = FieldName(descriptor);
  (*variables)["default"] = FullNameToLower(descriptor->full_name())
	+ "__default_value";
  (*variables)["deprecated"] = FieldDeprecated(descriptor);
}

// ===================================================================

StringFieldGenerator::
StringFieldGenerator(const FieldDescriptor* descriptor)
  : FieldGenerator(descriptor) {
  SetStringVariables(descriptor, &variables_);
}

StringFieldGenerator::~StringFieldGenerator() {}

void StringFieldGenerator::GenerateStructMembers(io::Printer* printer) const
{
  switch (descriptor_->label()) {
    case FieldDescriptor::LABEL_REQUIRED:
    case FieldDescriptor::LABEL_OPTIONAL:
      printer->Print(variables_, "char *$name$$deprecated$;\n");
      break;
    case FieldDescriptor::LABEL_REPEATED:
      printer->Print(variables_, "size_t n_$name$$deprecated$;\n");
      printer->Print(variables_, "char **$name$$deprecated$;\n");
      break;
  }
}
void StringFieldGenerator::GenerateDefaultValueDeclarations(io::Printer* printer) const
{
  printer->Print(variables_, "extern char $default$[];\n");
}
void StringFieldGenerator::GenerateDefaultValueImplementations(io::Printer* printer) const
{
  std::map<string, string> vars;
  vars["default"] = variables_.find("default")->second;
  vars["escaped"] = CEscape(descriptor_->default_value_string());
  printer->Print(vars, "char $default$[] = \"$escaped$\";\n");
}

string StringFieldGenerator::GetDefaultValue(void) const
{
  return variables_.find("default")->second;
}
void StringFieldGenerator::GenerateStaticInit(io::Printer* printer) const
{
  std::map<string, string> vars;
  if (descriptor_->has_default_value()) {
    vars["default"] = GetDefaultValue();
  } else if (FieldSyntax(descriptor_) == 2) {
    vars["default"] = "NULL";
  } else {
    vars["default"] = "(char *)protobuf_c_empty_string";
  }
  switch (descriptor_->label()) {
    case FieldDescriptor::LABEL_REQUIRED:
    case FieldDescriptor::LABEL_OPTIONAL:
      printer->Print(vars, "$default$");
      break;
    case FieldDescriptor::LABEL_REPEATED:
      printer->Print(vars, "0,NULL");
      break;
  }
}
void StringFieldGenerator::GenerateDescriptorInitializer(io::Printer* printer) const
{
  GenerateDescriptorInitializerGeneric(printer, false, "STRING", "NULL");
}

}  // namespace c
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
