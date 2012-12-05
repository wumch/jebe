#coding:utf-8

thinstr = {}

thinstr['sqlrun'] = \
"""
select date(`date`) as datetime,sum(cost) as cost from t_consume where `date`<"2009-05-01" group by date(`date`);
"""
#2009-10-27 10:00:00
#"2009-11-01 00:00:00" and "2009-11-10 12:59:59"
#"2009-11-10 13:00:00" and "2009-11-15 23:59:59"
thinstr['rsqlrun'] = \
"""

"""

thinstr['sherun'] = \
"""
cd %(web)s && cat log/tdata/tdata.3.8.txt
"""
#cat log/tdata/tdata.txt
#script/stuff_add_column_id.php
#script/stuff_add_column.php
#script/stuff_add_column_fullname.php
#script/stuff_reduce.php
#script/stuff_reduce_idsall.php
#script/stuff_add_0805.php
#script/silver.php

thinstr['geprun'] = \
"""
cd ../applog/online;ls *2009-08*|xargs awk -F'|' '{if ($2>bigest){ bigest=$2; }}END{print bigest}'
"""

thinstr['sqlist'] = \
"""
aaa
"""

thinstr['sqlParam']= \
"""
aaa
"""

thinstr['excelMysql'] = \
"""
t_effort.1.3.xls
"""

thinstr['sftprun'] = \
"""
bbb
"""

'''
thinstr['sftprun'] = {
    'get' : ((),),
    'put' : ((),),
}
'''
