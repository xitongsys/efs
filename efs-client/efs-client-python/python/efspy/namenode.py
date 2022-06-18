from errno import errorcode
from pydoc import cli
import socket

from . import msg
from .types import Base


class NameNodeConn:
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port

        self.recv_buf = bytearray()

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((ip, port))
        self.alive = True

    def send(self, mg):
        msg_buf = mg.serialize()
        size_buf = Base.serialize_int(len(msg_buf), 4)
        buf = size_buf + msg_buf
        total_size, size = len(buf), 0
        while size < total_size:
            size += self.sock.send(buf[size:])

    def recv(self, mg):
        while self.alive:
            try:
                self.recv_buf += self.sock.recv(1024 * 1024)
            except socket.timeout:
                continue
            except Exception as e:
                raise e

            if len(self.recv_buf) <= 4:
                continue

            msg_size = Base.deserialize_int(self.recv_buf, 4, True)
            if msg_size + 4 > len(self.recv_buf):
                continue

            n = mg.deserialize(self.recv_buf[4:])
            if n != msg_size:
                raise Exception("bad msg")

            self.recv_buf = self.recv_buf[msg_size + 4:]
            return

    def host(self):
        req = msg.MsgHost()
        self.send(req)

        resp = msg.MsgHostResp()
        self.recv(resp)
        return resp

    def account(self):
        req = msg.MsgAccount()
        req.hdesc.token.value = b"abc"
        self.send(req)

        resp = msg.MsgAccountResp()
        self.recv(resp)
        return resp


if __name__ == '__main__':
    conn = NameNodeConn("127.0.0.1", 10000)
    resp = conn.host()
    print(resp.hosts.values, resp.error_code)

    resp = conn.account()
    print(resp.users.values, resp.error_code)
