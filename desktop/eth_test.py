from socket import *
from time import sleep

my_mac = b"\x00\xe0\x99\x00\x09\x85"
dst_mac = b"\x00\x01\x02\x03\x04\x05"
payload = b"\xB0\x0B\xA0"

def send_eth(dst, src, type_len, payload, iface="wlo1"):
    assert(len(src) == len(dst) == 6)
    assert(len(type_len) == 2)
    s = socket(AF_PACKET, SOCK_RAW)
    s.bind((iface, 0))
    return s.send(src + dst + type_len + payload)

if __name__ == "__main__":
    while True:
        send_eth(my_mac, dst_mac, b"\x00\x03", payload)
        print("Data sent")
        sleep(1)