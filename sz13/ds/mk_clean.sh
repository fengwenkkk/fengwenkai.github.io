#! /bin/sh

find . -name '*.o' -type f -print |xargs rm -f
find . -name 'dtp.a' -type f -print |xargs rm -f
find . -name 'gos.a' -type f -print |xargs rm -f
find . -name 'main.a' -type f -print |xargs rm -f
find . -name 'gplat.a' -type f -print |xargs rm -f
find . -name 'pdb.a' -type f -print |xargs rm -f
find . -name 'dis.a' -type f -print |xargs rm -f
find . -name 'omc.a' -type f -print |xargs rm -f
find . -name 'ds_chk.a' -type f -print |xargs rm -f
find . -name 'procmon.a' -type f -print |xargs rm -f
find . -name 'mrs.a' -type f -print |xargs rm -f
find . -name '.depend' -type f -print |xargs rm -f
