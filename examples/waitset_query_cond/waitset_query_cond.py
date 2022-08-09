
# WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

# This file was generated from waitset_query_cond.idl
# using RTI Code Generator (rtiddsgen) version 4.0.0.
# The rtiddsgen tool is part of the RTI Connext DDS distribution.
# For more information, type 'rtiddsgen -help' at a command shell
# or consult the Code Generator User's Manual.

from dataclasses import field
from typing import Union, Sequence, Optional
import rti.idl as idl
from enum import IntEnum


@idl.struct(
    member_annotations = {
        'name': [idl.key, idl.bound(255)],
    }
)
class waitset_query_cond:
    x: idl.int16 = 0
    name: str = ""
