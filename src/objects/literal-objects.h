// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_LITERAL_OBJECTS_H_
#define V8_OBJECTS_LITERAL_OBJECTS_H_

#include "src/base/bit-field.h"
#include "src/objects/fixed-array.h"
#include "src/objects/struct.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

class ClassLiteral;
class StructBodyDescriptor;

#include "torque-generated/src/objects/literal-objects-tq.inc"

class ObjectBoilerplateDescriptionShape final : public AllStatic {
 public:
  static constexpr int kElementSize = kTaggedSize;
  using ElementT = Object;
  static constexpr RootIndex kMapRootIndex =
      RootIndex::kObjectBoilerplateDescriptionMap;
  static constexpr bool kLengthEqualsCapacity = true;

#define FIELD_LIST(V)                                                   \
  V(kCapacityOffset, kTaggedSize)                                       \
  V(kBackingStoreSizeOffset, kTaggedSize)                               \
  V(kFlagsOffset, kTaggedSize)                                          \
  V(kUnalignedHeaderSize, OBJECT_POINTER_PADDING(kUnalignedHeaderSize)) \
  V(kHeaderSize, 0)
  DEFINE_FIELD_OFFSET_CONSTANTS(HeapObject::kHeaderSize, FIELD_LIST)
#undef FIELD_LIST
};

// ObjectBoilerplateDescription is a list of properties consisting of name
// value pairs. In addition to the properties, it provides the projected number
// of properties in the backing store. This number includes properties with
// computed names that are not in the list.
class ObjectBoilerplateDescription
    : public TaggedArrayBase<ObjectBoilerplateDescription,
                             ObjectBoilerplateDescriptionShape> {
  using Super = TaggedArrayBase<ObjectBoilerplateDescription,
                                ObjectBoilerplateDescriptionShape>;
  OBJECT_CONSTRUCTORS(ObjectBoilerplateDescription, Super);

 public:
  using Shape = ObjectBoilerplateDescriptionShape;

  template <class IsolateT>
  static inline Handle<ObjectBoilerplateDescription> New(
      IsolateT* isolate, int boilerplate, int all_properties, int index_keys,
      bool has_seen_proto, AllocationType allocation = AllocationType::kYoung);

  // ObjectLiteral::Flags for nested object literals.
  inline int flags() const;
  inline void set_flags(int value);

  // Number of boilerplate properties and properties with computed names.
  inline int backing_store_size() const;
  inline void set_backing_store_size(int backing_store_size);

  inline int boilerplate_properties_count() const;

  inline Tagged<Object> name(int index) const;
  inline Tagged<Object> value(int index) const;

  inline void set_key_value(int index, Tagged<Object> key,
                            Tagged<Object> value);

  DECL_CAST(ObjectBoilerplateDescription)
  DECL_VERIFIER(ObjectBoilerplateDescription)
  DECL_PRINTER(ObjectBoilerplateDescription)

  class BodyDescriptor;

 private:
  static constexpr int kElementsPerEntry = 2;
  static constexpr int NameIndex(int i) { return i * kElementsPerEntry; }
  static constexpr int ValueIndex(int i) { return i * kElementsPerEntry + 1; }
};

class ArrayBoilerplateDescription
    : public TorqueGeneratedArrayBoilerplateDescription<
          ArrayBoilerplateDescription, Struct> {
 public:
  inline ElementsKind elements_kind() const;
  inline void set_elements_kind(ElementsKind kind);

  inline bool is_empty() const;

  // Dispatched behavior.
  DECL_PRINTER(ArrayBoilerplateDescription)
  void BriefPrintDetails(std::ostream& os);

  using BodyDescriptor = StructBodyDescriptor;

 private:
  TQ_OBJECT_CONSTRUCTORS(ArrayBoilerplateDescription)
};

class RegExpBoilerplateDescription
    : public TorqueGeneratedRegExpBoilerplateDescription<
          RegExpBoilerplateDescription, Struct> {
 public:
  // Dispatched behavior.
  void BriefPrintDetails(std::ostream& os);

  using BodyDescriptor = StructBodyDescriptor;

 private:
  TQ_OBJECT_CONSTRUCTORS(RegExpBoilerplateDescription)
};

class ClassBoilerplate : public FixedArray {
 public:
  enum ValueKind { kData, kGetter, kSetter };

  struct ComputedEntryFlags {
#define COMPUTED_ENTRY_BIT_FIELDS(V, _) \
  V(ValueKindBits, ValueKind, 2, _)     \
  V(KeyIndexBits, unsigned, 29, _)
    DEFINE_BIT_FIELDS(COMPUTED_ENTRY_BIT_FIELDS)
#undef COMPUTED_ENTRY_BIT_FIELDS
  };

  enum DefineClassArgumentsIndices {
    kConstructorArgumentIndex = 1,
    kPrototypeArgumentIndex = 2,
    // The index of a first dynamic argument passed to Runtime::kDefineClass
    // function. The dynamic arguments are consist of method closures and
    // computed property names.
    kFirstDynamicArgumentIndex = 3,
  };

  static const int kMinimumClassPropertiesCount = 6;
  static const int kMinimumPrototypePropertiesCount = 1;

  DECL_CAST(ClassBoilerplate)

  DECL_BOOLEAN_ACCESSORS(install_class_name_accessor)
  DECL_INT_ACCESSORS(arguments_count)
  DECL_ACCESSORS(static_properties_template, Tagged<Object>)
  DECL_ACCESSORS(static_elements_template, Tagged<Object>)
  DECL_ACCESSORS(static_computed_properties, Tagged<FixedArray>)
  DECL_ACCESSORS(instance_properties_template, Tagged<Object>)
  DECL_ACCESSORS(instance_elements_template, Tagged<Object>)
  DECL_ACCESSORS(instance_computed_properties, Tagged<FixedArray>)

  template <typename IsolateT, typename Dictionary>
  static void AddToPropertiesTemplate(IsolateT* isolate,
                                      Handle<Dictionary> dictionary,
                                      Handle<Name> name, int key_index,
                                      ValueKind value_kind, Tagged<Smi> value);

  template <typename IsolateT>
  static void AddToElementsTemplate(IsolateT* isolate,
                                    Handle<NumberDictionary> dictionary,
                                    uint32_t key, int key_index,
                                    ValueKind value_kind, Tagged<Smi> value);

  template <typename IsolateT>
  static Handle<ClassBoilerplate> BuildClassBoilerplate(IsolateT* isolate,
                                                        ClassLiteral* expr);

  enum {
    kArgumentsCountIndex,
    kClassPropertiesTemplateIndex,
    kClassElementsTemplateIndex,
    kClassComputedPropertiesIndex,
    kPrototypePropertiesTemplateIndex,
    kPrototypeElementsTemplateIndex,
    kPrototypeComputedPropertiesIndex,
    kBoilerplateLength  // last element
  };

 private:
  DECL_INT_ACCESSORS(flags)

  OBJECT_CONSTRUCTORS(ClassBoilerplate, FixedArray);
};

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_LITERAL_OBJECTS_H_
