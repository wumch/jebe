#!/bin/sh

tmp="/tmp/jebe"
rm -fr ${tmp}
cp -rL /data/fsuggest/jebe ${tmp}
cd ${tmp}

home="${tmp}"
if [ ! -n "${home}" ] && [ "${h:0:5}" != "/tmp/" ]; then
    echo -e 'cannot resolve home-path.'
    exit 1
fi

cd ${home}
rm -fr .git libsvm cws/bydict/counted.txt cws/bydict/tests/* cws/gendict/tests/* /data/fsuggest/jebe/.autotools  /data/fsuggest/jebe/.cproject  /data/fsuggest/jebe/.project

cd ${home}/cws/bydict
make clean

cd ${home}/cws/gendict
make clean

cd $(dirname ${home})
directory="$(basename ${home})"
tgz="${directory}.tgz"
tar zcf ${tgz} ${directory}

scp ${tgz} david@211.154.172.140:/home/david/jebe/
#ssh david@211.154.172.140 rm -fr /home/david/jebe/jebe && tar zxf /home/david/jebe/${tgz} -C /home/david/jebe && mv /home/david/jebe/jebe/* /home/david/jebe/

