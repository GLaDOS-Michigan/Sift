import sys
import os

weight = {'(':1, ')':-1}

breakchar = {'(', ' ', '~'}

def process_binary_op(st, op):
    target = "_" + op
    i = st.find(target)
    while i != -1:
        j = i - 1
        while st[j] not in breakchar:
            j -= 1
        s = 0
        for k in range(i, len(st)):
            if st[k] in weight:
                s += weight[st[k]]
            elif st[k] == ',' and s == 1:
                break
        st = st[:j + 1] + st[i + 2 : k] + ' ' + op + st[k + 1:]
        i = st.find(target)

    return st

def process_const(st, val):
    target = "_" + val
    i = st.find(target)
    while i != -1:
        j = i - 1
        while st[j] not in breakchar:
            j -= 1
        st = st[:j + 1] + st[i + 1:]
        i = st.find(target)

    return st

inv = open(sys.argv[1], 'r')
for st in inv:
    if not st.startswith('#') and "_prop" not in st:
        st = process_binary_op(st, '<')
        st = process_const(st, '0')
        print st,
