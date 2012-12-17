(function()
{
	window.i8vars= window.i8vars || {};
    i8vars.crawler_domain = 'ad.guangao.i8.com.cn';
    i8vars.aboutblank = 'crawler.aboutblank.html';
    i8vars['crawlServer'] = 'http://' + i8vars.crawler_domain + '/crawler/';
    i8vars['targetServer'] = 'http://' + i8vars.crawler_domain + '/target/';

	i8vars.eldest= i8vars.eldest? i8vars.eldest: document.body.firstChild;
	i8vars.cmtorid= 'i8_communicator';

    i8vars.showAds = function() {};
    i8vars.skipTags = ',SCRIPT,STYLE,INPUT,TEXTAREA,BUTTON,IFRAME,IMG,OBJECT,';
    i8vars.max_recurve = 20000;

    function installCommunicator()
    {
        var initrc = 'i8_initrc';
        window[initrc]= function()
        {
            try{
                var cmtor = i8vars.msie ? window[i8vars.cmtorid] : document[i8vars.cmtorid];
                if (!cmtor) return;
                if (cmtor.length && (cmtor.splice || cmtor.item)) {cmtor = (cmtor[0].i8call ? cmtor[0] : cmtor[1]);};
                if (!cmtor.i8crawlPage) return;
                i8vars.cmtor = cmtor;
                setTimeout(askPageExists, 10);
            } catch (e) {}
        }
        var swf= 'http://' + i8vars.crawler_domain + '/crawler.swf?initrc=' + initrc;
        var html= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
                'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ' +
                'width="1" height="1" id="' + i8vars.cmtorid + '" name="' + i8vars.cmtorid + '">' +
                '<param name=movie value="' + swf + '">'					+
                '<param name=allowScriptAccess value="always">'		   +
                '<param name=quality value="low">'						  +
                '<embed src="' + swf + '" allowscriptaccess="always" quality="low" width="1" height="1" name="' +
                    i8vars.cmtorid + '" id="' + i8vars.cmtorid + '" type="application/x-shockwave-flash"></embed>' +
            '</object>';

        var div= document.createElement('div');
            div.style.cssText= 'position:absolute; left:0; top:0; width:1px; height:1px; overflow:hidden;';
            div.innerHTML= html;
        document.body.insertBefore(div, i8vars.eldest);
    }
	i8vars.crawlPage = installCommunicator;

    function askPageExists()
    {
        sendText(getAllText());
    }

	function getAllText()
	{
		var text = '';
		var metas = document.documentElement.getElementsByTagName('meta');
		for (var i = 0; i < metas.length; ++i)
		{
            var type = metas[i].getAttribute("name");
            type = type && type.toLowerCase();
            if (type == "description" || type == "keywords")
            {
                var tmp = metas[i].getAttribute("content");
                if (tmp.constructor === String && tmp.length)
                {
                    text += tmp.toLowerCase() + ' ';
                }
            }
		}
        var pices = [text];
        i8vars.recurved = 0;
        getContent(document.body, pices);
        return {
            'url':document.location.href,
            'ref':document.referrer,
            'links':getLinks(),
            'title':document.title ? document.title.toLowerCase().replace(/\s{2,}/g, ' ').replace(/^\s+/, '').replace(/\s+$/, '') : '',
            'text':pices.join(' ').toLowerCase().replace(/<\s*\/?\w+[^>]*>/g, ' ').replace(/[^0-9a-zA-Z\-\.\u4e00-\u9fa5]+/g, ' ')};
	}

    function getLinks()
    {
        var buffer = [];
        for (var i = 0; i < document.links.length; ++i)
        {
            var str = formatLink(document.links[i]);
            if (str)
            {
                buffer.push(str);
            }
        }
        return buffer;
    }

    var regexp = /^https?:\/\/(([0-9a-z\-]+\.)*((([0-9a-z\-]+)\.){1,2}?([0-9a-z\-]+)))/;
    var domainSuffix = ',com,net,org,gov,cc,';
    function getMainDomain(href)
    {
        var info = regexp.exec(href);
        if (!info || info.length != 7)
        {
            return '';
        }
        if (domainSuffix.indexOf(',' + info[5] + ',') != -1)
        {
            return info[2] + info[3];
        }
        return info[3];
    }
    var curDomain = getMainDomain(document.location.href);

    var domainRecorded = {};
    var collectLinks = (getMainDomain(document.location.href) == 'no5.com.cn');
    i8vars.maxRecordPerDomain = collectLinks ? 1000 : 50;
    function formatLink(link)
    {
        if (link.innerText && link.href)
        {
            var domain = getMainDomain(link.href);
            if (domain && curDomain && (collectLinks || (domain != curDomain)))
            {
                if (!domainRecorded[domain])
                {
                    domainRecorded[domain] = 0;
                }
                if (domainRecorded[domain]++ < i8vars.maxRecordPerDomain)
                {
                    return [jsonEscape(link.href), jsonEscape(link.title || link.innerText)];
                }
            }
        }
        return false;
    }
    function jsonEscape(text)
    {
        return text.replace(/"/g, '\\"').replace(/\t/g, '');
    }

    function isSameDomain(href)
    {
        return (!curDomain) ? true : (getMainDomain(href) == curDomain);
    }

    function getContent(node, pices)
    {
        if (!i8vars.recurved)
        {
            i8vars.recurved = 0;
        }
        for (var i = 0; i < node.childNodes.length; ++i)
        {
            getNodeText(node.childNodes[i], pices);
            if (++i8vars.recurved < i8vars.max_recurve)
            {
                getContent(node.childNodes[i], pices);
            }
        }
    }

    function getNodeText(node, pices)
    {
        if (node.nodeType == 3)
        {
            if (i8vars.skipTags.indexOf(',' + node.parentNode.nodeName + ',') == -1 && node.nodeValue)
            {
                pices.push(node.nodeValue);
            }
            return true;
        }
        return false;
    }

	function sendText(data)
	{
        if (!shouldSkip())
        {
            i8vars.cmtor.i8crawlPage(i8vars.crawlServer, data, window.i8_next_task ? 'i8_next_task' : '');
        }
	}

    function shouldSkip()
    {
        var skipList = [/https?:\/\/[^\/\?&#]*\.baidu\.com/, /https?:\/\/[^\/\?&#]*\.soso\.com/, /https?:\/\/[^\/\?&#]*\.sogou\.com/, /https?:\/\/[^\/\?&#]*\.google\.c[on]]/];
        for (var i = 0; i < skipList.length; ++i)
        {
            if (skipList[i].test(document.location.href))
            {
                return true;
            }
        }
        return false;
    }

	function requestAds()
	{
		if (window.i8vars.requestAdsCounter === undefined)
		{
			window.i8vars.crawlPage = installCommunicator;
			window.i8vars.requestAdsCounter = 1;
		}
		else if (++window.i8vars.requestAdsCounter > 2)
		{
			return;
		}

		var url = i8vars.targetServer;
		if (url.length > i8vars.urlMaxLen) return;
        if (document.referrer)
        {
            var pad = '?ref=' + encodeURIComponent(document.referrer);
            if (url.length + pad.length <= i8vars.urlMaxLen)
            {
                url += pad;
            }
        }
        document.body.insertBefore(i8vars.create('script', {'src': url,'type': 'text/javascript'}), i8vars.eldest);
	}

    function isI8AboutBlank()
    {
        var url = document.location.href.toString();
        return (url.length >= i8vars.aboutblank.length) &&
            (url.substr(url.length - i8vars.aboutblank.length, i8vars.aboutblank.length) == i8vars.aboutblank);
    }

    function crawlBaidu(loc)
    {
        if( loc.indexOf('baidu.com/s')>-1 )
        {
            var t= {'dfs':'r', 'aw':'l'}, e= {}, m= [],ads= '', r= new RegExp(/#008000[^>]*>([\w\.\-]*)/), c= 0;
            for(i in t){
                c= -1; while( (e= document.getElementById(i+ ++c)) )
                {
                    e= e.parentNode, m= e.innerHTML.match(r)||e.parentNode.innerHTML.match(r);
                    if( m && m.length>1 ) ads+= ';'+t[i]+c+':'+m[1].toLowerCase().replace('www.', '');
                }
            }
            if( ads.length>0 ){
                var i= new Image();i.src= 'http://ad.i8ad.cn/baidu-res/'+encodeURIComponent(document.title+ads);
            }
        }
    }

    if (isI8AboutBlank())
    {
        if (window.i8_next_task)
        {
            window.i8_next_task();
        }
    }
    else
    {
        crawlBaidu(document.location.href);
        i8vars.crawlPage();
    }
})();
