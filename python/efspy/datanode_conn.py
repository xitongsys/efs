from pydoc import cli
import socket

from . import msg


class DataNodeConn:
    def __init__(self, ip: str, port: int, user: str, password: str):
        self.ip = ip
        self.port = port
        self.user = user
        self.password = password

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
        req.user.value = bytes(self.user, encoding='utf-8')
        req.password.value = bytes(self.password, encoding='utf-8')
        self.send(req)

        resp = msg.MsgLoginResp()
        self.recv(resp)
        return resp

    def ls(self, path: str):
        req = msg.MsgLs()
        req.path.value = bytes(path, encoding='utf-8')
        self.send(req)

        resp = msg.MsgLsResp()
        self.recv(resp)
        return resp

    def mkdir(self, path: str):
        req = msg.MsgMkdir()
        req.path.value = bytes(path, encoding='utf-8')
        self.send(req)

        resp = msg.MsgMkdirResp()
        self.recv(resp)
        return resp

    def rm(self, path: str):
        req = msg.MsgRm()
        req.path.value = bytes(path, encoding='utf-8')
        self.send(req)

        resp = msg.MsgRmResp()
        self.recv(resp)
        return resp

    def chmod(self, path: str, mod: int):
        req = msg.MsgChmod()
        req.path.value = bytes(path, encoding='utf-8')
        req.mod.value = mod
        self.send(req)

        resp = msg.MsgChmodResp()
        self.recv(resp)
        return resp

    def chown(self, path: str, user: str):
        req = msg.MsgChown()
        req.path.value = bytes(path, encoding='utf-8')
        req.user.value = bytes(user, encoding='utf-8')
        self.send(req)

        resp = msg.MsgChownResp()
        self.recv(resp)
        return resp

    def open(self, path: str, open_mod: str):
        req = msg.MsgOpen()
        req.path.value = bytes(path, encoding='utf-8')
        req.open_mod.value = bytes(open_mod, encoding='utf-8')
        self.send(req)

        resp = msg.MsgOpenResp()
        self.recv(resp)
        return resp

    def close(self, fd: int):
        req = msg.MsgClose()
        req.fd.value = fd
        self.send(req)

        resp = msg.MsgCloseResp()
        self.recv(resp)
        return resp

    def read(self, fd: int, read_size: int):
        req = msg.MsgRead()
        req.fd.value = fd
        req.read_size.value = read_size
        self.send(req)

        resp = msg.MsgReadResp()
        self.recv(resp)
        return resp

    def write(self, fd: int, data: bytes):
        req = msg.MsgWrite()
        req.fd.value = fd
        req.data.value = data

        self.send(req)

        resp = msg.MsgWriteResp()
        self.recv(resp)
        return resp


if __name__ == '__main__':
    conn = DataNodeConn("127.0.0.1", 12345, "zxt", "zxt")
    resp = conn.login()
    resp = conn.ls("/")
    for f in resp.files.values:
        print(f.path)
    
    resp = conn.open("/zxt/a.txt", "w+")
    fd = resp.fd.value
    print(resp.fd, resp.error_code)

    resp = conn.write(fd, b"hello,world\ngoodbye!")
    print(resp.write_size, resp.error_code)


