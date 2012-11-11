#coding:utf-8

if __name__ == '__main__':
    import os, sys
    src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
    if src_path not in sys.path:
        sys.path.append(src_path)
import zmq, struct
from config import sysconfig, config, logger

class Tokenizer(object):

    _SERVER_CHARSET = 'utf-8'

    actions = {
        'marve' : 11,
        'split' : 12,
        'count' : 13,
        'compare' : 14,
    }

    _instance = None

    @classmethod
    def instance(cls):
        """
        @return Tokenizer
        """
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self, server=config.getTokenizer()):
        self.sock = sysconfig.zmq_context.socket(zmq.REQ)
        self.sock.connect(server)
        self.actionPacker = struct.Struct('B')

    def marve(self, content):
        return self.request(content=content, action='marve')

    def count(self, content, raw=False):
        return self.request(content=content, action='count')

    def split(self, content):
        return self.request(content=content, action='split')

    def raw_request(self, content, action):
        try:
            data = self.actionPacker.pack(self.actions[action]) + (content.encode(self._SERVER_CHARSET) if isinstance(content, unicode) else content)
            return self._request(data=data)
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))

    def request(self, content, action):
        try:
            data = self.actionPacker.pack(self.actions[action]) + (content.encode(self._SERVER_CHARSET) if isinstance(content, unicode) else content)
            res = self._request(data=data)
            return config.msgpack.decode(res)
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(content)) + str(e.args))

    def _request(self, data):
        self.sock.send(data)
        return self.sock.recv()

if __name__ == '__main__':
    from time import time
    from utils.misc import export
    if len(sys.argv) == 1:
        print """usage: %s <content="test"> [request-times=1]""" % sys.argv[0]
        sys.exit(1)
    tokenizer = Tokenizer.instance()
    print "cws-server:[%s]" % config.getTokenizer()
    testContent = (u'存储容量 dzwww').encode(Tokenizer._SERVER_CHARSET)
    testContent = '存存储容量 fcukyoudzwww' * 3
    testContent = '随机变量X 的分布称为泊松分布，记作P(λ)。这个分布是S.-D.泊松研究二项分布的渐近公式时提出来的。泊松分布P (λ)中只有一个参数λ ，它既是泊松分布的均值，也是泊松分布的方差。在实际事例中，当一个随机事件，例如某电话交换台收到的呼叫、来到某公共汽车站的乘客、某放射性物质发射出的粒子、显微镜下某区域中的白血球等等，以固定的平均瞬时速率 λ(或称密度)随机且独立地出现时，那么这个事件在单位时间（面积或体积）内出现的次数或个数就近似地服从泊松分布。因此泊松分布在管理科学，运筹学以及自然科学的某些问题中都占有重要的地位。'
    testContent = "然而，她不但比他大整整10岁，还是个带着4个孩子的寡妇，闲言碎语如同一张无形的大网紧紧地笼罩在“大逆不道”的他们头上。他们喘口气的力气都快没有了。 于是，1956年8月一天早上，村里人发现她和4个孩子突然失踪了，同时失踪的还有19岁的他。40多年后，2001年的中秋，一队户外旅行社在原始森林探险时发现罕无人迹的高山深处竟然住着两位老人。他们仿佛生活在刀耕火种的原始社会，点的是他亲手做的煤油灯，住的是简陋的泥房。而以前没有泥房子时住的是山洞。在自己开垦的田地上耕种，自给自足。他们就是几十年前失踪的他和她。"
    testContent = """qq空间 黄钻 免费装扮 开心农场 qq农场 qq牧场 如果您看到这个提示 说明qq空间无法正常打开 请尝试 使用空间小助手修复 用读屏软件的朋友从这里进入qq空间读屏版 如果您读屏遇到问题 点击这里进行反馈 跳到内容区 qq空间6.0 个人中心 我的主页 应用 装扮 续费 升级年费 向好友索取 赠送给好友 黄钻官网 消息通知 短消息 目前没有消息 查看更多消息 1 修改资料 主页排版 空间设置 好友管理 在线客服 77 退出 您已经隐藏了所有的应用 您没有最近使用的应用 您还没有隐藏的应用 http 418799568.qzone.qq.com 77 升级年费 我的好友 好友动态 特别关心 与我相关 实名朋友 腾讯微博 认证空间 应用中心 影视中心 1 0 最近使用 情侣主页 欢乐淘 你画我猜 切水果 情侣头像 宝贝战争 十幅图 机场大亨 好友问问 好友买卖 我的全部应用 添加新应用 现在就可以写说说了 快试试 表情 图片 音乐 视频 模板 预览 更多 发表 好友最新原创 全部 作者 羽嘉 作者 云儿 作者 苏怜 我的空间动态 我参与的 好友动态 全部 日志 相册 查看更多推荐好友 动态排序 动态过滤 关注动态 关注好友 动态隐私 新动态提示 显示非好友评论 选择qq群 选择群 袁莉 你是最疼我的 但是你永远都是最先离开我的. 14 33 来自qq签名 赞 评论 转发 我也说一句 隐藏此人信息 隐藏此类信息 举报 官方qzone 奶茶被曝含重金属 奶香飘飘毒如砒霜 街边奶茶究竟是什么东西做的 奶精对人体危害你知多少 美味的 珍珠 是淀粉 还是塑料 点击查看 伤不起 0 10 40 通过qq空间 分享 我也说一句 隐藏此类信息 何总 换了一套 还是会寂寞 主题的 不了 装扮 让自己的空间与众不同 更多推荐 14 19 赞 458 评论 分享 我也要换装 458人 觉得很赞 我也说一句 隐藏此人信息 隐藏此类信息 楚楚街 路是自己选的 不论前方如何 只有自己去走完它 感到无法继续前行的时候 停下来 想想自己当初是怎么走到这里 13 36 赞 1192 评论 1615 转发 10704 1192人 觉得很赞 我也说一句 取消关注 贪贪 10月30日生日 收到 生日礼物 彩虹蛋糕 送你美味的彩虹蛋糕 希望你的生活像彩虹一样多姿多彩 共收到5份 生日礼物 13 05 赞 赠送礼物 查看全部礼物 隐藏此人信息 隐藏此类信息 猪猪 瞬間無語了 這都是神馬跟神馬啊 12 37 来自 qq空间手机版 赞 评论 1 转发 你shi我de 云朵 怎么啦 13 08 回复 我也说一句 隐藏此人信息 隐藏此类信息 举报 查看更多动态 查看更多动态 数据加载中 请稍候... 日期 10.29 周一 day 3 签到 写日志 传照片 发视频 热门话题 蔡依林新专辑惹议 hot 蔡依林新专辑mv曝光 变身现代夏娃 57岁环卫工遭暴打 hot 57岁环卫工阻止乱倒垃圾 遭暴打 央视批信用卡高额罚息 hot 央视炮轰罚息 透支11万5年还44万 更多 精彩生活 loading... 好友生日 可能认识的人 最近访客 功能推荐 应用标题 右侧操作区 返回个人中心 黄钻贵族 官方qzone 官方微博 qq互联 认证服务 腾讯博客 腾讯客服 complaint guidelines 粤网文 2011 0483-070号 copyright 2005 - 2012 tencent. all rights reserved. 腾讯公司 版权所有 主页 日志 留言板 相册 个人档 更多"""
    content = sys.argv[1] if sys.argv[1] != 'test' else testContent
    times = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    res = tokenizer.count(content=content)
    print "standard result: ", res
    begin = time()
    for i in xrange(0, times):
        assert tokenizer.count(content=content) == res, "requests with same parameter got different results"
    consumed = time() - begin
    print "finished %d requests in %f seconds" % (times, consumed)
    print "QPS: " + (str(times / consumed) if consumed > 0 else 'infinite')
    print "BPS: " + (str(times * len(testContent) / consumed) if consumed > 0 else 'infinite')
    print
    export(res)
