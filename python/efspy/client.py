from pydoc import cli
import socket

from . import msg


class Client:
    def __init__(self, ip: str, port: int, user: str, password: str):
        self.ip = ip
        self.port = port
        self.user = bytes(user, encoding='utf-8')
        self.password = bytes(password, encoding='utf-8')

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
                    self.recv_buf += self.sock.recv(1024)
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

    def login(self):
        req = msg.MsgLogin()
        req.user.value = self.user
        req.password.value = self.password
        self.send(req)

        resp = msg.MsgLoginResp()
        self.recv(resp)
        return resp

    def ls(self, path: str):
        req = msg.MsgLs()
        req.path.value = path
        self.send(req)

        resp = msg.MsgLsResp()
        self.recv(resp)
        return resp


if __name__ == '__main__':
    client = Client("127.0.0.1", 12345, "zxt", "zxt")

    client.login()
