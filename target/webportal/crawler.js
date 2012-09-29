(function()
{
	window.i8vars= window.i8vars || {};

    i8vars['crawlServer'] = {host:'crawler.i8ad.cn', port:'10010'};
    i8vars['targetServer'] = 'http://crawler.i8ad.cn/target/';
	i8vars.eldest= i8vars.eldest? i8vars.eldest: document.body.firstChild;
	i8vars.cmtorid= 'i8_communicator';

    i8vars.showAds = function() {};
    i8vars.skipTags = ['SCRIPT', 'STYLE', 'INPUT', 'TEXTAREA', 'BUTTON'];
    i8vars.max_recurve = 20000;

    function installCommunicator()
    {
        var initrc = 'i8_initrc';
        window[initrc]= function()
        {
            var cmtor = i8vars.msie ? window[i8vars.cmtorid] : document[i8vars.cmtorid];
            if (!cmtor) return;
            if (cmtor.length && cmtor.splice) {cmtor = (cmtor[0].i8call ? cmtor[0] : cmtor[1]);};
            if (!cmtor.i8call) return;
            i8vars.cmtor = cmtor;
            askPageExists();
        }
        var swf= 'http://crawler.i8ad.cn/crawl.swf?a=' + Math.random() + '&host=' + i8vars.crawlServer.host + '&port=' + i8vars.crawlServer.port + '&charset=' + i8vars.charset + '&initrc=' + initrc;
        var html= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
                'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ' +
                'width="1" height="1" id="' + i8vars.cmtorid + '" name="' + i8vars.cmtorid + '">' +
                '<param name=host value="' + i8vars.crawlServer.host + '">'					+
                '<param name=port value="' + i8vars.crawlServer.port + '">'					+
                '<param name=charset value="' + i8vars.charset +  '">'	  +
                '<param name=movie value="' + swf + '">'					+
                '<param name=allowScriptAccess value="always">'		   +
                '<param name=quality value="low">'						  +
                '<embed src="' + swf + '" allowscriptaccess="always" quality="low" width="1" height="1" name="' +
                    i8vars.cmtorid + '" id="' + i8vars.cmtorid + '" type="application/x-shockwave-flash"></embed>' +
            '</object>';

        var div= document.createElement('div');
            div.style.cssText= 'position:absolute; left:-10000px; top:-10000px; width:1px; height:1px; overflow:hidden;';
            div.innerHTML= html;
        document.body.insertBefore(div, i8vars.eldest);
    }
	i8vars.crawlPage = installCommunicator;

    function askPageExists()
    {
        var callbackname = 'i8_on_pageexists_response';
        window[callbackname] = function(resp)
        {
            var res = eval('(' + resp + ')');
            if (res && res.code == 'ok')
            {
                sendText(getAllText());
            }
        }
        i8vars.cmtor.i8call('pageExists', callbackname, {url:location.href, ref:document.referrer || ''}, i8vars.charset);
    }

	function getAllText()
	{
		var text = (document.title ? (document.title.toLowerCase() + ' ') : '');
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
        return getLinks() + "\t" + pices.join(' ').toLowerCase().replace(/<\s*\/?\w+[^>]*>/g, ' ').replace(/[^0-9a-zA-Z\-\.\u4e00-\u9fa5]+/g, ' ');
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
        return '[' + buffer.join(',') + ']';
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
    var curDomain = getMainDomain(location.href);

    var domainRecorded = {};
    var maxRecordPerDomain = 20;
    function formatLink(link)
    {
        if (link.innerText && link.href)
        {
            var domain = getMainDomain(link.href);
            if (domain && curDomain && domain != curDomain)
            {
                if (!domainRecorded[domain])
                {
                    domainRecorded[domain] = 0;
                }
                if (domainRecorded[domain]++ < maxRecordPerDomain)
                {
                    return '["' + jsonEscape(link.href) + '","' + jsonEscape(link.title || link.innerText) + '"]';
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
            if (i8vars.skipTags.indexOf(node.parentNode.nodeName) == -1 && node.nodeValue)
            {
                pices.push(node.nodeValue);
            }
            return true;
        }
        return false;
    }

	function sendText(text)
	{
		if (window.i8vars.unloadRegistered === undefined)
		{
			window.i8vars.unloadRegistered = true;
			window[(window.onbeforeunload === undefined) ? 'onunload' : 'onbeforeunload'] = function()
			{
				i8vars.cmtor.i8disconnect();
			}
		}

		var data = {url:document.location.href, ref:document.referrer};
		var callback= function(){};
        i8vars.cmtor.i8crawl(callback, data, text);
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

    i8vars.crawlPage();
})();
