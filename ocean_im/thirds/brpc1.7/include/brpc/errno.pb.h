// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: brpc/errno.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_brpc_2ferrno_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_brpc_2ferrno_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3018000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3018003 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_brpc_2ferrno_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_brpc_2ferrno_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_brpc_2ferrno_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace brpc {

enum Errno : int {
  ENOSERVICE = 1001,
  ENOMETHOD = 1002,
  EREQUEST = 1003,
  ERPCAUTH = 1004,
  ETOOMANYFAILS = 1005,
  EPCHANFINISH = 1006,
  EBACKUPREQUEST = 1007,
  ERPCTIMEDOUT = 1008,
  EFAILEDSOCKET = 1009,
  EHTTP = 1010,
  EOVERCROWDED = 1011,
  ERTMPPUBLISHABLE = 1012,
  ERTMPCREATESTREAM = 1013,
  EEOF = 1014,
  EUNUSED = 1015,
  ESSL = 1016,
  EH2RUNOUTSTREAMS = 1017,
  EREJECT = 1018,
  EINTERNAL = 2001,
  ERESPONSE = 2002,
  ELOGOFF = 2003,
  ELIMIT = 2004,
  ECLOSE = 2005,
  EITP = 2006,
  ERDMA = 3001,
  ERDMAMEM = 3002
};
bool Errno_IsValid(int value);
constexpr Errno Errno_MIN = ENOSERVICE;
constexpr Errno Errno_MAX = ERDMAMEM;
constexpr int Errno_ARRAYSIZE = Errno_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Errno_descriptor();
template<typename T>
inline const std::string& Errno_Name(T enum_t_value) {
  static_assert(::std::is_same<T, Errno>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function Errno_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    Errno_descriptor(), enum_t_value);
}
inline bool Errno_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, Errno* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<Errno>(
    Errno_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace brpc

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::brpc::Errno> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::Errno>() {
  return ::brpc::Errno_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_brpc_2ferrno_2eproto
