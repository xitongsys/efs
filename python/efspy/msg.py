from enum import Enum
from .types import Base, BaseType, Vector


class MsgType:
    DEFAULT = 0
    LOGIN = 1
    LOGIN_RESP = 2
    OPEN = 3
    OPEN_RESP = 4
    CLOSE = 5
    CLOSE_RESP = 6
    WRITE = 7
    WRITE_RESP = 8
    READ = 9
    READ_RESP = 10
    LS = 11
    LS_RESP = 12
    MKDIR = 13
    MKDIR_RESP = 14
    RM = 15
    RM_RESP = 16
    CHMOD = 17
    CHMOD_RESP = 18
    CHOWN = 19
    CHOWN_RESP = 20


class FileDesc:
    def __init__(self):
        self.path = Base(BaseType.string, "")
        self.fsize = Base(BaseType.int64, 0)
        self.uid = Base(BaseType.int32, 0)
        self.gid = Base(BaseType.int32, 0)
        self.mod = Base(BaseType.uint16, 0)
        self.create_time = Base(BaseType.int64, 0)
        self.modified_time = Base(BaseType.int64, 0)

        self.fields = [self.path, self.fsize, self.uid, self.gid,
                       self.mod, self.create_time, self.modified_time]

    def serialize(self) -> bytearray:
        res = bytearray()
        for field in self.fields:
            res += field.encode()
        return res

    def deserialize(self, buf: bytearray) -> int:
        ln = 0
        for field in self.fields:
            if ln >= len(buf):
                return 0
            c = field.deserialize(buf[ln:])
            if c == 0:
                return 0
            ln += c
        return ln

    def size(self) -> int:
        res = 0
        for field in self.fields:
            res += field.size()
        return res

    def copy(self):
        fdesc = FileDesc()
        fdesc.path = self.path
        fdesc.fsize = self.fsize
        fdesc.uid = self.uid
        fdesc.gid = self.gid
        fdesc.mod = self.mod
        fdesc.create_time = self.create_time
        fdesc.modified_time = self.modified_time
        fdesc.fields = [fdesc.path, fdesc.fsize, fdesc.uid, fdesc.gid,
                        fdesc.mod, fdesc.create_time, fdesc.modified_time]
        return fdesc


class Msg:
    def __init__(self):
        self.msg_type = Base(BaseType.int8, MsgType.DEFAULT)
        self.error_code = Base(BaseType.int8, 0)
        self.fields = [self.msg_type, self.error_code]

    def serialize(self) -> bytearray:
        res = bytearray()
        for field in self.fields:
            res += field.serialize()
        return res

    # 0: not full  -1: bad   >0: size of the msg
    def deserialize(self, buf: bytearray) -> int:
        ln = 0
        for field in self.fields:
            if ln >= len(buf):
                return 0
            c = field.deserialize(buf[ln:])
            if c == 0:
                return 0
            ln += c
        return ln

    def size(self):
        res = 0
        for field in self.fields:
            res += field.size()
        return res

    def __str__(self) -> str:
        res = ""
        for field in self.fields:
            res += field.__str__() + "\n"
        return res


class MsgLogin(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.LOGIN

        self.user = Base(BaseType.string, b"")
        self.password = Base(BaseType.string, b"")

        self.fields += [self.user, self.password]


class MsgLoginResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.LOGIN_RESP


class MsgLs(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.LS

        self.path = Base(BaseType.string, b"")

        self.fields += [self.path]


class MsgLsResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.LS_RESP

        self.files = Vector(FileDesc())

        self.fields += [self.files]


class MsgRm(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.RM

        self.path = Base(BaseType.string, b"")

        self.fields += [self.path]


class MsgRmResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.RM_RESP


class MsgMkdir(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.MKDIR

        self.path = Base(BaseType.string, b"")

        self.fields += [self.path]


class MsgMkdirResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.MKDIR


class MsgOpen(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.OPEN

        self.path = Base(BaseType.string, b"")
        self.open_mod = Base(BaseType.string, b"")

        self.fields += [self.path, self.open_mod]


class MsgOpenResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.OPEN_RESP

        self.fd = Base(BaseType.int32, 0)

        self.fields += [self.fd]


class MsgClose(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CLOSE

        self.fd = Base(BaseType.int32, 0)

        self.fields += [self.fd]


class MsgCloseResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CLOSE_RESP


if __name__ == '__main__':
    pass
