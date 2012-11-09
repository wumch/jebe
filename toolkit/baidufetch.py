#:coding:utf-8
import urllib2
from sgmllib import SGMLParser
class HotExtract(SGMLParser):
   
    def __init__(self):
        SGMLParser.__init__(self)
        self.is_a = ""
        self.hot = []
        
    def start_a(self, attrs):
        if len(attrs) == 0:
            pass
        else:
            for (variable, value) in attrs:
                if variable == "mon" and value == "ct=1&a=30":
                    self.is_a = 1
                    break
                
    def end_a(self):
        self.is_a = ""
        
    def handle_data(self, text):
        print self.is_a
        if self.is_a == 1:
            self.hot.append(text)

def getHtml(url):
    headers = {
        'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
        'Accept-Charset':'GBK,utf-8;q=0.7,*;q=0.3',
        'Accept-Encoding':'gzip,deflate,sdch',
        'Accept-Language':'zh-CN,zh;q=0.8,en;q=0.6',
        'Cache-Control':'max-age=0',
        'Connection':'close',
        'Cookie':'BAIDUID=6B349A575F2205DBB7AC88A6CA929B22:FG=1; BDUT=fiti6B349A575F2205DBB7AC88A6CA929B2213ad44b549d0; BDSFRCVID=MdPEJ2702S74goSBqLHwENtYogKK0gsvQQTsVMYfoNJgfiVM3J; H_BDCLCKID_SF=tJIHoIDKJKvbfP0kMtvEbbQH-UnLqMnzX2TZoD0Mtf8bMb3vQtnaXMkWQtTJX4RMW20j0-OKLxjxqUQEWtjoXxIsbxrpW6JZfJutoDthJD03fP36q4rSMt00qxby26ngyeJ4aD_5tIo6MIbThUoIKfAn5Pow-pcy523ion3bbfjGS-OKXqrthUCpbUj0LfueWJLH_C_MtCLbbP365ITEh-FO5eT22-us56RWKlF-JKtKVI0mbPnr5J_TyhJ4WJ3kaKviaKJj--QGVfbR3xcZb44NyxJhal5ut2LEoDK-tID-MIv65nt_MtAtbh_X5-RLfbR0Mh5FtD02hx86j-Jo0PCLX-7f0x6JyCOMahkM5-O-Vfj2KpjYWtKeX46MKtcpBR7ZVD_atDtaMC0r5nJbq4FjMfOtetJyaR3DWbvNWD_bhKobjTOJhptwKtv7-MQwbN5y0hvctn35fxjh3bb4XqFEhPTZK-50QeDeaDF-JC_2hDL6enJb5ICthMrb-C62aKDs2Rbx3lFKMIt40TtMbP49LlQrBnb4BjnkWb3cWKJJqPQh0qO53xu4bxnwBtbh2GcuohFLK-oj-DKwjTt-3e; Hm_lvt_79a0e9c520104773e13ccd072bc956aa=1352183548711; Hm_lpvt_79a0e9c520104773e13ccd072bc956aa=1352185014743',
#        'Host':'top.baidu.com',
        'Referer':'http://www.baidu.com/s?wd=%B7%E7%D4%C6%B0%F1',
        'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.52 Safari/536.5',
    }
    request = urllib2.Request(url=url, headers=headers)
    try:
        response = urllib2.urlopen(request, timeout=10)
        if response.code == 200:
            return response.read()
        else:
            print "response-code:", response.code
    except urllib2.HTTPError, e:
        print "http-error:", str(e.args)
    except urllib2.URLError, e:
        print "url-error:", str(e.args)
    except Exception, e:
        print "unknow-error:", str(e.args)

def extract_hot(html):
    hotExtract = HotExtract()
    hotExtract.feed(html)
    return hotExtract.hot

html = getHtml("http://top.baidu.com/")
print len(html)
print unicode(html, 'gbk')
html = html[:100].decode('gbk')
print html
hot_list = extract_hot(html)
for hot in hot_list:
    print "fetched"
    print hot
