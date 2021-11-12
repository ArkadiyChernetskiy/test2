#! /bin/python

import random

result = []
k = random.randint(3,6)
for i in range (1,k):
    result.append ([])
    for j in range (1, random.randint(5,10)):
        result[-1].append (random.randint(1,1000))

print (' '.join ([str(len(i)) for i in result]))
for i in result:
    print (' '.join ([str(j) for j in i]))
