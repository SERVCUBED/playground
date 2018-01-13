#!/usr/bin/python

# using dvorak-ukp layout
qwerty = '1234567890-=qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>? '
dvorak = '1234567890-=/,.pyfgcrl[]\\aoeuidhtns\';qjkxbmwvz~!@#$%^&*()_+?<>PYFGCRL{}|AOEUIDHTNS":QJKXBMWVZ '


def encode(inp: str) -> str:
    return ''.join([dvorak[qwerty.index(s)] for s in inp])


def decode(inp: str) -> str:
    return ''.join([qwerty[dvorak.index(s)] for s in inp])


if __name__ == '__main__':
    while True:
        inp = input("enter input: ")
        oper = input("enter:\ne encode\nd decode")
        print(encode(inp) if oper == 'e' else (decode(inp) if oper == 'd' else "Invalid option"))
