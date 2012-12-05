from rabs import rabs

class shesql(rabs):
    
    def exc(self, sql):
        return self._shell(self.__decorate(sql))
    
    def __decorate(self, sql):
        return 'mysql -h' + self.config['mysql']['host'] \
        + ' -u' + self.config['mysql']['user'] \
        + ((self.config['mysql']['password'] is None and ' ') or (' -p' + self.config['mysql']['password'])) \
        + ' --database=' + self.config['mysql']['database'] \
        + ' --default-character-set=' + self.config['mysql']['charset'] \
        + ' -e \'' + sql.replace("'", '"') + '\''
