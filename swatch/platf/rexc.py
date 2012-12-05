from rabs import rabs

class rexc(rabs):
    
    def exc(self, cmd):
        return self._shell(self.__decorate(cmd))
    
    def __decorate(self, cmd):
        return cmd
