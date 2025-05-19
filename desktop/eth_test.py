from socket import *
from time import sleep

my_mac = b"\x00\xe0\x99\x00\x09\x85"
dst_mac = b"\x00\x01\x02\x03\x04\x05"
payload = b"HelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHello"
payload += b'\x00' * (64 - len(payload))

def send_eth(dst, src, type_len, payload, iface="enp3s0f3u2c2"):
    assert(len(src) == len(dst) == 6)
    assert(len(type_len) == 2)
    s = socket(AF_PACKET, SOCK_RAW)
    s.bind((iface, 0))
    return s.send(src + dst + type_len + payload)

if __name__ == "__main__":
    send_eth(my_mac, dst_mac, int.to_bytes(len(payload), 2), payload)
    print("Data sent")
