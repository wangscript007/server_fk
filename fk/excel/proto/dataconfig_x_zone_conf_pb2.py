# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: dataconfig_x_zone_conf.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='dataconfig_x_zone_conf.proto',
  package='dataconfig',
  syntax='proto2',
  serialized_pb=_b('\n\x1c\x64\x61taconfig_x_zone_conf.proto\x12\ndataconfig\"\x8e\x01\n\x0bx_zone_conf\x12\r\n\x02id\x18\x01 \x01(\x05:\x01\x30\x12\x17\n\rlocation_desc\x18\x02 \x01(\t:\x00\x12\x0f\n\x04zone\x18\x03 \x01(\x05:\x01\x30\x12\x0f\n\x04name\x18\x04 \x01(\x05:\x01\x30\x12\x0e\n\x04\x64\x65sc\x18\x05 \x01(\t:\x00\x12\x12\n\x07is_open\x18\x06 \x01(\x05:\x01\x30\x12\x11\n\x06\x62\x61\x63kup\x18\x07 \x01(\x05:\x01\x30\";\n\x11x_zone_conf_array\x12&\n\x05items\x18\x01 \x03(\x0b\x32\x17.dataconfig.x_zone_conf')
)




_X_ZONE_CONF = _descriptor.Descriptor(
  name='x_zone_conf',
  full_name='dataconfig.x_zone_conf',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='dataconfig.x_zone_conf.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='location_desc', full_name='dataconfig.x_zone_conf.location_desc', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='zone', full_name='dataconfig.x_zone_conf.zone', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='name', full_name='dataconfig.x_zone_conf.name', index=3,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='desc', full_name='dataconfig.x_zone_conf.desc', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='is_open', full_name='dataconfig.x_zone_conf.is_open', index=5,
      number=6, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='backup', full_name='dataconfig.x_zone_conf.backup', index=6,
      number=7, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=45,
  serialized_end=187,
)


_X_ZONE_CONF_ARRAY = _descriptor.Descriptor(
  name='x_zone_conf_array',
  full_name='dataconfig.x_zone_conf_array',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='items', full_name='dataconfig.x_zone_conf_array.items', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=189,
  serialized_end=248,
)

_X_ZONE_CONF_ARRAY.fields_by_name['items'].message_type = _X_ZONE_CONF
DESCRIPTOR.message_types_by_name['x_zone_conf'] = _X_ZONE_CONF
DESCRIPTOR.message_types_by_name['x_zone_conf_array'] = _X_ZONE_CONF_ARRAY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

x_zone_conf = _reflection.GeneratedProtocolMessageType('x_zone_conf', (_message.Message,), dict(
  DESCRIPTOR = _X_ZONE_CONF,
  __module__ = 'dataconfig_x_zone_conf_pb2'
  # @@protoc_insertion_point(class_scope:dataconfig.x_zone_conf)
  ))
_sym_db.RegisterMessage(x_zone_conf)

x_zone_conf_array = _reflection.GeneratedProtocolMessageType('x_zone_conf_array', (_message.Message,), dict(
  DESCRIPTOR = _X_ZONE_CONF_ARRAY,
  __module__ = 'dataconfig_x_zone_conf_pb2'
  # @@protoc_insertion_point(class_scope:dataconfig.x_zone_conf_array)
  ))
_sym_db.RegisterMessage(x_zone_conf_array)


# @@protoc_insertion_point(module_scope)