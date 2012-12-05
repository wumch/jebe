param = {}

param['s1'] = {}
param['s2'] = {}
param['s3'] = {}
param['s4'] = {}
param['s5'] = {}
param['s6'] = {}
param['s7'] = {}
param['s8'] = {}
param['s9'] = {}
param['s10'] = {}
param['s11'] = {}
param['s12'] = {}
param['s13'] = {}
param['s14'] = {}
param['s15'] = {}
param['s16'] = {}
param['s17'] = {}
param['s18'] = {}

common = {
    'put' : [
        (r'C:\\aaa.txt', '%(web)s/script'),
        (r'C:\\bbb.txt', '%(web)s/web_app/data.txt')
        ],
}

for sid, info in param.items():
    param[sid].update({'common' : common})
