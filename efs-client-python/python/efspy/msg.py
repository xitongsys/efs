from asyncio.events import BaseDefaultEventLoopPolicy
from enum import Enum
from .types import Base, BaseType, Vector


class MsgType:
    DEFAULT = 0
    LOGIN = 1
    LOGIN_RESP = 2

    GET_FILE_DESC = 3
    GET_FILE_DESC_RESP = 4

    OPEN = 5
    OPEN_RESP = 6

    CLOSE = 7
    CLOSE_RESP = 8

    WRITE = 9
    WRITE_RESP = 10

    READ = 11
    READ_RESP = 12

    LS = 13
    LS_RESP = 14

    MKDIR = 15
    MKDIR_RESP = 16

    RM = 17
    RM_RESP = 18

    CHMOD = 19
    CHMOD_RESP = 20
    
    CHOWN = 21
    CHOWN_RESP = 22

    PERM = 23
    PERM_RESP = 24

    ACCOUNT = 25
    ACCOUNT_RESP = 26

    HOST = 27
    HOST_RESP = 28


class UserDesc:
    def __init__(self):
        self.user = Base(BaseType.string, b"")
        self.password = Base(BaseType.string, b"")
        self.uid = Base(BaseType.int16, 0)
        self.gid = Base(BaseType.int16, 0)
        self.root_path = Base(BaseType.string, b"")

        self.fields = [self.user, self.password,
                       self.uid, self.gid, self.root_path]

    def serialize(self) -> bytearray:
        res = bytearray()
        for field in self.fields:
            res += field.serialize()
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
        udesc = UserDesc()
        udesc.user = self.user.copy()
        udesc.password = self.password.copy()
        udesc.uid = self.uid
        udesc.gid = self.gid
        udesc.root_path = self.root_path.copy()

        udesc.fields = [udesc.user, udesc.password,
                        udesc.uid, udesc.gid, udesc.root_path]
        return udesc


class GroupDesc:
    def __init__(self):
        self.group = Base(BaseType.string, b"")
        self.gid = Base(BaseType.int16, 0)

        self.fields = [self.group, self.gid]

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
        gdesc = GroupDesc()
        gdesc.group = self.group.copy()
        gdesc.gid = self.gid

        gdesc.fields = [gdesc.group, gdesc.gid]
        return gdesc


class HostDesc:
    def __init__(self):
        self.name = Base(BaseType.string, b"")
        self.token = Base(BaseType.string, b"")
        self.ip = Base(BaseType.string, b"")
        self.port = Base(BaseType.uint16, 0)
        self.paths = Vector(Base(BaseType.string, b""))

        self.fields = [self.name, self.token, self.ip, self.port, self.paths]

    def serialize(self) -> bytearray:
        res = bytearray()
        for field in self.fields:
            res += field.serialize()
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
        hdesc = HostDesc()
        hdesc.name = self.name.copy()
        hdesc.token = self.token.copy()
        hdesc.ip = self.ip.copy()
        hdesc.port = self.port
        hdesc.paths = self.paths.copy()

        hdesc.fields = [hdesc.name, hdesc.token,
                        hdesc.ip, hdesc.port, hdesc.paths]
        return hdesc


class FileDesc:
    def __init__(self):
        self.path = Base(BaseType.string, b"")
        self.fsize = Base(BaseType.int64, 0)
        self.uid = Base(BaseType.int16, 0)
        self.gid = Base(BaseType.int16, 0)
        self.mod = Base(BaseType.uint16, 0)
        self.create_time = Base(BaseType.int64, 0)
        self.modified_time = Base(BaseType.int64, 0)

        self.fields = [self.path, self.fsize, self.uid, self.gid,
                       self.mod, self.create_time, self.modified_time]

    def serialize(self) -> bytearray:
        res = bytearray()
        for field in self.fields:
            res += field.serialize()
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
        fdesc.path = self.path.copy()
        fdesc.fsize = self.fsize.copy()
        fdesc.uid = self.uid.copy()
        fdesc.gid = self.gid.copy()
        fdesc.mod = self.mod.copy()
        fdesc.create_time = self.create_time.copy()
        fdesc.modified_time = self.modified_time.copy()
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
        self.msg_type.value = MsgType.MKDIR_RESP


class MsgChmod(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CHMOD

        self.path = Base(BaseType.string, b"")
        self.mod = Base(BaseType.uint16, 0b0000000111)

        self.fields += [self.path, self.mod]


class MsgChmodResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CHMOD_RESP


class MsgChown(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CHOWN

        self.path = Base(BaseType.string, b"")
        self.user = Base(BaseType.string, b"")

        self.fields += [self.path, self.user]


class MsgChownResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.CHOWN_RESP


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


class MsgRead(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.READ

        self.fd = Base(BaseType.int32, 0)
        self.read_size = Base(BaseType.int32, 0)

        self.fields += [self.fd, self.read_size]


class MsgReadResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.READ_RESP

        self.fd = Base(BaseType.int32, 0)
        self.data = Base(BaseType.string, b"")

        self.fields += [self.fd, self.data]


class MsgWrite(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.WRITE

        self.fd = Base(BaseType.int32, 0)
        self.data = Base(BaseType.string, b"")

        self.fields += [self.fd, self.data]


class MsgWriteResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.WRITE_RESP

        self.fd = Base(BaseType.int32, 0)
        self.write_size = Base(BaseType.int32, 0)

        self.fields += [self.fd, self.write_size]


class MsgHost(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.HOST


class MsgHostResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.HOST_RESP
        self.hosts = Vector(HostDesc())

        self.fields += [self.hosts]


class MsgAccount(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.ACCOUNT
        self.hdesc = HostDesc()

        self.fields += [self.hdesc]


class MsgAccountResp(Msg):
    def __init__(self):
        Msg.__init__(self)
        self.msg_type.value = MsgType.ACCOUNT_RESP
        self.users = Vector(UserDesc())
        self.groups = Vector(GroupDesc())

        self.fields += [self.users, self.groups]


if __name__ == '__main__':
    pass
