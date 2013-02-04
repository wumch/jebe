#!/usr/bin/env python
#coding:utf-8

import re
s = r'<li class="domain"><strong>域名: </strong><a href=".*" class="external" target="_blank">.*</a></li>'

regexp = re.compile(s);

content = '''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
<link rel="stylesheet" type="text/css" href="/css/pager.css" />
<title>星座 - 休闲娱乐 - 第一分类目录</title>
<meta name="description" content="休闲娱乐星座网站分类目录导航" />
<meta name="keywords" content="休闲娱乐星座" />
<meta name="copyright" content="Copyright 2009-2012 - 第一分类目录|www.dir001.com|免费快速提升网站流量的平台" />
<meta name="author" content="by Josh" />
<link rel="icon" href="/favicon.ico" type="image/x-icon" />
<link rel="stylesheet" type="text/css" href="/css/base.css" />
<script src="/misc/common.js" type="text/javascript"></script>
</head>
<body>
<div id="header">
  <div id="top-tool">
    <div class="description">第一分类目录免费收录各类优秀网站，快速提升网站流量</div>
    <div class="help">
            <a onclick="fAddFavorite('第一分类目录免费收录各类优秀网站！','http://www.dir001.com/')" href="javascript:void(0);">收藏本站</a> - <a href="/sites/add">网站提交</a>
    </div>
  </div>
  <div id="top-logo">
    <div class="logo"><a href="http://www.dir001.com/"><img src="/img/logo.png" alt="第一分类目录" /></a></div>
    <div class="top-banner" style="padding:10px 0 0 160px; width:510px; height:49px; overflow:hidden;">
            </div>
  </div>

  <div class="nav_1029">
    <ul class="nav_1029_ul">
      <li class="no_bg"><a href="/">首页</a></li>
      <li><a href="/list/1">休闲娱乐</a></li>
      <li><a href="/list/108">网上商城</a></li>
      <li><a href="/list/2">网络服务</a></li>
      <li><a href="/list/3">商业经济</a></li>
      <li><a href="/list/4">生活服务</a></li>
      <li><a href="/list/5">教育文化</a></li>
      <li><a href="/list/6">博客论坛</a></li>
      <li><a href="/list/7">综合其他</a></li>
      <li><a href="/sites/new/1">最新收录</a></li>
      <li><a href="/category">分类浏览</a></li>
    </ul>
    <span class="keywords_label">站内搜索:</span>

    <form method="get" action="/s" onsubmit="return check();" style="z-index: -1;">
      <div class="form-box">
        <input name="q" id="searchbox1" onblur="if(this.value=='') {this.style.color='#D8D8D8'; this.value='搜索超过70万个站点信息';}" onclick="this.style.color='#000';if(this.value=='搜索超过70万个站点信息') this.value='';" autocomplete="off" onkeypress="if(event.keyCode==13) check();" value="搜索超过70万个站点信息" maxlength="26" type="text" /><button type="submit" id="btnSearch">全文检索</button>
      </div>
    </form>
    <p class="nav_1029_news"><span>热搜:</span><a href="/area/1">北京</a>|<a href="/area/2">上海</a>|<a href="/area/275">深圳</a>|<a href="/s?q=游戏" title="游戏">游戏</a>|<a href="/s?q=商城" title="商城">商城</a>|<a href="/s?q=人才" title="人才">人才</a>|<a href="/s?q=淘宝" title="淘宝">淘宝</a>|<a href="/s?q=交友">交友</a>|<a href="/s?q=艺术">艺术</a>|<a href="/s?q=礼品">礼品</a></p>

  </div>

</div>


<div id="container" class="ss-list">
  <div class="main">
    <div class="box">
      <h2>分类: 
        <a href="/list/1"><font class="c_green">休闲娱乐</font></a> &gt; 
        <a href="/list/12"><font class="c_green">星座</font></a>
      </h2>

      <div class="content">


        <div class="siteList">
          <ul>

            <li>
              <a href="http://www.nongli.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.nongli.com" alt="中华农历网"/></a>

              <h3><a alt="点击查看&#13;中华农历网" title="点击查看&#13;中华农历网" href="/www/nongli.com" target="_blank">中华农历网</a> <span>www.nongli.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">540</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/nongli.com" class="external" target="_blank">http://www.nongli.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank6.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">24,839</b></li>
              </ul>
              <p>　　中华农历网</p>
            </li>
            <li>
              <a href="http://www.go108.com.cn" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.go108.com.cn" alt="科技紫微星座网"/></a>

              <h3><a alt="点击查看&#13;科技紫微星座网" title="点击查看&#13;科技紫微星座网" href="/www/go108.com.cn" target="_blank">科技紫微星座网</a> <span>www.go108.com.cn</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">682</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/go108.com.cn" class="external" target="_blank">http://www.go108.com.cn</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank6.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">61,562</b></li>
              </ul>
              <p>　　诚信正规的星座命理网站</p>
            </li>
            <li>
              <a href="http://www.51jiemeng.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.51jiemeng.com" alt="周公解梦"/></a>

              <h3><a alt="点击查看&#13;周公解梦" title="点击查看&#13;周公解梦" href="/www/51jiemeng.com" target="_blank">周公解梦</a> <span>www.51jiemeng.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">579</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/51jiemeng.com" class="external" target="_blank">http://www.51jiemeng.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank6.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">19,654</b></li>
              </ul>
              <p>　　周公解梦,为您解梦！专业、免费的在线周公解梦网站</p>
            </li>
            <li>
              <a href="http://www.qmw.cc" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.qmw.cc" alt="宝宝起名网"/></a>

              <h3><a alt="点击查看&#13;宝宝起名网" title="点击查看&#13;宝宝起名网" href="/www/qmw.cc" target="_blank">宝宝起名网</a> <span>www.qmw.cc</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">556</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/qmw.cc" class="external" target="_blank">http://www.qmw.cc</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank3.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">--</b></li>
              </ul>
              <p>　　宝宝起名网是专门提供宝宝起名知识的专业网站，您可以学到起名知识，起名技巧，起名方法等。</p>
            </li>
            <li>
              <a href="http://www.xingzuo360.cn" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.xingzuo360.cn" alt="360星座网"/></a>

              <h3><a alt="点击查看&#13;360星座网" title="点击查看&#13;360星座网" href="/www/xingzuo360.cn" target="_blank">360星座网</a> <span>www.xingzuo360.cn</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">963</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/xingzuo360.cn" class="external" target="_blank">http://www.xingzuo360.cn</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank4.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">--</b></li>
              </ul>
              <p>　　提供星座查询，星座配对，星座运势，八字，面相，风水，等方面的服务。</p>
            </li>
            <li>
              <a href="http://www.xingzuo12.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.xingzuo12.com" alt="12星座网"/></a>

              <h3><a alt="点击查看&#13;12星座网" title="点击查看&#13;12星座网" href="/www/xingzuo12.com" target="_blank">12星座网</a> <span>www.xingzuo12.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">479</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/xingzuo12.com" class="external" target="_blank">http://www.xingzuo12.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank3.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">2,494,442</b></li>
              </ul>
              <p>　　星座爱好者之家,提供时尚个性的星座咨询以及趣味测试.</p>
            </li>
            <li>
              <a href="http://www.k366.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.k366.com" alt="华易算命网"/></a>

              <h3><a alt="点击查看&#13;华易算命网" title="点击查看&#13;华易算命网" href="/www/k366.com" target="_blank">华易算命网</a> <span>www.k366.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">1176</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/k366.com" class="external" target="_blank">http://www.k366.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank5.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">37,139</b></li>
              </ul>
              <p>　　在线免费算命,周易预测，姓名占卜.</p>
            </li>
            <li>
              <a href="http://www.d1xz.net" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.d1xz.net" alt="第1星座"/></a>

              <h3><a alt="点击查看&#13;第1星座" title="点击查看&#13;第1星座" href="/www/d1xz.net" target="_blank">第1星座</a> <span>www.d1xz.net</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">863</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/d1xz.net" class="external" target="_blank">http://www.d1xz.net</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank5.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">37,278</b></li>
              </ul>
              <p>　　第一星座网，专业的星座命理网</p>
            </li>
            <li>
              <a href="http://www.99166.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.99166.com" alt="非常运势网"/></a>

              <h3><a alt="点击查看&#13;非常运势网" title="点击查看&#13;非常运势网" href="/www/99166.com" target="_blank">非常运势网</a> <span>www.99166.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">974</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/99166.com" class="external" target="_blank">http://www.99166.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank5.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">40,500</b></li>
              </ul>
              <p>　　专业的综合命理网站，提供免费测算，在线测算，大师亲算等全方位服务</p>
            </li>
            <li>
              <a href="http://www.mengjiexi.com" class="playpic" target="_blank" rel="nofollow"><img width="120" height="90" border="1" src="http://open.thumbshots.org/image.pxf?url=http://www.mengjiexi.com" alt="周公解梦网"/></a>

              <h3><a alt="点击查看&#13;周公解梦网" title="点击查看&#13;周公解梦网" href="/www/mengjiexi.com" target="_blank">周公解梦网</a> <span>www.mengjiexi.com</span> &nbsp;&nbsp;<span style="font-size:12px;">查看次数:</span><span class="score">714</span></h3>
              <ul>
                <li class="domain"><strong>域名: </strong><a href="/www/mengjiexi.com" class="external" target="_blank">http://www.mengjiexi.com</a></li>
                <li><strong>PR: </strong><img width="58" height="11" align="absmiddle" src="/img/googlerank/pagerank5.gif" /></li>
                <li><strong>AlexaRank: </strong><b class="alexarank">306,507</b></li>
              </ul>
              <p>　　周公解梦,解梦,原版周公解梦大全,现代周公解梦,专业、免费的在线周公解梦网，国内收录最齐全的周公解梦网站。</p>
            </li>

          </ul>
        </div>

		<div class="pagelink">
			<ul id="yw0" class="yiiPager"><li class="first hidden"><a href="/list/12">&lt;&lt; 首页</a></li>
<li class="previous hidden"><a href="/list/12">&lt; 上页</a></li>
<li class="page selected"><a href="/list/12">1</a></li>
<li class="page"><a href="/list/12/2">2</a></li>
<li class="page"><a href="/list/12/3">3</a></li>
<li class="page"><a href="/list/12/4">4</a></li>
<li class="page"><a href="/list/12/5">5</a></li>
<li class="page"><a href="/list/12/6">6</a></li>
<li class="page"><a href="/list/12/7">7</a></li>
<li class="page"><a href="/list/12/8">8</a></li>
<li class="next"><a href="/list/12/2">&gt; 下页</a></li>
<li class="last"><a href="/list/12/8">&gt;&gt; 尾页</a></li></ul>		</div>
      </div>
    </div>
  </div>

  <div class="sideR">
    <div class="box">
      <h2>网站分类</h2>
      <div class="content">

        <dl class="list-catalog">
          <dt class="c_red">休闲娱乐(2283)</dt>
          <dd id="div_cur">
            <a href="/list/8">电影<span>(308)</span></a>
            <a href="/list/109">音乐<span>(80)</span></a>
            <a href="/list/9">动漫<span>(48)</span></a>
            <a href="/list/23">游戏<span>(371)</span></a>
            <a href="/list/35">视频<span>(41)</span></a>
            <a href="/list/106">社区<span>(103)</span></a>
            <a href="/list/11">明星<span>(38)</span></a>
            <a href="/list/14">交友<span>(89)</span></a>
            <a href="/list/116">时尚<span>(165)</span></a>
            <a href="/list/117">小说<span>(212)</span></a>
            <a href="/list/15">笑话<span>(110)</span></a>
            <a href="/list/16">体育<span>(150)</span></a>
            <a href="/list/12" class="c_red">星座<span>(77)</span></a>
            <a href="/list/13">收集珍藏<span>(52)</span></a>
            <a href="/list/10">图片壁纸<span>(76)</span></a>
            <a href="/list/17">其他<span>(342)</span></a>
          </dd>
        </dl>

        <div class="ggadR">
            <script type="text/javascript"> var cpro_id = 'u265516';</script><script type="text/javascript" src="http://cpro.baidu.com/cpro/ui/c.js"></script>
        </div>

        <div class="ggadR">
            <script type="text/javascript">/*160*600，创建于2012-9-9*/ var cpro_id = 'u1055204';</script><script src="http://cpro.baidustatic.com/cpro/ui/c.js" type="text/javascript"></script>
        </div>

        <div class="ggadR">
            <script type="text/javascript">/*200*200，创建于2012-9-9*/ var cpro_id = 'u1055206';</script><script src="http://cpro.baidustatic.com/cpro/ui/c.js" type="text/javascript"></script>
        </div>

        <div class="ggadR">
                    </div>
      </div>
    </div>
  </div>

</div>

<div style="margin:0 auto 8px; clear:both; overflow:hidden; width:960px; height:90px;">
    <script type="text/javascript">/*960*90，创建于2012-9-10*/ var cpro_id = 'u1055222';</script><script src="http://cpro.baidustatic.com/cpro/ui/c.js" type="text/javascript"></script>
</div>

<div id="footer">

	<div class="f-nav"><a href="/about">网站简介</a><span>|</span><a href="/duty">免责声明</a><span>|</span><a href="/contact">联系我们</a><span>|</span><a href="/sites/add">网站提交</a></div>

	<div class="copyright">
		<p>&copy; 2009-2012 <a href="http://www.dir001.com/">第一分类目录</a> dir001.com 版权所有 粤ICP备09194960号  <script src="http://s65.cnzz.com/stat.php?id=1752431&web_id=1752431" language="JavaScript" charset="gb2312"></script></p>
	</div>

	<div class="safe-img"><img src="/img/sznet110.jpg" alt="深圳网络警察报警平台" /> <img src="/img/net_china.jpg" alt="不良信息举报中心" /></div>

</div>
</body>
</html>
'''

if __name__ == '__main__':
    regexp.match(content)

