// Generated by the protocol buffer compiler.  DO NOT EDIT!

#include "nymble_credential.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format_inl.h>
namespace Nymble { namespace Marshal { void protobuf_BuildDesc_nymble_5fticket_2eproto(); } }

namespace Nymble {
namespace Marshal {

namespace {

const ::google::protobuf::Descriptor* Credential_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Credential_reflection_ = NULL;

}  // namespace


void protobuf_BuildDesc_nymble_5fcredential_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  ::google::protobuf::DescriptorPool* pool =
    ::google::protobuf::DescriptorPool::internal_generated_pool();

  ::Nymble::Marshal::protobuf_BuildDesc_nymble_5fticket_2eproto();
  const ::google::protobuf::FileDescriptor* file = pool->InternalBuildGeneratedFile(
    "\n\027nymble_credential.proto\022\016Nymble.Marsha"
    "l\032\023nymble_ticket.proto\"E\n\nCredential\022\017\n\007"
    "nymble0\030\001 \002(\014\022&\n\006ticket\030\002 \003(\0132\026.Nymble.M"
    "arshal.Ticket", 133);
  Credential_descriptor_ = file->message_type(0);
  Credential_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Credential_descriptor_,
      &Credential::default_instance(),
      Credential::_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Credential, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Credential, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Credential_descriptor_, &Credential::default_instance());
}

// Force BuildDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_nymble_5fcredential_2eproto {
  StaticDescriptorInitializer_nymble_5fcredential_2eproto() {
    protobuf_BuildDesc_nymble_5fcredential_2eproto();
  }
} static_descriptor_initializer_nymble_5fcredential_2eproto_;


// ===================================================================

const Credential Credential::default_instance_;

const ::std::string Credential::_default_nymble0_;

const int Credential::_offsets_[2] = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Credential, nymble0_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Credential, ticket_),
};

Credential::Credential()
  : _cached_size_(0),
    nymble0_(const_cast< ::std::string*>(&_default_nymble0_)) {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (this == &default_instance_) {
  }
}

Credential::Credential(const Credential& from)
  : _cached_size_(0),
    nymble0_(const_cast< ::std::string*>(&_default_nymble0_)) {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  MergeFrom(from);
}

Credential::~Credential() {
  if (nymble0_ != &_default_nymble0_) {
    delete nymble0_;
  }
  if (this != &default_instance_) {
  }
}

const ::google::protobuf::Descriptor* Credential::descriptor() {
  if (Credential_descriptor_ == NULL) protobuf_BuildDesc_nymble_5fcredential_2eproto();
  return Credential_descriptor_;
}

Credential* Credential::New() const {
  return new Credential;
}

const ::google::protobuf::Descriptor* Credential::GetDescriptor() const {
  return descriptor();
}

const ::google::protobuf::Reflection* Credential::GetReflection() const {
  if (Credential_reflection_ == NULL) protobuf_BuildDesc_nymble_5fcredential_2eproto();
  return Credential_reflection_;
}

}  // namespace Marshal
}  // namespace Nymble
