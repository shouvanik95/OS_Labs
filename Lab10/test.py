import os
import sys

os.system("mkdir files")
for i in xrange(1,25):
    os.system("dd if=/dev/urandom of=./"+str(i)+" bs="+str(1024*1024*10)+" count=1");
