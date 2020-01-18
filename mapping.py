#!/usr/bin/python3

import sys, re

dic = dict()
# read Big5-ZhuYin.map into dictionary
for line in open(sys.argv[1], "r", encoding='big5hkscs'):
	wordlist = re.split('[/\s]', line)
	word = wordlist[0]

	for x in wordlist:
		if not x:
			continue
		if x[0] not in dic:
			dic[x[0]] = set()
		dic[x[0]].add(word)

with open(sys.argv[2], "w", encoding='big5hkscs') as f:
	for k, v in dic.items():
		print(k, '\t', ' '.join(str(x) for x in v), file=f)