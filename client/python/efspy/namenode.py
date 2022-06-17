from errno import errorcode
from pydoc import cli
import socket

from . import msg


class NameNodeConn:
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port

        self.recv_buf = bytearray()

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((ip, port))
        self.alive = True

    def send(self, mg):
        buf = mg.serialize()
        total_size, size = len(buf), 0
        while size < total_size:
            size += self.sock.send(buf[size:])

    def recv(self, mg):
        while self.alive:
            cnt = mg.deserialize(self.recv_buf)
            if cnt == 0:
                try:
                    self.recv_buf += self.sock.recv(1024 * 1024)
                except socket.timeout:
                    continue
                except Exception as e:
                    raise e
                continue

            elif cnt > 0:
                self.recv_buf = self.recv_buf[cnt:]
                break
            else:
                raise Exception("bad message")

    def host(self):
        req = msg.MsgHost()
        self.send(req)

        resp = msg.MsgLsResp()
        self.recv(resp)
        return resp


if __name__ == '__main__':
    conn = NameNodeConn("127.0.0.1", 12345)
    resp = conn.host()
    print(resp.error_code)
