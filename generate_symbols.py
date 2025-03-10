#!/usr/bin/env python2
# coding: utf-8
"""
	Generate a symbol table from nm output.
"""

import fileinput

def formatLine(line):
    stripLength = len(line.strip().split(" "))
    if stripLength < 3 or stripLength > 4:
        return

    #  _, _, name = line.strip().split(" ")
    temp_list = line.strip().split(" ")
    size = 0
    if len(temp_list) >= 4:
        name, _, _, size = temp_list
    else:
        name, _, _ = temp_list
    if name == "abs":
        # abs is a keyword, so we'll just pretend we don't have that
        return
    if name == "kernel_symbols_start" or name == "kernel_symbols_end":
        # we also don't want to include ourselves...
        return
    print """
    .extern %s
    .align 8
    .quad %s
    .quad 0x%s
    .asciz \"%s\"""" % (name, name, size, name)


print ".section symbols"

print ".global kernel_symbols_start"
print "kernel_symbols_start:"
for line in fileinput.input():
    formatLine(line)

print ".global kernel_symbols_end"
print "kernel_symbols_end: ;"