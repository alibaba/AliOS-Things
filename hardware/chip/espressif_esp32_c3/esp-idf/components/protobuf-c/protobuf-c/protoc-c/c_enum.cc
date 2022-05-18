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

#include <set>
#include <map>

#include <protoc-c/c_enum.h>
#include <protoc-c/c_helpers.h>
#include <google/protobuf/io/printer.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace c {

EnumGenerator::EnumGenerator(const EnumDescriptor* descriptor,
                             const string& dllexport_decl)
  : descriptor_(descriptor),
    dllexport_decl_(dllexport_decl) {
}

EnumGenerator::~EnumGenerator() {}

void EnumGenerator::GenerateDefinition(io::Printer* printer) {
  std::map<string, string> vars;
  vars["classname"] = FullNameToC(descriptor_->full_name());
  vars["shortname"] = descriptor_->name();
  vars["uc_name"] = FullNameToUpper(descriptor_->full_name());

  SourceLocation sourceLoc;
  descriptor_->GetSourceLocation(&sourceLoc);
  PrintComment (printer, sourceLoc.leading_comments);

  printer->Print(vars, "typedef enum _$classname$ {\n");
  printer->Indent();

  const EnumValueDescriptor* min_value = descriptor_->value(0);
  const EnumValueDescriptor* max_value = descriptor_->value(0);


  vars["opt_comma"] = ",";
  vars["prefix"] = FullNameToUpper(descriptor_->full_name()) + "__";
  for (int i = 0; i < descriptor_->value_count(); i++) {
    vars["name"] = descriptor_->value(i)->name();
    vars["number"] = SimpleItoa(descriptor_->value(i)->number());
    if (i + 1 == descriptor_->value_count())
      vars["opt_comma"] = "";

    SourceLocation valSourceLoc;
    descriptor_->value(i)->GetSourceLocation(&valSourceLoc);

    PrintComment (printer, valSourceLoc.leading_comments);
    PrintComment (printer, valSourceLoc.trailing_comments);
    printer->Print(vars, "$prefix$$name$ = $number$$opt_comma$\n");

    if (descriptor_->value(i)->number() < min_value->number()) {
      min_value = descriptor_->value(i);
    }
    if (descriptor_->value(i)->number() > max_value->number()) {
      max_value = descriptor_->value(i);
    }
  }

  printer->Print(vars, "  PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE($uc_name$)\n");
  printer->Outdent();
  printer->Print(vars, "} $classname$;\n");
}

void EnumGenerator::GenerateDescriptorDeclarations(io::Printer* printer) {
  std::map<string, string> vars;
  if (dllexport_decl_.empty()) {
    vars["dllexport"] = "";
  } else {
    vars["dllexport"] = dllexport_decl_ + " ";
  }
  vars["classname"] = FullNameToC(descriptor_->full_name());
  vars["lcclassname"] = FullNameToLower(descriptor_->full_name());

  printer->Print(vars,
    "extern $dllexport$const ProtobufCEnumDescriptor    $lcclassname$__descriptor;\n");
}

struct ValueIndex
{
  int value;
  unsigned index;
  unsigned final_index;		/* index in uniqified array of values */
  const char *name;
};
void EnumGenerator::GenerateValueInitializer(io::Printer *printer, int index)
{
  const EnumValueDescriptor *vd = descriptor_->value(index);
  std::map<string, string> vars;
  bool optimize_code_size = descriptor_->file()->options().has_optimize_for() &&
    descriptor_->file()->options().optimize_for() ==
    FileOptions_OptimizeMode_CODE_SIZE;
  vars["enum_value_name"] = vd->name();
  vars["c_enum_value_name"] = FullNameToUpper(descriptor_->full_name()) + "__" + vd->name();
  vars["value"] = SimpleItoa(vd->number());
  if (optimize_code_size)
    printer->Print(vars, "  { NULL, NULL, $value$ }, /* CODE_SIZE */\n");
  else
    printer->Print(vars,
        "  { \"$enum_value_name$\", \"$c_enum_value_name$\", $value$ },\n");
}

static int compare_value_indices_by_value_then_index(const void *a, const void *b)
{
  const ValueIndex *vi_a = (const ValueIndex *) a;
  const ValueIndex *vi_b = (const ValueIndex *) b;
  if (vi_a->value < vi_b->value) return -1;
  if (vi_a->value > vi_b->value) return +1;
  if (vi_a->index < vi_b->index) return -1;
  if (vi_a->index > vi_b->index) return +1;
  return 0;
}

static int compare_value_indices_by_name(const void *a, const void *b)
{
  const ValueIndex *vi_a = (const ValueIndex *) a;
  const ValueIndex *vi_b = (const ValueIndex *) b;
  return strcmp (vi_a->name, vi_b->name);
}

void EnumGenerator::GenerateEnumDescriptor(io::Printer* printer) {
  std::map<string, string> vars;
  vars["fullname"] = descriptor_->full_name();
  vars["lcclassname"] = FullNameToLower(descriptor_->full_name());
  vars["cname"] = FullNameToC(descriptor_->full_name());
  vars["shortname"] = descriptor_->name();
  vars["packagename"] = descriptor_->file()->package();
  vars["value_count"] = SimpleItoa(descriptor_->value_count());

  bool optimize_code_size = descriptor_->file()->options().has_optimize_for() &&
    descriptor_->file()->options().optimize_for() ==
    FileOptions_OptimizeMode_CODE_SIZE;

  // Sort by name and value, dropping duplicate values if they appear later.
  // TODO: use a c++ paradigm for this!
  NameIndex *name_index = new NameIndex[descriptor_->value_count()];
  ValueIndex *value_index = new ValueIndex[descriptor_->value_count()];
  for (int j = 0; j < descriptor_->value_count(); j++) {
    const EnumValueDescriptor *vd = descriptor_->value(j);
    name_index[j].index = j;
    name_index[j].name = vd->name().c_str();
    value_index[j].index = j;
    value_index[j].value = vd->number();
    value_index[j].name = vd->name().c_str();
  }
  qsort(value_index, descriptor_->value_count(),
	sizeof(ValueIndex), compare_value_indices_by_value_then_index);

  // only record unique values
  int n_unique_values;
  if (descriptor_->value_count() == 0) {
    n_unique_values = 0; // should never happen
  } else {
    n_unique_values = 1;
    value_index[0].final_index = 0;
    for (int j = 1; j < descriptor_->value_count(); j++) {
      if (value_index[j-1].value != value_index[j].value)
	value_index[j].final_index = n_unique_values++;
      else
	value_index[j].final_index = n_unique_values - 1;
    }
  }

  vars["unique_value_count"] = SimpleItoa(n_unique_values);
  printer->Print(vars,
      "static const ProtobufCEnumValue $lcclassname$__enum_values_by_number[$unique_value_count$] =\n"
      "{\n");
  if (descriptor_->value_count() > 0) {
    GenerateValueInitializer(printer, value_index[0].index);
    for (int j = 1; j < descriptor_->value_count(); j++) {
      if (value_index[j-1].value != value_index[j].value) {
        GenerateValueInitializer(printer, value_index[j].index);
      }
    }
  }
  printer->Print(vars, "};\n");
  printer->Print(vars, "static const ProtobufCIntRange $lcclassname$__value_ranges[] = {\n");
  unsigned n_ranges = 0;
  if (descriptor_->value_count() > 0) {
    unsigned range_start = 0;
    unsigned range_len = 1;
    int range_start_value = value_index[0].value;
    int last_value = range_start_value;
    for (int j = 1; j < descriptor_->value_count(); j++) {
      if (value_index[j-1].value != value_index[j].value) {
        if (last_value + 1 == value_index[j].value) {
          range_len++;
        } else {
          // output range
          vars["range_start_value"] = SimpleItoa(range_start_value);
          vars["orig_index"] = SimpleItoa(range_start);
          printer->Print (vars, "{$range_start_value$, $orig_index$},");
          range_start_value = value_index[j].value;
          range_start += range_len;
          range_len = 1;
          n_ranges++;
        }
        last_value = value_index[j].value;
      }
    }
    {
      vars["range_start_value"] = SimpleItoa(range_start_value);
      vars["orig_index"] = SimpleItoa(range_start);
      printer->Print (vars, "{$range_start_value$, $orig_index$},");
      range_start += range_len;
      n_ranges++;
    }
    {
      vars["range_start_value"] = SimpleItoa(0);
      vars["orig_index"] = SimpleItoa(range_start);
      printer->Print (vars, "{$range_start_value$, $orig_index$}\n};\n");
    }
  }
  vars["n_ranges"] = SimpleItoa(n_ranges);

  if (!optimize_code_size) {
    qsort(value_index, descriptor_->value_count(),
        sizeof(ValueIndex), compare_value_indices_by_name);
    printer->Print(vars,
        "static const ProtobufCEnumValueIndex $lcclassname$__enum_values_by_name[$value_count$] =\n"
        "{\n");
    for (int j = 0; j < descriptor_->value_count(); j++) {
      vars["index"] = SimpleItoa(value_index[j].final_index);
      vars["name"] = value_index[j].name;
      printer->Print (vars, "  { \"$name$\", $index$ },\n");
    }
    printer->Print(vars, "};\n");
  }

  if (optimize_code_size) {
    printer->Print(vars,
        "const ProtobufCEnumDescriptor $lcclassname$__descriptor =\n"
        "{\n"
        "  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,\n"
        "  NULL,NULL,NULL,NULL, /* CODE_SIZE */\n"
        "  $unique_value_count$,\n"
        "  $lcclassname$__enum_values_by_number,\n"
        "  0, NULL, /* CODE_SIZE */\n"
        "  $n_ranges$,\n"
        "  $lcclassname$__value_ranges,\n"
        "  NULL,NULL,NULL,NULL   /* reserved[1234] */\n"
        "};\n");
  } else {
    printer->Print(vars,
        "const ProtobufCEnumDescriptor $lcclassname$__descriptor =\n"
        "{\n"
        "  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,\n"
        "  \"$fullname$\",\n"
        "  \"$shortname$\",\n"
        "  \"$cname$\",\n"
        "  \"$packagename$\",\n"
        "  $unique_value_count$,\n"
        "  $lcclassname$__enum_values_by_number,\n"
        "  $value_count$,\n"
        "  $lcclassname$__enum_values_by_name,\n"
        "  $n_ranges$,\n"
        "  $lcclassname$__value_ranges,\n"
        "  NULL,NULL,NULL,NULL   /* reserved[1234] */\n"
        "};\n");
  }

  delete[] value_index;
  delete[] name_index;
}



}  // namespace c
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
