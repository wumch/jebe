#coding:utf-8

import re
from rules import PredicateList as PL, AttrPumper
# field name:
URL = 'url'
LOC = 'location'        # domain + '/' + toppath
DOMAIN = 'domain'
MAIN_DOMAIN = 'maindomain'
SUB_DOMAIN = 'subdomain'
TOP_PATH = 'toppath'
TITLE = 'title'
TITLE_SLICE = 'title_slice'

url = AttrPumper(URL)
location = AttrPumper(LOC)
domain = AttrPumper(DOMAIN)
maindomain = AttrPumper(MAIN_DOMAIN)
subdomain = AttrPumper(SUB_DOMAIN)
toppath = AttrPumper(TOP_PATH)
title = AttrPumper(TITLE)
stitle = AttrPumper(TITLE_SLICE)

baidu_cates = {
    1:{'title' : '音乐影视'},

    101:{
        'title' : '娱乐综合',
        'parent' : 1,
        'cond' : PL(
            domain.eq('.iqiyi.com/zongyi/'),     # 奇艺综艺
            maindomain.eq('youku.com') & title.contains(u'—综艺—'),
            maindomain.eq('iqiyi.com') & stitle.contains(u'综艺'),
            domain.eq('zy.tudou.com'),          # （主页）
            domain.eq('zy.kankan.com') & stitle.contains(u'综艺'),
        ),
    },

    102: {
        'title' : '在线音乐',
        'parent' : 1,
        'cond' : PL(
            domain.eq('music.youku.com'),
            maindomain.eq('kugou.com') & toppath.inside(['webkugouplayer', 'yueku']),
            domain.eq('music.baidu.com'),
            domain.eq('www.1ting.com'),
            domain.eq('www.yinyuetai.com'),
            domain.eq('www.9ku.com'),
            domain.eq('www.kuwo.cn'),
            domain.eq('y.qq.com'),
            maindomain.eq('hcdj.com'),
        ),
    },
    103:{
        'title' : '宽带电影',       # 电影、电视剧，qvod等
        'parent' : 1,
        'cond' : PL(
            maindomain.eq('youku.com') & title.contains(u'—电影—'),
            maindomain.eq('tudou.com') & title.contains(u'电影'),
            maindomain.eq('iqiyi.com') & stitle.contains(u'电影'),
            maindomain.eq('kankan.com') & stitle.contains(u'电影'),
            maindomain.eq('juchang.com') & stitle.contains(u'电影'),
            maindomain.eq('qire123.com') & title.contains(u'电影'),
            domain.eq('video.sina.com.cn') & title.unmatch(u'(第[0-9]{1,3}|全)[集|季]'),
            domain.eq('www.m1905.com') & toppath.inside(['vod', 'mdb']),
            domain.eq('dianying.cntv.cn'),
            domain.eq('v.qq.com') & title.unmatch(u'(第[0-9]{1,3}|全)[集|季]'),
            (maindomain.eq('letv.com') & title.unmatch('[0-9]{2,} - ')) | domain.eq('movie.letv.com'),
            domain.eq('tv.sohu.com') & title.unmatch(u'(第[0-9]{1,3}|全)[集|季]'),
            maindomain.eq('hunantv.com') & title.contains(u'电影库'),
        )
    },
    104:{
        'title' : '视频短片',
        'parent' : 1,
        'cond' : PL(
            domain.eq('v.youku.com') & title.contains(u'—在线播放—'),
        ),
    },
    105:{
        'title' : '网络电视',
        'parent' : 1,
        'cond' : PL(

        ),
    },

    2:{'title' : '休闲娱乐'},
    201:{'title' : '笑话', 'parent' : 2},
    202:{'title' : '图片', 'parent' : 2},
    203:{'title' : '动漫', 'parent' : 2},
    204:{'title' : 'flash', 'parent' : 2},
    205:{'title' : '星座/算命与占卜', 'parent' : 2},
    206:{'title' : '非主流', 'parent' : 2},

    3:{'title' : '游戏'},
    301:{
        'title' : '综合游戏',
        'parent' : 3,
        'cond' : PL(
            domain.eq('www.17173.com'),
            domain.eq('www.4399.com') & toppath.notinside(['flash', 'special']),
            maindomain.eq('17173.com') & subdomain.inside(['www', 'news', 'media', 'download', 'bbs', 'blog', 'act', 'china']),
        ),
    },
    302:{
        'title' : '网络游戏',
        'parent' : 3,
        'cond' : PL(
            maindomain.eq('17173.com') & subdomain.notinside(['www', 'news', 'media', 'download', 'bbs', 'blog', 'web', 'pic', 'passport', 'images', 'act', 'china', 'flash']),
        ),
    },
    303:{'title' : '单机游戏', 'parent' : 3},
    304:{
        'title' : '掌机/手机游戏',
        'parent' : 3,
        'cond' : PL(
            domain.eq('iphone.shouyou.com'),
            domain.eq('itunes.apple.com'),
        ),
    },
    305:{
        'title' : '休闲小游戏',
        'parent' : 3,
        'cond': PL(
            domain.eq('www.4399.com') & toppath.inside(['flash', 'special']),
            domain.eq('www.7k7k.com') & toppath.inside(['flash', 'special']),
            domain.eq('flash.17173.com'),
        ),
    },
    306:{
        'title' : '网页游戏',
        'parent' : 3,
        'cond': PL(
            (maindomain.eq('4399.com') & subdomain.notinside(['www', 'web'])) | domain.eq('web.4399.com'),
            domain.eq('web.7k7k.com'),
            domain.contains('.web.17173.com'),
            domain.eq('game.yeyou.com'),
        ),
    },

    4:{'title' : '网络服务应用'},
    401:{
        'title' : '源码下载',
        'parent' : 4,
        'cond' : PL(
            domain.eq('download.csdn.net'),
            domain.eq('www.pudn.com'),
            maindomain.eq('oschina.net') & toppath.notinside(['news', 'question']),
        ),
    },
    402:{
        'title' : '技术论坛',
        'parent' : 4,
        'cond' : PL(
            maindomain.eq('csdn.net'),
            maindomain.eq('iteye.com'),
            maindomain.inside(['chinaunix.com', 'chinaunix.net']),
            maindomain.eq('infoq.com'),
        ),
    },
    403:{
        'title' : '电子邮箱',
        'parent' : 4,
        'cond' : PL(
            domain.eq('mail.163.com'),
            domain.eq('mail.126.com'),
            domain.eq('mail.cn.yahoo.com'),
            domain.eq('mail.sina.com.cn'),
            domain.eq('mail.qq.com'),
            domain.eq('mail.hotmail.com'),
            domain.eq('mail.gmail.com') | domain.contains('mail.google.com'),
            domain.eq('mail.10086.cn'),
            domain.eq('mail.sohu.com'),
            domain.eq('mail.189.cn'),
            domain.eq('mail.tom.com'),
            domain.eq('mail.sogou.com'),
            domain.eq('mail.eyou.com'),
            domain.eq('mail.21cn.com'),
            domain.eq('mail.188.com'),
            domain.eq('mail.yeah.net'),
            domain.eq('mail.foxmail.com'),
            domain.eq('mail.wo.com.cn'),
            domain.eq('mail.263.net'),
            domain.eq('mail.outlook.com'),
        ),
    },
    404:{
        'title' : '网盘',
        'parent' : 4,
        'cond' : PL(
            maindomain.eq('115.com'),
            maindomain.eq('kuaipan.com'),
            maindomain.eq('dbank.com'),
            domain.eq('kuai.xunlei.com'),
            maindomain.eq('weiyun.com'),
            domain.eq('pan.baidu.com'),
            maindomain.eq('1tpan.com'),
        ),
    },
    405:{
        'title' : '网络相册',
        'parent' : 4,
        'cond' : PL(

        ),
    },
    406:{
        'title' : '在线翻译',
        'parent' : 4,
        'cond' : PL(

        ),
    },
    407:{
        'title' : '域名注册',
        'parent' : 4,
        'cond' : PL(

        ),
    },
    408:{
        'title' : '搜索查询',
        'parent' : 4,
        'cond' : PL(

        ),
    },

    5:{'title' : '博客'},
    501:{'title' : '财经', 'parent' : 5},
    502:{'title' : '娱乐', 'parent' : 5},
    503:{'title' : '体育', 'parent' : 5},
    504:{'title' : '社会', 'parent' : 5},
    505:{'title' : '教育', 'parent' : 5},
    506:{'title' : '女性', 'parent' : 5},
    507:{'title' : '空间周边', 'parent' : 5},

    6:{'title' : '网址导航'},

    7:{'title' : '计算机软件硬件'},
    701:{
        'title' : '电脑产品',
        'parent' : 7,
        'cond' : PL(

        ),
    },
    702:{'title' : '硬件资讯', 'parent' : 7},
    703:{'title' : '软件下载', 'parent' : 7},
    704:{'title' : '软件交流', 'parent' : 7},

    8:{'title' : '数码及手机'},
    801:{'title' : '数码及手机资讯', 'parent' : 8},
    802:{'title' : 'MP3及MP4', 'parent' : 8},
    803:{'title' : '相机/摄像机', 'parent' : 8},
    804:{'title' : '数字家电', 'parent' : 8},
    805:{'title' : '游戏机', 'parent' : 8},
    806:{'title' : '手机资源及下载', 'parent' : 8},
    807:{'title' : '3G服务', 'parent' : 8},

    9:{'title' : '教学及考试'},
    901:{'title' : '幼儿教育', 'parent' : 9},
    902:{'title' : '小学/初中教育', 'parent' : 9},
    903:{'title' : '高中教育/高考', 'parent' : 9},
    904:{'title' : '考研', 'parent' : 9},
    905:{'title' : '资格考试', 'parent' : 9},
    906:{'title' : '英语及其他培训', 'parent' : 9},
    907:{'title' : '课件下载', 'parent' : 9},

    10:{'title' : '医疗保健'},
    1001:{'title' : '医学行业', 'parent' : 10},
    1002:{'title' : '医疗健康', 'parent' : 10},
    1003:{'title' : '母婴育儿', 'parent' : 10},
    1004:{'title' : '两性健康', 'parent' : 10},
    1005:{'title' : '儿童健康', 'parent' : 10},

    11:{'title' : '女性时尚'},
    1101:{'title' : '美容减肥', 'parent' : 11},
    1102:{'title' : '时装时尚', 'parent' : 11},
    1103:{'title' : '奢侈品', 'parent' : 11},
    1104:{'title' : '时尚论坛', 'parent' : 11},
    1105:{'title' : '女性综合', 'parent' : 11},

    12:{'title' : '社交网络和在线社区'},
    1201:{'title' : '综合论坛', 'parent' : 12},
    1202:{'title' : '交友和婚恋', 'parent' : 12},
    1203:{'title' : '同学录', 'parent' : 12},
    1204:{'title' : 'SNS', 'parent' : 12},
    1205:{'title' : '垂直论坛', 'parent' : 12},

    13:{'title' : '生活服务'},
    1301:{'title' : '宠物综合', 'parent' : 13},
    1302:{'title' : '菜谱/饮食网址', 'parent' : 13},
    1303:{'title' : '分类信息', 'parent' : 13},

    14:{'title' : '房产家居'},
    1401:{'title' : '买卖租赁', 'parent' : 14},
    1402:{'title' : '家居装饰', 'parent' : 14},

    15:{'title' : '汽车'},
    1501:{'title' : '汽车资讯', 'parent' : 15},
    1502:{'title' : '汽车周边服务', 'parent' : 15},
    1503:{'title' : '车友会', 'parent' : 15},
    1504:{'title' : '地方车网', 'parent' : 15},

    16:{'title' : '交通旅游'},
    1601:{'title' : '旅游资讯', 'parent' : 16},
    1602:{'title' : '票务服务（机票、火车票）', 'parent' : 16},
    1603:{'title' : '旅行社/酒店', 'parent' : 16},
    1604:{'title' : '出行查询（地图、天气）', 'parent' : 16},

    17:{'title' : '体育运动'},
    1701:{'title' : '足球', 'parent' : 17},
    1702:{'title' : '篮球NBA', 'parent' : 17},
    1703:{'title' : '体育综合', 'parent' : 17},

    18:{'title' : '投资金融'},
    1801:{'title' : '财经资讯', 'parent' : 18},
    1802:{'title' : '股票基金', 'parent' : 18},
    1803:{'title' : '博彩/彩票', 'parent' : 18},

    19:{'title' : '垂直行业'},
    1901:{'title' : '电子五金', 'parent' : 19},
    1902:{'title' : '农林牧渔', 'parent' : 19},
    1903:{'title' : '轻工/纺织', 'parent' : 19},
    1904:{'title' : '重工/机械', 'parent' : 19},
    1905:{'title' : '化工/能源', 'parent' : 19},
    1906:{'title' : '仪表仪器', 'parent' : 19},
    1907:{'title' : '纸业印刷', 'parent' : 19},
    1908:{'title' : '交通物流', 'parent' : 19},
    1909:{'title' : '法律法规', 'parent' : 19},
    1910:{'title' : '商务贸易', 'parent' : 19},

    20:{'title' : '新闻媒体'},
    2001:{'title' : '综合门户', 'parent' : 20},
    2002:{'title' : '地方媒体', 'parent' : 20},
    2003:{'title' : '媒体报刊', 'parent' : 20},
    2004:{'title' : '广播/电视', 'parent' : 20},
    2005:{'title' : '政史军事', 'parent' : 20},

    21:{'title' : '人文艺术'},
    2101:{'title' : '摄影', 'parent' : 21},
    2102:{'title' : '琴棋书画', 'parent' : 21},
    2103:{'title' : '设计', 'parent' : 21},
    2104:{'title' : '曲艺', 'parent' : 21},
    2105:{'title' : '鉴赏收藏', 'parent' : 21},

    22:{'title' : '小说'},
    2201:{'title' : '小说阅读', 'parent' : 22},
    2202:{'title' : '文化文学', 'parent' : 22},
    2203:{'title' : '电子书', 'parent' : 22},

    23:{'title' : '人才招聘'},
    2301:{'title' : '综合人才网站', 'parent' : 23},
    2302:{'title' : '地方人才网站精选', 'parent' : 23},
    2303:{'title' : '行业人才网站', 'parent' : 23},

    24:{'title' : '网络购物'},
    2401:{'title' : '购物综合', 'parent' : 24},
    2402:{'title' : '家居用品', 'parent' : 24},
    2403:{'title' : '数码家电', 'parent' : 24},
    2404:{'title' : '导购比价', 'parent' : 24},
    2405:{'title' : '电子支付', 'parent' : 24},
}

i8cates = {
    106 : {
        'title':'电视剧',
        'parent':1,
        'cond': PL(
            maindomain.eq('youku.com') & stitle.contains(u'电视剧'),
            maindomain.eq('tudou.com') & stitle.contains(u'电视剧'),
            maindomain.eq('iqiyi.com') & stitle.contains(u'电视剧'),
            maindomain.eq('kankan.com') & stitle.contains(u'电视剧'),
            maindomain.eq('juchang.com') & stitle.contains(u'电视剧'),
            maindomain.eq('qire123.com') & (title.contains(u'电视剧') | title.match(u'(第[0-9]{1,3}|全)[集|季]')),
            domain.eq('video.sina.com.cn') & title.match(u'(第[0-9]{1,3}|全)[集|季]'),
            domain.eq('www.m1905.com') & toppath.inside(['vod', 'mdb']),
            domain.eq('dianshiju.cntv.cn'),
            domain.eq('v.qq.com') & title.match(u'(第[0-9]{1,3}|全)[集|季]'),
            domain.eq('tv.letv.com') & title.match(u'[0-9]{2,} - '),
            domain.eq('tv.sohu.com') & title.match(u'(第[0-9]{1,3}|全)[集|季]'),
            maindomain.eq('hunantv.com') & title.contains(u'电视剧库'),
            domain.contains('jebe'),
        )
    },

    2406 : {'title' : '团购', 'parent' : 24 },

    1206 : {'title' : 'qq空间', 'parent': 12 },

    25 : {'title': '成人(18禁)'},
    2501 : {'title' : '成人文学', 'parent': 25 },
    2502 : {'title' : '成人论坛', 'parent': 25 },
    2503 : {'title' : '成人交友', 'parent': 25 },
    2504 : {'title' : '成人视频', 'parent': 25 },
    2505 : {'title' : '成人用品', 'parent': 25 },
    2506 : {'title' : '成人动漫', 'parent': 25 },


#    2502 : {'title' : '', 'parent': 25 },
}

cates = baidu_cates.copy()
cates.update(i8cates)
