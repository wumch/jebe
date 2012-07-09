#coding:utf-8

from __future__ import division
import os
from math import log

class PhraseList(object):
    def __init__(self, size):
        self.__size = size
        self.phrases = {}

    @property
    def size(self):
        return self.__size

    def addPhrase(self, phrase):
        if phrase not in self.phrases:
            self.phrases[phrase] = 0
        self.phrases[phrase] += 1

    def countPrefix(self, prefix):
        plen = len(prefix)
        assert plen < self.size
        pnum, acount = 0, 0
        for phrase in self.phrases:
            if phrase[:plen] == prefix:
                pnum += 1
                acount += self.phrases[phrase]
        return pnum, acount

    def count(self, phrase):
        return self.phrases[phrase] if phrase in self.phrases else 0

    def __repr__(self):
        return str(self.phrases) + os.linesep

class DictBuilder(object):
    def __init__(self, maxWordLen = 7):
        self.maxWordLen = maxWordLen
        self.phraseLists = [PhraseList(i + 1) for i in range(0, self.maxWordLen)]

    def scan(self, doc):
        content = unicode(doc, 'utf-8')
        cursor, n = 0, len(content)
        while cursor < n - self.maxWordLen:
            for offset in range(0, self.maxWordLen):
                self.phraseLists[offset].addPhrase(content[cursor:cursor+offset+1])
            cursor += 1
        while cursor < n:
            offset = 0
            while cursor + offset < n:
                self.phraseLists[offset].addPhrase(content[cursor:cursor+offset+1])
                offset += 1
            cursor += 1

    def __repr__(self):
        pcount, acount = 0, 0
        for plist in self.phraseLists:
            for phrase, count in plist.phrases.iteritems():
                print phrase, "\t", count
                pcount += 1
                acount += count
            print
        return str((pcount, acount))

class WordJudger(object):

    def __init__(self, phraseLists, joinThreshold = 30, entropyThreshold = 0.5):
        self.phraseLists = phraseLists
        self.entropyThreshold = entropyThreshold
        self.joinThreshold = joinThreshold
        self.phraseNumList = [sum(pl.phrases.values()) for pl in self.phraseLists]
        self.charNum = self.phraseNumList[0]

    def count(self, phrase):
        return self.phraseLists[len(phrase) - 1].count(phrase)

    def getSuffixesCount(self, main, correctSuffix):
        phrase = main + correctSuffix
        plen = len(phrase)
        res = []
        phrases = self.phraseLists[plen - 1].phrases
        for p in phrases:
            if p[:len(main)] == main:
                if p != phrase:
                    res.append(phrases[p])
                else:
                    res.insert(0, phrases[p])
        return res

    def judge(self, main, suffix):
        phrase = main + suffix
        global results
        res = [phrase]
        main_count = self.count(main)
        suffix_count = self.count(suffix)
        count = self.count(phrase)
        phraseNum = self.phraseNumList[len(phrase) - 1]     # 等长phrase出现次数
        joinedProb = count
        predNum = main_count * (suffix_count / self.charNum) # 前验概率
        res.append(count / predNum)
        if count / predNum >= self.joinThreshold:
            res.append(0.0)
            res.append(True)
            results.append(res)
            return True

        suffixesCount = self.getSuffixesCount(main, suffix)
        totalSuffixCount = sum(suffixesCount)
        avgEntropy = 0
        tmpProb = 0
        for scount in suffixesCount:
            tmpProb = scount / totalSuffixCount
            avgEntropy -= tmpProb * log(tmpProb)
        res.append(avgEntropy)
        r = avgEntropy <= self.entropyThreshold
        res.append(r)
        results.append(res)
        return r

    def profile(self, phrases):
        for phrase in phrases:
            print phrase, ":", self.phraseLists[len(phrase) - 1].phrases[phrase]
        phrase = ''.join(phrases)
        print phrase, ":", self.phraseLists[len(phrase) - 1].phrases[phrase]

results = []
if __name__ == '__main__':
    contents = [
#        '广告投放中，并不是频次越高越好，过少的接触不会在接触的用户心中产生印象，过多的接触反而会使接触的用户产生不快，厌恶。1972年，美国心理学家赫尔伯特.克鲁格曼经过研究，确立了消费者接触广告三次的心理学关系：第一次好奇：“这是什么？”第二次是认识：“干什么用的？”第三次是判断：“对广告产生什么印象？”。当然，因为产品、市场、品牌、竞争、创意以及媒体等不同，在频次设置上也会有所不同，不过，对广告的有效接触频次限定一般都是以3次为底限的。',
#        '网络广告频次控制的原理非常简单。当用户通过浏览器访问页面时，会请求放置在页面的广告位代码，广告位代码和服务器进行交互，广告位代码将用户的cookie信息（包含对广告的访问次数）传给服务器（如果没有cookie，服务器会生成一个），服务器进行频次的匹配，超过频次设定的广告将不会被投放，在同时判断了其他定向条件后，服务器回传适合的广告到浏览器进行投放，在返回信息的同时，还会将用户cookie上此广告的浏览次数加1。通过这种方式，网络广告实现了精确的频次控制。',
#        '在网络广告的投放中，频次的控制对象比其他媒介更广泛，频次可以控制广告的浏览、点击、完整浏览，甚至是广告的转发、下载等其他的行为，因此互联网的频次指的是访客与广告发生互动的最高次数，而互动的行为设定则需要能够在广告系统中进行设置。当然经常还是对广告的浏览进行频次设置，我们也以此举例。',
#        '频次是指个人或家庭接触广告信息的次数。在传统的电视媒介中，我们不能准确的控制每一个人接触广告信息的次数，只能是通过总收视点除以到达率计算得出。但是在网络广告中，一个人可以接触广告信息的最高频次是可以严格控制的，实现严格控制的基础技术也是cookie，可见cookie对于互联网广告精准投放的重要性。',
        '''对中文资料进行自然语言处理时，我们会遇到很多其他语言不会有的困难，例如分词——汉语的词与词之间没有空格，那计算机怎么才知道“已结婚的和尚未结婚的”究竟是“已／结婚／的／和／尚未／结婚／的”，还是“已／结婚／的／和尚／未／结婚／的”呢？

        这就是所谓的分词歧义难题。不过，现在很多语言模型都已能比较漂亮地解决这一问题了。但在中文分词领域里，还有一个比分词歧义更令人头疼的东西——未登录词。中文没有首字母大写，专名号也被取消了，这叫计算机如何辨认人名地名之类的东西？更惨的则是机构名、品牌名、专业名词、缩略语、网络新词等，它们的产生机制似乎完全无规律可寻。最近十年来，中文分词领域都在集中攻克这一难关，自动发现新词成为关键的环节。

        挖掘新词的传统方法是，先对文本进行分词，然后猜测未能成功匹配的剩余片段就是新词。这似乎陷入了一个怪圈：分词的准确性本身就依赖于词库的完整性，如果词库中根本没有新词，我们又怎能信任分词结果呢？

        此时，一种大胆的想法是，首先不依赖于任何已有的词库，仅仅根据词的共同特征，将一段大规模语料中可能成词的文本片段全部提取出来，不管它是新词还是旧词。然后，再把所有抽出来的词和已有词库进行比较，不就能找出新词了吗？有了抽词算法后，我们还能以词为单位做更多有趣的数据挖掘工作。这里，我所选用的语料是人人网2011年12月前半个月部分用户的状态。

        成词标准之一：内部凝固程度

        要想从一段文本中抽出词来，我们的第一个问题就是，怎样的文本片段才算一个词？大家想到的第一个标准或许是，看这个文本片段出现的次数是否足够多。我们可以把所有出现频数超过某个阈值的片段提取出来，作为该语料中的词汇输出。不过，光是出现频数高还不够，一个经常出现的文本片段有可能不是一个词，而是多个词构成的词组。在人人网用户状态中，“的电影”出现了389次，“电影院”只出现了175次，然而我们却更倾向于把“电影院”当作一个词，因为直觉上看，“电影”和“院”凝固得更紧一些。

        为了证明“电影院”一词的内部凝固程度确实很高，我们可以计算一下，如果“电影”和“院”真的是各自独立地在文本中随机出现，它俩正好拼到一起的概率会有多小。在整个2400万字的数据中，“电影”一共出现了2774次，出现的概率约为0.000113。“院”字则出现了4797次，出现的概率约为0.0001969。如果两者之间真的毫无关系，它们恰好拼在了一起的概率就应该是0.000113×0.0001969，约为2.223乘以10的–8次方。但事实上，“电影院”在语料中一共出现了175次，出现概率约为7.183乘以10的–6次方，是预测值的300多倍。类似地，统计可得“的”字的出现概率约为0.0166，因而“的”和“电影”随机组合到了一起的理论概率值为0.0166×0.000113，约为1.875乘以10的–6次方，这与“的电影”出现的真实概率很接近——真实概率约为1.6乘以10的–5次方，是预测值的8.5倍。计算结果表明，“电影院”可能是一个更有意义的搭配，而“的电影”则更像是“的”和“电影”这两个成分偶然拼到一起的。

        当然，作为一个无知识库的抽词程序，我们并不知道“电影院”是“电影”加“院”得来的，也并不知道“的电影”是“的”加上“电影”得来的。错误的切分方法会过高地估计该片段的凝合程度。如果我们把“电影院”看作是“电”加“影院”所得，由此得到的凝合程度会更高一些。因此，为了算出一个文本片段的凝合程度，我们需要枚举它的凝合方式——这个文本片段是由哪两部分组合而来的。令p(x)为文本片段x在整个语料中出现的概率，那么我们定义“电影院”的凝合程度就是p(电影院)与p(电)·p(影院)的比值和p(电影院)与p(电影)·p(院)的比值中的较小值，“的电影”的凝合程度则是p(的电影)分别除以p(的)·p(电影)和p(的电)·p(影)所得的商的较小值。

        可以想到，凝合程度最高的文本片段就是诸如“蝙蝠”、“蜘蛛”、“彷徨”、“忐忑”之类的词了，这些词里的每一个字几乎总是会和另一个字同时出现，从不在其他场合中使用。

        成词标准之二：自由运用程度

        光看文本片段内部的凝合程度还不够，我们还需要从整体来看它在外部的表现。考虑“被子”和“辈子”这两个片段。我们可以说“买被子”、“盖被子”、“进被子”、“好被子”、“这被子”等，在“被子”前面加各种字；但“辈子”的用法却非常固定，除了“一辈子”、“这辈子”、“上辈子”、“下辈子”，基本上“辈子”前面不能加别的字了。“辈子”这个文本片段左边可以出现的字太有限，以至于直觉上我们可能会认为，“辈子”并不单独成词，真正成词的其实是“一辈子”、“这辈子”之类的整体。可见，文本片段的自由运用程度也是判断它是否成词的重要标准。如果一个文本片段能够算作一个词的话，它应该能够灵活地出现在各种不同的环境中，具有非常丰富的左邻字集合和右邻字集合。

        “信息熵”是一个非常神奇的概念，它能够反映一个事件的结果平均会给你带来多大的信息量。如果某个结果的发生概率为p，当你知道它确实发生了，你得到的信息量就被定义为-log(p)。p越小，你得到的信息量就越大。如果一颗骰子的六个面分别是1、1、1、2、2、3，那么你知道了投掷的结果是1时可能并不会那么吃惊，它给你带来的信息量是-log(1/2)，约为0.693。知道投掷结果是2，给你带来的信息量则是-log(1/3)≈1.0986。知道投掷结果是3，给你带来的信息量则有-log(1/6)≈1.79。但你只有1/2的机会得到0.693的信息量，只有1/3的机会得到1.0986的信息量，只有1/6的机会得到1.79的信息量，因而平均情况下你会得到0.693/2+1.0986/3+1.79/6≈1.0114的信息量。这个1.0114就是那颗骰子的信息熵。现在，假如某颗骰子有100个面，其中99个面都是1，只有一个面上写的2。知道骰子的抛掷结果是2会给你带来一个巨大无比的信息量，它等于-log(1/100)，约为4.605；但你只有1%的概率获取到这么大的信息量，其他情况下你只能得到-log(99/100)≈0.01005的信息量。平均情况下，你只能获得0.056的信息量，这就是这颗骰子的信息熵。再考虑一个最极端的情况：如果一颗骰子的六个面都是1，投掷它不会给你带来任何信息，它的信息熵为-log(1)=0。什么时候信息熵会更大呢？换句话说，发生了怎样的事件之后，你最想问一下它的结果如何？直觉上看，当然就是那些结果最不确定的事件。没错，信息熵直观地反映了一个事件的结果有多么的随机。

        我们用信息熵来衡量一个文本片段的左邻字集合和右邻字集合有多随机。考虑这么一句话“吃葡萄不吐葡萄皮不吃葡萄倒吐葡萄皮”，“葡萄”一词出现了四次，其中左邻字分别为{吃, 吐, 吃, 吐}，右邻字分别为{不, 皮, 倒, 皮}。根据公式，“葡萄”一词的左邻字的信息熵为-(1/2)·log(1/2)-(1/2)·log(1/2)≈0.693，它的右邻字的信息熵则为-(1/2)·log(1/2)-(1/4)·log(1/4)-(1/4)·log(1/4)≈1.04。可见，在这个句子中，“葡萄”一词的右邻字更加丰富一些。

        凝固程度和自由程度，两种判断标准缺一不可。若只看前者，程序会找出实际上是“半个词”的片段；若只看后者，程序则会找出诸如“了一”、“的电影”一类的“垃圾词组”。

        在人人网用户状态中，“被子”一词一共出现了956次，“辈子”一词一共出现了2330次，两者的右邻字集合的信息熵分别为3.87404和4.11644，数值上非常接近。但“被子”的左邻字用例非常丰富：用得最多的是“晒被子”，它一共出现了162次；其次是“的被子”，出现了85次；接下来分别是“条被子”、“在被子”、“床被子”，分别出现了69次、64次和52次；当然，还有“叠被子”、“盖被子”、“加被子”、“新被子”、“掀被子”、“收被子”、“薄被子”、“踢被子”、“抢被子”等100多种不同的用法构成的长尾。所有左邻字的信息熵为3.67453。但“辈子”的左邻字就很可怜了，2330个“辈子”中有1276个是“一辈子”，有596个“这辈子”，有235个“下辈子”，有149个“上辈子”，有32个“半辈子”，有10个“八辈子”，有7个“几辈子”，有6个“哪辈子”，以及“n辈子”、“两辈子”等13种更罕见的用法。所有左邻字的信息熵仅为1.25963。因而，“辈子”能否成词，明显就有争议了。“下子”则是更典型的例子，310个“下子”的用例中有294个出自“一下子”，5个出自“两下子”，5个出自“这下子”，其余的都是只出现过一次的罕见用法。事实上，“下子”的左邻字信息熵仅为0.294421，我们不应该把它看作一个能灵活运用的词。当然，一些文本片段的左邻字没啥问题，右邻字用例却非常贫乏，例如“交响”、“后遗”、“鹅卵”等，把它们看作单独的词似乎也不太合适。我们不妨就把一个文本片段的自由运用程度定义为它的左邻字信息熵和右邻字信息熵中的较小值。

        在实际运用中你会发现，文本片段的凝固程度和自由程度，两种判断标准缺一不可。只看凝固程度的话，程序会找出“巧克”、“俄罗”、“颜六色”、“柴可夫”等实际上是“半个词”的片段；只看自由程度的话，程序则会把“吃了一顿”、“看了一遍”、“睡了一晚”、“去了一趟”中的“了一”提取出来，因为它的左右邻字都太丰富了。''',
    ]
    builder = DictBuilder()
    for doc in contents:
        builder.scan(doc)
#    print builder

    judger = WordJudger(builder.phraseLists)

    print len(builder.phraseLists[1].phrases)
    for phrase in builder.phraseLists[3].phrases:
        judger.judge(phrase[:-1], phrase[1])

    def campare(r1, r2):
        if r1[3] != r2[3]:
            return cmp(r1[3], r2[3])
        if r1[2] == 0 and r2[2] == 0:
            return cmp(r1[1], r2[1])
        elif r1[2] == 0 or r2[2] == 0:
            return -cmp(r1[2], r2[2])
        else:
            return cmp(r1[2], r2[2])

    results.sort(cmp=campare)
    for res in results:
        if res[3]:
            print "%s\t%-.20f\t%-.20f\t" % (res[0].encode('utf-8'), res[1], res[2], )
