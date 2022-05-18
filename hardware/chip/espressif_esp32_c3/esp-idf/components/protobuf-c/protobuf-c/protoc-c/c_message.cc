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

#include <algorithm>
#include <map>
#include <protoc-c/c_message.h>
#include <protoc-c/c_enum.h>
#include <protoc-c/c_extension.h>
#include <protoc-c/c_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace c {

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor,
                                   const string& dllexport_decl)
  : descriptor_(descriptor),
    dllexport_decl_(dllexport_decl),
    field_generators_(descriptor),
    nested_generators_(new scoped_ptr<MessageGenerator>[
      descriptor->nested_type_count()]),
    enum_generators_(new scoped_ptr<EnumGenerator>[
      descriptor->enum_type_count()]),
    extension_generators_(new scoped_ptr<ExtensionGenerator>[
      descriptor->extension_count()]) {

  for (int i = 0; i < descriptor->nested_type_count(); i++) {
    nested_generators_[i].reset(
      new MessageGenerator(descriptor->nested_type(i), dllexport_decl));
  }

  for (int i = 0; i < descriptor->enum_type_count(); i++) {
    enum_generators_[i].reset(
      new EnumGenerator(descriptor->enum_type(i), dllexport_decl));
  }

  for (int i = 0; i < descriptor->extension_count(); i++) {
    extension_generators_[i].reset(
      new ExtensionGenerator(descriptor->extension(i), dllexport_decl));
  }
}

MessageGenerator::~MessageGenerator() {}

void MessageGenerator::
GenerateStructTypedef(io::Printer* printer) {
  printer->Print("typedef struct _$classname$ $classname$;\n",
                 "classname", FullNameToC(descriptor_->full_name()));

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateStructTypedef(printer);
  }
}

void MessageGenerator::
GenerateEnumDefinitions(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateEnumDefinitions(printer);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateDefinition(printer);
  }
}


void MessageGenerator::
GenerateStructDefinition(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateStructDefinition(printer);
  }

  std::map<string, string> vars;
  vars["classname"] = FullNameToC(descriptor_->full_name());
  vars["lcclassname"] = FullNameToLower(descriptor_->full_name());
  vars["ucclassname"] = FullNameToUpper(descriptor_->full_name());
  vars["field_count"] = SimpleItoa(descriptor_->field_count());
  if (dllexport_decl_.empty()) {
    vars["dllexport"] = "";
  } else {
    vars["dllexport"] = dllexport_decl_ + " ";
  }

  // Generate the case enums for unions
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    const OneofDescriptor *oneof = descriptor_->oneof_decl(i);
    vars["opt_comma"] = ",";

    vars["oneofname"] = FullNameToUpper(oneof->name());
    vars["foneofname"] = FullNameToC(oneof->full_name());

    printer->Print("typedef enum {\n");
    printer->Indent();
    printer->Print(vars, "$ucclassname$__$oneofname$__NOT_SET = 0,\n");
    for (int j = 0; j < oneof->field_count(); j++) {
      const FieldDescriptor *field = oneof->field(j);
      vars["fieldname"] = FullNameToUpper(field->name());
      vars["fieldnum"] = SimpleItoa(field->number());
      bool isLast = j == oneof->field_count() - 1;
      if (isLast) {
        vars["opt_comma"] = "";
      }
      printer->Print(vars, "$ucclassname$__$oneofname$_$fieldname$ = $fieldnum$$opt_comma$\n");
    }
    printer->Print(vars, "  PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE($ucclassname$__$oneofname$)\n");
    printer->Outdent();
    printer->Print(vars, "} $foneofname$Case;\n\n");
  }

  SourceLocation msgSourceLoc;
  descriptor_->GetSourceLocation(&msgSourceLoc);
  PrintComment (printer, msgSourceLoc.leading_comments);

  printer->Print(vars,
    "struct $dllexport$ _$classname$\n"
    "{\n"
    "  ProtobufCMessage base;\n");

  // Generate fields.
  printer->Indent();
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor *field = descriptor_->field(i);
    if (field->containing_oneof() == NULL) {
      SourceLocation fieldSourceLoc;
      field->GetSourceLocation(&fieldSourceLoc);

      PrintComment (printer, fieldSourceLoc.leading_comments);
      PrintComment (printer, fieldSourceLoc.trailing_comments);
      field_generators_.get(field).GenerateStructMembers(printer);
    }
  }

  // Generate unions from oneofs.
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    const OneofDescriptor *oneof = descriptor_->oneof_decl(i);
    vars["oneofname"] = FullNameToLower(oneof->name());
    vars["foneofname"] = FullNameToC(oneof->full_name());

    printer->Print(vars, "$foneofname$Case $oneofname$_case;\n");

    printer->Print("union {\n");
    printer->Indent();
    for (int j = 0; j < oneof->field_count(); j++) {
      const FieldDescriptor *field = oneof->field(j);
      SourceLocation fieldSourceLoc;
      field->GetSourceLocation(&fieldSourceLoc);

      PrintComment (printer, fieldSourceLoc.leading_comments);
      PrintComment (printer, fieldSourceLoc.trailing_comments);
      field_generators_.get(field).GenerateStructMembers(printer);
    }
    printer->Outdent();
    printer->Print(vars, "};\n");
  }
  printer->Outdent();

  printer->Print(vars, "};\n");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor *field = descriptor_->field(i);
    if (field->has_default_value()) {
      field_generators_.get(field).GenerateDefaultValueDeclarations(printer);
    }
  }

  printer->Print(vars, "#define $ucclassname$__INIT \\\n"
		       " { PROTOBUF_C_MESSAGE_INIT (&$lcclassname$__descriptor) \\\n    ");
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor *field = descriptor_->field(i);
    if (field->containing_oneof() == NULL) {
      printer->Print(", ");
      field_generators_.get(field).GenerateStaticInit(printer);
    }
  }
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    const OneofDescriptor *oneof = descriptor_->oneof_decl(i);
    vars["foneofname"] = FullNameToUpper(oneof->full_name());
    // Initialize the case enum
    printer->Print(vars, ", $foneofname$__NOT_SET");
    // Initialize the union
    printer->Print(", {0}");
  }
  printer->Print(" }\n\n\n");

}

void MessageGenerator::
GenerateHelperFunctionDeclarations(io::Printer* printer, bool is_submessage)
{
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateHelperFunctionDeclarations(printer, true);
  }

  std::map<string, string> vars;
  vars["classname"] = FullNameToC(descriptor_->full_name());
  vars["lcclassname"] = FullNameToLower(descriptor_->full_name());
  printer->Print(vars,
		 "/* $classname$ methods */\n"
		 "void   $lcclassname$__init\n"
		 "                     ($classname$         *message);\n"
		);
  if (!is_submessage) {
    printer->Print(vars,
		 "size_t $lcclassname$__get_packed_size\n"
		 "                     (const $classname$   *message);\n"
		 "size_t $lcclassname$__pack\n"
		 "                     (const $classname$   *message,\n"
		 "                      uint8_t             *out);\n"
		 "size_t $lcclassname$__pack_to_buffer\n"
		 "                     (const $classname$   *message,\n"
		 "                      ProtobufCBuffer     *buffer);\n"
		 "$classname$ *\n"
		 "       $lcclassname$__unpack\n"
		 "                     (ProtobufCAllocator  *allocator,\n"
                 "                      size_t               len,\n"
                 "                      const uint8_t       *data);\n"
		 "void   $lcclassname$__free_unpacked\n"
		 "                     ($classname$ *message,\n"
		 "                      ProtobufCAllocator *allocator);\n"
		);
  }
}

void MessageGenerator::
GenerateDescriptorDeclarations(io::Printer* printer) {
  printer->Print("extern const ProtobufCMessageDescriptor $name$__descriptor;\n",
                 "name", FullNameToLower(descriptor_->full_name()));

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDescriptorDeclarations(printer);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateDescriptorDeclarations(printer);
  }
}
void MessageGenerator::GenerateClosureTypedef(io::Printer* printer)
{
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateClosureTypedef(printer);
  }
  std::map<string, string> vars;
  vars["name"] = FullNameToC(descriptor_->full_name());
  printer->Print(vars,
                 "typedef void (*$name$_Closure)\n"
		 "                 (const $name$ *message,\n"
		 "                  void *closure_data);\n");
}

static int
compare_pfields_by_number (const void *a, const void *b)
{
  const FieldDescriptor *pa = *(const FieldDescriptor **)a;
  const FieldDescriptor *pb = *(const FieldDescriptor **)b;
  if (pa->number() < pb->number()) return -1;
  if (pa->number() > pb->number()) return +1;
  return 0;
}

void MessageGenerator::
GenerateHelperFunctionDefinitions(io::Printer* printer, bool is_submessage)
{
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateHelperFunctionDefinitions(printer, true);
  }

  std::map<string, string> vars;
  vars["classname"] = FullNameToC(descriptor_->full_name());
  vars["lcclassname"] = FullNameToLower(descriptor_->full_name());
  vars["ucclassname"] = FullNameToUpper(descriptor_->full_name());
  printer->Print(vars,
		 "void   $lcclassname$__init\n"
		 "                     ($classname$         *message)\n"
		 "{\n"
		 "  static const $classname$ init_value = $ucclassname$__INIT;\n"
		 "  *message = init_value;\n"
		 "}\n");
  if (!is_submessage) {
    printer->Print(vars,
		 "size_t $lcclassname$__get_packed_size\n"
		 "                     (const $classname$ *message)\n"
		 "{\n"
		 "  assert(message->base.descriptor == &$lcclassname$__descriptor);\n"
		 "  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));\n"
		 "}\n"
		 "size_t $lcclassname$__pack\n"
		 "                     (const $classname$ *message,\n"
		 "                      uint8_t       *out)\n"
		 "{\n"
		 "  assert(message->base.descriptor == &$lcclassname$__descriptor);\n"
		 "  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);\n"
		 "}\n"
		 "size_t $lcclassname$__pack_to_buffer\n"
		 "                     (const $classname$ *message,\n"
		 "                      ProtobufCBuffer *buffer)\n"
		 "{\n"
		 "  assert(message->base.descriptor == &$lcclassname$__descriptor);\n"
		 "  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);\n"
		 "}\n"
		 "$classname$ *\n"
		 "       $lcclassname$__unpack\n"
		 "                     (ProtobufCAllocator  *allocator,\n"
		 "                      size_t               len,\n"
                 "                      const uint8_t       *data)\n"
		 "{\n"
		 "  return ($classname$ *)\n"
		 "     protobuf_c_message_unpack (&$lcclassname$__descriptor,\n"
		 "                                allocator, len, data);\n"
		 "}\n"
		 "void   $lcclassname$__free_unpacked\n"
		 "                     ($classname$ *message,\n"
		 "                      ProtobufCAllocator *allocator)\n"
		 "{\n"
		 "  if(!message)\n"
		 "    return;\n"
		 "  assert(message->base.descriptor == &$lcclassname$__descriptor);\n"
		 "  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);\n"
		 "}\n"
		);
  }
}

void MessageGenerator::
GenerateMessageDescriptor(io::Printer* printer) {
    std::map<string, string> vars;
    vars["fullname"] = descriptor_->full_name();
    vars["classname"] = FullNameToC(descriptor_->full_name());
    vars["lcclassname"] = FullNameToLower(descriptor_->full_name());
    vars["shortname"] = ToCamel(descriptor_->name());
    vars["n_fields"] = SimpleItoa(descriptor_->field_count());
    vars["packagename"] = descriptor_->file()->package();

    bool optimize_code_size = descriptor_->file()->options().has_optimize_for() &&
        descriptor_->file()->options().optimize_for() ==
        FileOptions_OptimizeMode_CODE_SIZE;

    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
      nested_generators_[i]->GenerateMessageDescriptor(printer);
    }

    for (int i = 0; i < descriptor_->enum_type_count(); i++) {
      enum_generators_[i]->GenerateEnumDescriptor(printer);
    }

    for (int i = 0; i < descriptor_->field_count(); i++) {
      const FieldDescriptor *fd = descriptor_->field(i);
      if (fd->has_default_value()) {
	field_generators_.get(fd).GenerateDefaultValueImplementations(printer);
      }
    }

    for (int i = 0; i < descriptor_->field_count(); i++) {
      const FieldDescriptor *fd = descriptor_->field(i);
      if (fd->has_default_value()) {

	bool already_defined = false;
	vars["name"] = fd->name();
	vars["lcname"] = CamelToLower(fd->name());
	vars["maybe_static"] = "static ";
	vars["field_dv_ctype_suffix"] = "";
	vars["default_value"] = field_generators_.get(fd).GetDefaultValue();
	switch (fd->cpp_type()) {
	case FieldDescriptor::CPPTYPE_INT32:
	  vars["field_dv_ctype"] = "int32_t";
	  break;
	case FieldDescriptor::CPPTYPE_INT64:
	  vars["field_dv_ctype"] = "int64_t";
	  break;
	case FieldDescriptor::CPPTYPE_UINT32:
	  vars["field_dv_ctype"] = "uint32_t";
	  break;
	case FieldDescriptor::CPPTYPE_UINT64:
	  vars["field_dv_ctype"] = "uint64_t";
	  break;
	case FieldDescriptor::CPPTYPE_FLOAT:
	  vars["field_dv_ctype"] = "float";
	  break;
	case FieldDescriptor::CPPTYPE_DOUBLE:
	  vars["field_dv_ctype"] = "double";
	  break;
	case FieldDescriptor::CPPTYPE_BOOL:
	  vars["field_dv_ctype"] = "protobuf_c_boolean";
	  break;
	  
	case FieldDescriptor::CPPTYPE_MESSAGE:
	  // NOTE: not supported by protobuf
	  vars["maybe_static"] = "";
	  vars["field_dv_ctype"] = "{ ... }";
	  GOOGLE_LOG(DFATAL) << "Messages can't have default values!";
	  break;
	case FieldDescriptor::CPPTYPE_STRING:
	  if (fd->type() == FieldDescriptor::TYPE_BYTES)
	  {
	    vars["field_dv_ctype"] = "ProtobufCBinaryData";
	  }
	  else   /* STRING type */
	  {
	    already_defined = true;
	    vars["maybe_static"] = "";
	    vars["field_dv_ctype"] = "char";
	    vars["field_dv_ctype_suffix"] = "[]";
	  }
	  break;
	case FieldDescriptor::CPPTYPE_ENUM:
	  {
	    const EnumValueDescriptor *vd = fd->default_value_enum();
	    vars["field_dv_ctype"] = FullNameToC(vd->type()->full_name());
	    break;
	  }
	default:
	  GOOGLE_LOG(DFATAL) << "Unknown CPPTYPE";
	  break;
	}
	if (!already_defined)
	  printer->Print(vars, "$maybe_static$const $field_dv_ctype$ $lcclassname$__$lcname$__default_value$field_dv_ctype_suffix$ = $default_value$;\n");
      }
    }

    if ( descriptor_->field_count() ) {
  printer->Print(vars,
	"static const ProtobufCFieldDescriptor $lcclassname$__field_descriptors[$n_fields$] =\n"
	"{\n");
  printer->Indent();
  const FieldDescriptor **sorted_fields = new const FieldDescriptor *[descriptor_->field_count()];
  for (int i = 0; i < descriptor_->field_count(); i++) {
    sorted_fields[i] = descriptor_->field(i);
  }
  qsort (sorted_fields, descriptor_->field_count(),
       sizeof (const FieldDescriptor *), 
       compare_pfields_by_number);
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor *field = sorted_fields[i];
    field_generators_.get(field).GenerateDescriptorInitializer(printer);
  }
  printer->Outdent();
  printer->Print(vars, "};\n");

  if (!optimize_code_size) {
    NameIndex *field_indices = new NameIndex [descriptor_->field_count()];
    for (int i = 0; i < descriptor_->field_count(); i++) {
      field_indices[i].name = sorted_fields[i]->name().c_str();
      field_indices[i].index = i;
    }
    qsort (field_indices, descriptor_->field_count(), sizeof (NameIndex),
        compare_name_indices_by_name);
    printer->Print(vars, "static const unsigned $lcclassname$__field_indices_by_name[] = {\n");
    for (int i = 0; i < descriptor_->field_count(); i++) {
      vars["index"] = SimpleItoa(field_indices[i].index);
      vars["name"] = field_indices[i].name;
      printer->Print(vars, "  $index$,   /* field[$index$] = $name$ */\n");
    }
    printer->Print("};\n");
    delete[] field_indices;
  }

  // create range initializers
  int *values = new int[descriptor_->field_count()];
  for (int i = 0; i < descriptor_->field_count(); i++) {
    values[i] = sorted_fields[i]->number();
  }
  int n_ranges = WriteIntRanges(printer,
				descriptor_->field_count(), values,
				vars["lcclassname"] + "__number_ranges");
  delete [] values;
  delete [] sorted_fields;

  vars["n_ranges"] = SimpleItoa(n_ranges);
    } else {
      /* MS compiler can't handle arrays with zero size and empty
       * initialization list. Furthermore it is an extension of GCC only but
       * not a standard. */
      vars["n_ranges"] = "0";
  printer->Print(vars,
        "#define $lcclassname$__field_descriptors NULL\n"
        "#define $lcclassname$__field_indices_by_name NULL\n"
        "#define $lcclassname$__number_ranges NULL\n");
    }

  printer->Print(vars,
      "const ProtobufCMessageDescriptor $lcclassname$__descriptor =\n"
      "{\n"
      "  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,\n");
  if (optimize_code_size) {
    printer->Print("  NULL,NULL,NULL,NULL, /* CODE_SIZE */\n");
  } else {
    printer->Print(vars,
        "  \"$fullname$\",\n"
        "  \"$shortname$\",\n"
        "  \"$classname$\",\n"
        "  \"$packagename$\",\n");
  }
  printer->Print(vars,
      "  sizeof($classname$),\n"
      "  $n_fields$,\n"
      "  $lcclassname$__field_descriptors,\n");
  if (optimize_code_size) {
    printer->Print("  NULL, /* CODE_SIZE */\n");
  } else {
    printer->Print(vars,
        "  $lcclassname$__field_indices_by_name,\n");
  }
  printer->Print(vars,
      "  $n_ranges$,"
      "  $lcclassname$__number_ranges,\n"
      "  (ProtobufCMessageInit) $lcclassname$__init,\n"
      "  NULL,NULL,NULL    /* reserved[123] */\n"
      "};\n");
}

}  // namespace c
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
