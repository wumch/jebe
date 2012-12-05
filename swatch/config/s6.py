import os
from copy import deepcopy

import template
reload(template)

__sid = os.path.split(__file__)[1].split('.')[0]
__hsid = 'h2'
template.__initializition(__sid)
template.__servercombinedout(__sid)
template.__datacombine(__sid, __hsid)
config = deepcopy(template.__config)
config['mysql']['password'] = None
config['mysql']['database'] = config['rmysql']['database'] = __sid

if __name__ == '__main__':
    print(config)
