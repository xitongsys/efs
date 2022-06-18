from argparse import FileType
from enum import Enum


class BaseType(Enum):
    default = 0,
    int8 = 1,
    uint8 = 2,
    int16 = 3,
    uint16 = 4,
    int32 = 5,
    uint32 = 6,
    int64 = 7,
    uint64 = 8,
    float = 9,
    double = 10,
    string = 11,


INT_TYPES = [BaseType.int8, BaseType.uint8, BaseType.int16, BaseType.uint16,
             BaseType.int32, BaseType.uint32, BaseType.int64, BaseType.uint64, ]

NUM_TYPES = INT_TYPES + [BaseType.float, BaseType.double]


class Base:
    def serialize_int(v: int, size: int) -> bytearray:
        res = bytearray()
        for i in range(size):
            res.append((v >> (i*8)) & 0xff)
        return res

    def deserialize_int(buf: bytearray, size: int, sign: bool):
        res = 0
        for i in range(size):
            res |= int(buf[i]) << (i * 8)

        if sign and (res >> (size*8-1)) > 0:
            res = ((1 << (size*8)) - res) * -1
        return res

    def __init__(self, t: BaseType, v):
        self.type = t
        self.value = v

    def size(self):
        if self.type == BaseType.int8 or self.type == BaseType.uint8:
            return 1

        elif self.type == BaseType.int16 or self.type == BaseType.uint16:
            return 2

        elif self.type == BaseType.int32 or self.type == BaseType.uint32:
            return 4

        elif self.type == BaseType.int64 or self.type == BaseType.uint64:
            return 8

        elif self.type == BaseType.string:
            return 4 + len(self.value)

        else:
            raise Exception("unknown type: ", self.type)

    def serialize(self) -> bytearray:
        res = bytearray()
        if self.type == BaseType.int8 or self.type == BaseType.uint8:
            res += Base.serialize_int(self.value, 1)

        elif self.type == BaseType.int16 or self.type == BaseType.uint16:
            res += Base.serialize_int(self.value, 2)

        elif self.type == BaseType.int32 or self.type == BaseType.uint32:
            res += Base.serialize_int(self.value, 4)

        elif self.type == BaseType.int64 or self.type == BaseType.uint64:
            res += Base.serialize_int(self.value, 8)

        elif self.type == BaseType.string:
            res += Base.serialize_int(len(self.value), 4)
            res += self.value

        else:
            raise Exception("unknown type: ", self.type)
        return res

    # 0: not full, -1: bad, >0: size of the field
    def deserialize(self, buf):
        if self.type == BaseType.int8 or self.type == BaseType.uint8:
            if len(buf) < 1:
                return 0
            self.value = Base.deserialize_int(
                buf, 1, self.type == BaseType.int8)
            return 1

        elif self.type == BaseType.int16 or self.type == BaseType.uint16:
            if len(buf) < 2:
                return 0
            self.value = Base.deserialize_int(
                buf, 2, self.type == BaseType.int16)
            return 2

        elif self.type == BaseType.int32 or self.type == BaseType.uint32:
            if len(buf) < 4:
                return 0
            self.value = Base.deserialize_int(
                buf, 4, self.type == BaseType.int32)
            return 4

        elif self.type == BaseType.int64 or self.type == BaseType.uint64:
            if len(buf) < 8:
                return 0
            self.value = Base.deserialize_int(
                buf, 8, self.type == BaseType.int64)
            return 8

        elif self.type == BaseType.string:
            if len(buf) < 4:
                return 0
            ln = Base.deserialize_int(buf, 4, True)
            if len(buf) < ln + 4:
                return 0

            self.value = buf[4:4 + ln]
            return ln + 4

        else:
            raise Exception("can't decode")

    def copy(self):
        obj = Base(self.type, self.value)
        return obj

    def __str__(self):
        return f"{self.type},{self.value}"



class Vector:
    def __init__(self, item: any):
        self.item = item
        self.values = []

    def size(self):
        res = 4
        for value in self.values:
            res += value.size()
        return res

    def serialize(self) -> bytearray:
        res = Base.serialize_int(len(self.values), 4)
        for value in self.values:
            res += value.serialize()
        return res

    def deserialize(self, buf: bytearray) -> int:
        if len(buf) < 4:
            return 0
        self.values = []
        n = Base.deserialize_int(buf, 4, True)
        ln = 4
        for i in range(n):
            if ln >= len(buf):
                return 0

            value = self.item.copy()
            c = value.deserialize(buf[ln:])
            if c == 0:
                return 0
            ln += c
            self.values.append(value)
        return ln

    def copy(self):
        obj = Vector(self.item)
        for value in self.values:
            obj.values.append(value.copy())
        return obj
