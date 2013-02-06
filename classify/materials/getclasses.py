#!/usr/bin/env python
#coding:utf-8

import urllib2
import re
import math

urls = {}

config = {
    'start_page': 'http://www.dmoz.org/World/Chinese_Simplified/',
}

domains = []

class Category(object):

    def __init__(self, id=None, url=None, name=None, parent=None, children=None, count=0):
        self.id = id
        self.url = url
        self.name = name
        self.children = children or []
        self.parent = parent
        self.count = count

    def attach_children(self, children):
        children = [children] if isinstance(children, Category) else children
        self.children = list(set((self.children or []) + children))

class Cats(object):

    def __init__(self):
        self.root = Category()

    def build(self, urls):
        # top level
        for info in urls.itervalues():
            if 'parent' not in info:
                cat = Category(id=info['id'], name=info['title'])
                self.root.attach_children(cat)
        # second level
        for info in urls.itervalues():
            if 'parent' in info:
                cat = Category(id=info['id'], url=info['href'], parent=info['parent'], name=info['title'], count=info['count'])
                self.root.children[info['parent']].attach_children(cat)

    def attach_category(self, cat):
        pass

    def url_to_category(self, url_info):
        return Category(id=url_info['id'], name=url_info['title'],
            url=url_info['href'] if 'href' in url_info else None
        )

    def all(self):
        global urls
        self.build(urls)
        for cat in self.root.children:
            if cat.children:
                for c in cat.children:
                    yield c

class ContentProvider(object):

    def __init__(self, cat):
        self.cat = cat
        self.more = True
        self.pages = range(1, int(math.ceil(self.cat.count / 10.0)) + 1)
        self.curpage = 0
        self.prefix = self.cat.url + '/'

    def next_url(self):
        for page in self.pages:
            yield self.prefix + str(page)

    def request(self, url):
        try:
            return urllib2.urlopen(url)
        except urllib2.HTTPError:
            for i in range(0, 3):
                try:
                    return urllib2.urlopen(url)
                except urllib2.HTTPError:
                    pass

    def get_html(self, url):
        rep = self.request(url=url)
        return rep.read() if rep else ''

    def contents(self):
        for url in self.next_url():
            yield self.get_html(url=url)

class Pumper(object):

    def __init__(self):
        self.regexp = re.compile(r'<li class="domain"><strong>.*: </strong><a href=".*" class="external" target="_blank">(.*)</a></li>')

    def pump(self, content):
        return self.regexp.findall(content)

class GetClasses(object):

    def __init__(self):
        self.cats = Cats()
        self.pumper = Pumper()

    def run(self):
        for cat in self.cats.all():
            contents = ContentProvider(cat).contents()
            for content in contents:
                for url in self.pumper.pump(content):
                    print "%d\t%d\t%s" % (cat.parent, cat.id, url)

urls = {
    "1": {"id": 0, "title": "休闲娱乐"},
    "108": {"id": 1, "title": "网上商城"},
    "2": {"id": 2, "title": "网络服务"},
    "3": {"id": 3, "title": "商业经济"},
    "4": {"id": 4, "title": "生活服务"},
    "5": {"id": 5, "title": "教育文化"},
    "6": {"id": 6, "title": "博客论坛"},
    "7": {"id": 7, "title": "综合其他"},
    "8": {"id": 8, "parent": 0, "href": "http://www.dir001.com/list/8",
          "title": "电影", "count": 292},
    "9": {"id": 9, "parent": 0, "href": "http://www.dir001.com/list/9",
          "title": "动漫", "count": 47},
    "10": {"id": 10, "parent": 0, "href": "http://www.dir001.com/list/10",
           "title": "图片壁纸", "count": 74},
    "11": {"id": 11, "parent": 0, "href": "http://www.dir001.com/list/11",
           "title": "明星", "count": 38},
    "12": {"id": 12, "parent": 0, "href": "http://www.dir001.com/list/12",
           "title": "星座", "count": 76},
    "13": {"id": 13, "parent": 0, "href": "http://www.dir001.com/list/13",
           "title": "收集珍藏", "count": 50},
    "14": {"id": 14, "parent": 0, "href": "http://www.dir001.com/list/14",
           "title": "交友", "count": 88},
    "15": {"id": 15, "parent": 0, "href": "http://www.dir001.com/list/15",
           "title": "笑话", "count": 109},
    "16": {"id": 16, "parent": 0, "href": "http://www.dir001.com/list/16",
           "title": "体育", "count": 146},
    "17": {"id": 17, "parent": 0, "href": "http://www.dir001.com/list/17",
           "title": "其他", "count": 340},
    "18": {"id": 18, "parent": 2, "href": "http://www.dir001.com/list/18",
           "title": "门户名站", "count": 398},
    "19": {"id": 19, "parent": 2, "href": "http://www.dir001.com/list/19",
           "title": "搜索引擎", "count": 132},
    "20": {"id": 20, "parent": 1, "href": "http://www.dir001.com/list/20",
           "title": "商务平台", "count": 617},
    "21": {"id": 21, "parent": 2, "href": "http://www.dir001.com/list/21",
           "title": "电脑硬件", "count": 162},
    "22": {"id": 22, "parent": 2, "href": "http://www.dir001.com/list/22",
           "title": "软件编程", "count": 192},
    "23": {"id": 23, "parent": 0, "href": "http://www.dir001.com/list/23",
           "title": "游戏", "count": 362},
    "24": {"id": 24, "parent": 2, "href": "http://www.dir001.com/list/24",
           "title": "网络资源", "count": 290},
    "25": {"id": 25, "parent": 2, "href": "http://www.dir001.com/list/25",
           "title": "网络学校", "count": 36},
    "26": {"id": 26, "parent": 2, "href": "http://www.dir001.com/list/26",
           "title": "IT资讯", "count": 144},
    "27": {"id": 27, "parent": 2, "href": "http://www.dir001.com/list/27",
           "title": "资源下载", "count": 185},
    "28": {"id": 28, "parent": 2, "href": "http://www.dir001.com/list/28",
           "title": "域名空间", "count": 115},
    "29": {"id": 29, "parent": 2, "href": "http://www.dir001.com/list/29",
           "title": "网站建设", "count": 862},
    "30": {"id": 30, "parent": 2, "href": "http://www.dir001.com/list/30",
           "title": "站长资源", "count": 107},
    "31": {"id": 31, "parent": 2, "href": "http://www.dir001.com/list/31",
           "title": "网络营销", "count": 383},
    "32": {"id": 32, "parent": 2, "href": "http://www.dir001.com/list/32",
           "title": "网址黄页", "count": 286},
    "33": {"id": 33, "parent": 1, "href": "http://www.dir001.com/list/33",
           "title": "数码电器", "count": 206},
    "34": {"id": 34, "parent": 2, "href": "http://www.dir001.com/list/34",
           "title": "手机通信", "count": 141},
    "35": {"id": 35, "parent": 0, "href": "http://www.dir001.com/list/35",
           "title": "视频", "count": 40},
    "36": {"id": 36, "parent": 2, "href": "http://www.dir001.com/list/36",
           "title": "网络安全", "count": 69},
    "37": {"id": 37, "parent": 2, "href": "http://www.dir001.com/list/37",
           "title": "其他", "count": 253},
    "38": {"id": 38, "parent": 3, "href": "http://www.dir001.com/list/38",
           "title": "农林牧渔", "count": 400},
    "39": {"id": 39, "parent": 3, "href": "http://www.dir001.com/list/39",
           "title": "能源化工", "count": 431},
    "40": {"id": 40, "parent": 3, "href": "http://www.dir001.com/list/40",
           "title": "机械电子", "count": 2564},
    "41": {"id": 41, "parent": 3, "href": "http://www.dir001.com/list/41",
           "title": "建筑环境", "count": 1028},
    "42": {"id": 42, "parent": 3, "href": "http://www.dir001.com/list/42",
           "title": "金融保险", "count": 385},
    "43": {"id": 43, "parent": 3, "href": "http://www.dir001.com/list/43",
           "title": "法律财经", "count": 199},
    "44": {"id": 44, "parent": 3, "href": "http://www.dir001.com/list/44",
           "title": "贸易招商", "count": 436},
    "45": {"id": 45, "parent": 3, "href": "http://www.dir001.com/list/45",
           "title": "广告营销", "count": 384},
    "46": {"id": 46, "parent": 3, "href": "http://www.dir001.com/list/46",
           "title": "商务服务", "count": 1132},
    "47": {"id": 47, "parent": 3, "href": "http://www.dir001.com/list/47",
           "title": "会展活动", "count": 83},
    "48": {"id": 48, "parent": 3, "href": "http://www.dir001.com/list/48",
           "title": "咨询服务", "count": 196},
    "49": {"id": 49, "parent": 3, "href": "http://www.dir001.com/list/49",
           "title": "工业制品", "count": 1131},
    "50": {"id": 50, "parent": 3, "href": "http://www.dir001.com/list/50",
           "title": "纸业印刷", "count": 242},
    "51": {"id": 51, "parent": 3, "href": "http://www.dir001.com/list/51",
           "title": "钟表眼镜", "count": 41},
    "52": {"id": 52, "parent": 3, "href": "http://www.dir001.com/list/52",
           "title": "交通物流", "count": 230},
    "53": {"id": 53, "parent": 3, "href": "http://www.dir001.com/list/53",
           "title": "影像器材", "count": 88},
    "54": {"id": 54, "parent": 3, "href": "http://www.dir001.com/list/54",
           "title": "仪器仪表", "count": 439},
    "55": {"id": 55, "parent": 3, "href": "http://www.dir001.com/list/55",
           "title": "冶金冶炼", "count": 265},
    "56": {"id": 56, "parent": 3, "href": "http://www.dir001.com/list/56",
           "title": "汽摩配件", "count": 314},
    "57": {"id": 57, "parent": 3, "href": "http://www.dir001.com/list/57",
           "title": "其他", "count": 1029},
    "58": {"id": 58, "parent": 1, "href": "http://www.dir001.com/list/58",
           "title": "服装鞋帽", "count": 642},
    "59": {"id": 59, "parent": 4, "href": "http://www.dir001.com/list/59",
           "title": "餐饮美食", "count": 329},
    "60": {"id": 60, "parent": 4, "href": "http://www.dir001.com/list/60",
           "title": "房产家居", "count": 1014},
    "61": {"id": 61, "parent": 4, "href": "http://www.dir001.com/list/61",
           "title": "宾馆旅游", "count": 939},
    "62": {"id": 62, "parent": 4, "href": "http://www.dir001.com/list/62",
           "title": "交通物流", "count": 372},
    "63": {"id": 63, "parent": 1, "href": "http://www.dir001.com/list/63",
           "title": "百货购物", "count": 313},
    "64": {"id": 64, "parent": 4, "href": "http://www.dir001.com/list/64",
           "title": "医疗保健", "count": 2863},
    "65": {"id": 65, "parent": 4, "href": "http://www.dir001.com/list/65",
           "title": "宠物玩具", "count": 80},
    "66": {"id": 66, "parent": 1, "href": "http://www.dir001.com/list/66",
           "title": "鲜花礼品", "count": 173},
    "67": {"id": 67, "parent": 1, "href": "http://www.dir001.com/list/67",
           "title": "美容", "count": 351},
    "68": {"id": 68, "parent": 4, "href": "http://www.dir001.com/list/68",
           "title": "家政礼仪", "count": 260},
    "69": {"id": 69, "parent": 4, "href": "http://www.dir001.com/list/69",
           "title": "生活常识", "count": 504},
    "70": {"id": 70, "parent": 4, "href": "http://www.dir001.com/list/70",
           "title": "婚恋家庭", "count": 177},
    "71": {"id": 71, "parent": 4, "href": "http://www.dir001.com/list/71",
           "title": "天气预报", "count": 23},
    "72": {"id": 72, "parent": 4, "href": "http://www.dir001.com/list/72",
           "title": "其他", "count": 1118},
    "73": {"id": 73, "parent": 5, "href": "http://www.dir001.com/list/73",
           "title": "高校", "count": 151},
    "74": {"id": 74, "parent": 5, "href": "http://www.dir001.com/list/74",
           "title": "人力资源", "count": 476},
    "75": {"id": 75, "parent": 5, "href": "http://www.dir001.com/list/75",
           "title": "高考考研", "count": 177},
    "76": {"id": 76, "parent": 5, "href": "http://www.dir001.com/list/76",
           "title": "少儿教育", "count": 209},
    "77": {"id": 77, "parent": 5, "href": "http://www.dir001.com/list/77",
           "title": "成人教育", "count": 132},
    "78": {"id": 78, "parent": 5, "href": "http://www.dir001.com/list/78",
           "title": "培训", "count": 769},
    "79": {"id": 79, "parent": 5, "href": "http://www.dir001.com/list/79",
           "title": "留学出国", "count": 89},
    "80": {"id": 80, "parent": 5, "href": "http://www.dir001.com/list/80",
           "title": "报刊图书", "count": 74},
    "81": {"id": 81, "parent": 5, "href": "http://www.dir001.com/list/81",
           "title": "音像制品", "count": 8},
    "82": {"id": 82, "parent": 5, "href": "http://www.dir001.com/list/82",
           "title": "文学论文", "count": 204},
    "83": {"id": 83, "parent": 5, "href": "http://www.dir001.com/list/83",
           "title": "英语", "count": 98},
    "84": {"id": 84, "parent": 5, "href": "http://www.dir001.com/list/84",
           "title": "文化艺术", "count": 262},
    "85": {"id": 85, "parent": 1, "href": "http://www.dir001.com/list/85",
           "title": "文体", "count": 61},
    "86": {"id": 86, "parent": 5, "href": "http://www.dir001.com/list/86",
           "title": "体育", "count": 37},
    "87": {"id": 87, "parent": 5, "href": "http://www.dir001.com/list/87",
           "title": "其他", "count": 204},
    "88": {"id": 88, "parent": 6, "href": "http://www.dir001.com/list/88",
           "title": "休闲娱乐", "count": 273},
    "89": {"id": 89, "parent": 6, "href": "http://www.dir001.com/list/89",
           "title": "电脑网络", "count": 349},
    "90": {"id": 90, "parent": 6, "href": "http://www.dir001.com/list/90",
           "title": "教育文化", "count": 85},
    "91": {"id": 91, "parent": 6, "href": "http://www.dir001.com/list/91",
           "title": "生活服务", "count": 360},
    "92": {"id": 92, "parent": 6, "href": "http://www.dir001.com/list/92",
           "title": "其他", "count": 161},
    "93": {"id": 93, "parent": 7, "href": "http://www.dir001.com/list/93",
           "title": "政府组织", "count": 57},
    "94": {"id": 94, "parent": 7, "href": "http://www.dir001.com/list/94",
           "title": "省市导航", "count": 62},
    "95": {"id": 95, "parent": 7, "href": "http://www.dir001.com/list/95",
           "title": "工商税务", "count": 43},
    "96": {"id": 96, "parent": 7, "href": "http://www.dir001.com/list/96",
           "title": "公共福利", "count": 18},
    "97": {"id": 97, "parent": 7, "href": "http://www.dir001.com/list/97",
           "title": "社会团体", "count": 36},
    "98": {"id": 98, "parent": 7, "href": "http://www.dir001.com/list/98",
           "title": "科学技术", "count": 87},
    "99": {"id": 99, "parent": 7, "href": "http://www.dir001.com/list/99",
           "title": "天文地理", "count": 9},
    "100": {"id": 100, "parent": 7, "href": "http://www.dir001.com/list/100"
        , "title": "历史考古", "count": 7},
    "101": {"id": 101, "parent": 7, "href": "http://www.dir001.com/list/101"
        , "title": "军事", "count": 19},
    "102": {"id": 102, "parent": 7, "href": "http://www.dir001.com/list/102"
        , "title": "新闻综合", "count": 169},
    "103": {"id": 103, "parent": 7, "href": "http://www.dir001.com/list/103"
        , "title": "综合网站", "count": 951},
    "104": {"id": 104, "parent": 7, "href": "http://www.dir001.com/list/104"
        , "title": "个人网站", "count": 408},
    "105": {"id": 105, "parent": 7, "href": "http://www.dir001.com/list/105"
        , "title": "其他", "count": 558},
    "106": {"id": 106, "parent": 0, "href": "http://www.dir001.com/list/106"
        , "title": "社区", "count": 102},
    "107": {"id": 107, "parent": 1, "href": "http://www.dir001.com/list/107"
        , "title": "导购淘客", "count": 798},
    "109": {"id": 109, "parent": 0, "href": "http://www.dir001.com/list/109"
        , "title": "音乐", "count": 80},
    "110": {"id": 110, "parent": 1, "href": "http://www.dir001.com/list/110"
        , "title": "配饰", "count": 94},
    "111": {"id": 111, "parent": 1, "href": "http://www.dir001.com/list/111"
        , "title": "家居", "count": 146},
    "112": {"id": 112, "parent": 1, "href": "http://www.dir001.com/list/112"
        , "title": "母婴", "count": 57},
    "113": {"id": 113, "parent": 1, "href": "http://www.dir001.com/list/113"
        , "title": "食品", "count": 145},
    "114": {"id": 114, "parent": 1, "href": "http://www.dir001.com/list/114"
        , "title": "虚拟产品", "count": 21},
    "116": {"id": 116, "parent": 0, "href": "http://www.dir001.com/list/116"
        , "title": "时尚", "count": 164},
    "117": {"id": 117, "parent": 0, "href": "http://www.dir001.com/list/117"
        , "title": "小说", "count": 205},
    "118": {"id": 118, "parent": 4, "href": "http://www.dir001.com/list/118"
        , "title": "摄影摄像", "count": 253},
    "119": {"id": 119, "parent": 1, "href": "http://www.dir001.com/list/119"
        , "title": "团购", "count": 166}
}

if __name__ == '__main__':
    # test URLProvider
    GetClasses().run()

"""
var all = {};
var total = 0;
var parentid = 0;
function fetch(dl)
{
    for (var i = 0; i < dl.children.length - 2; i += 2)
    {
        fetchArea(dl.children[i], dl.children[i+1]);
    }
}
function fetchArea(dt, dd)
{
    var parent = {
        id : parentid++,
        title: dt.innerText,
    };
    all[parent.id] = parent;
    var subcats = [];
    var ul = dd.firstElementChild;
    for (var i = 0; i < ul.children.length; ++i)
    {
        var li = ul.children[i];
        var info = li.firstElementChild.href.split('/');
        var id = parseInt(info[info.length - 1]);
        var cat = {
            id : id,
            parent: parent.id,
            href : li.firstElementChild.href,
            title : li.firstElementChild.innerText.replace(/\(\d+\)/g, ''),
            count : parseInt(li.firstElementChild.innerText.replace(/^.*\((\d+)\).*$/g, '$1'))
        };
        if (id in all)
        {
            console.log(id + " already exists");
        }
        total += 1;
        all[id] = cat;
    }
}
fetch(document.getElementsByTagName('dl')[0]);
"""
