// Generated by the protocol buffer compiler.  DO NOT EDIT!

#include "nymble_linking_token.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format_inl.h>

namespace Nymble {
namespace Marshal {

namespace {

const ::google::protobuf::Descriptor* LinkingToken_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  LinkingToken_reflection_ = NULL;

}  // namespace


void protobuf_BuildDesc_nymble_5flinking_5ftoken_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  ::google::protobuf::DescriptorPool* pool =
    ::google::protobuf::DescriptorPool::internal_generated_pool();

  const ::google::protobuf::FileDescriptor* file = pool->InternalBuildGeneratedFile(
    "\n\032nymble_linking_token.proto\022\016Nymble.Mar"
    "shal\" \n\014LinkingToken\022\020\n\010trapdoor\030\001 \002(\014", 78);
  LinkingToken_descriptor_ = file->message_type(0);
  LinkingToken_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      LinkingToken_descriptor_,
      &LinkingToken::default_instance(),
      LinkingToken::_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LinkingToken, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LinkingToken, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    LinkingToken_descriptor_, &LinkingToken::default_instance());
}

// Force BuildDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_nymble_5flinking_5ftoken_2eproto {
  StaticDescriptorInitializer_nymble_5flinking_5ftoken_2eproto() {
    protobuf_BuildDesc_nymble_5flinking_5ftoken_2eproto();
  }
} static_descriptor_initializer_nymble_5flinking_5ftoken_2eproto_;


// ===================================================================

const LinkingToken LinkingToken::default_instance_;

const ::std::string LinkingToken::_default_trapdoor_;
const int LinkingToken::_offsets_[1] = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LinkingToken, trapdoor_),
};

LinkingToken::LinkingToken()
  : _cached_size_(0),
    trapdoor_(const_cast< ::std::string*>(&_default_trapdoor_)) {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (this == &default_instance_) {
  }
}

LinkingToken::LinkingToken(const LinkingToken& from)
  : _cached_size_(0),
    trapdoor_(const_cast< ::std::string*>(&_default_trapdoor_)) {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  MergeFrom(from);
}

LinkingToken::~LinkingToken() {
  if (trapdoor_ != &_default_trapdoor_) {
    delete trapdoor_;
  }
  if (this != &default_instance_) {
  }
}

const ::google::protobuf::Descriptor* LinkingToken::descriptor() {
  if (LinkingToken_descriptor_ == NULL) protobuf_BuildDesc_nymble_5flinking_5ftoken_2eproto();
  return LinkingToken_descriptor_;
}

LinkingToken* LinkingToken::New() const {
  return new LinkingToken;
}

const ::google::protobuf::Descriptor* LinkingToken::GetDescriptor() const {
  return descriptor();
}

const ::google::protobuf::Reflection* LinkingToken::GetReflection() const {
  if (LinkingToken_reflection_ == NULL) protobuf_BuildDesc_nymble_5flinking_5ftoken_2eproto();
  return LinkingToken_reflection_;
}

}  // namespace Marshal
}  // namespace Nymble