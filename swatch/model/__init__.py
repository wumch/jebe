import os

__all__ = filter(lambda modname:modname and not (modname[0:2] == '__' and modname[-2:] == '__'), map(lambda fname:fname[-3:] == '.py' and fname.split('.')[0], os.listdir(os.path.dirname(os.path.realpath(__file__)))))
