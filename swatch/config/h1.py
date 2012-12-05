import os
from copy import deepcopy

import template
reload(template)

__sid = os.path.basename(__file__).split('.', 1)[0]
template.__initializition(__sid)
template.__servercombinedout(__sid)
config = deepcopy(template.__config)
config['mysql']['password'] = None
config['mysql']['database'] = config['rmysql']['database'] = __sid

if __name__ == '__main__':
    print(config)
