#!/bin/sh

./mk_dis.sh

chmod 777 /home/leijie/ds/bin/dis/dis
chmod 777 /home/leijie/ds/bin/dis/dis_chk
pkill procmon
pkill dis
pkill dis_chk
cp -f /home/leijie/ds/bin/dis/dis /home/admin/dis/dis
cp -f /home/leijie/ds/bin/dis/dis_chk /home/admin/dis/dis_chk


