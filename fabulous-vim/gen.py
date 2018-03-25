#!/usr/bin/python

import re

charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

out = "let g:colors_name = \"rainbow\"\nset background=dark\nhi clear\nif exists(\"syntax_on\")\n  syntax reset\nendif\n"
l = len(charset)
for i in range(l):
    hex = "%03X%03X" % (i, int(256 * (1 - i/l)))
    out += f"syn match group{i} /{re.escape(charset[i])}/ skipwhite\nhi def group{i} ctermfg={hex}\n"  # guifg={hex}

print(out)
