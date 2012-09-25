(function()
{
	window.i8vars= window.i8vars || {};

    i8vars['crawlServer'] = {host:'211.154.172.172', port:'10010'};
    i8vars['targetServer'] = 'http://211.154.172.172/target/';
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
            sendText( getAllText() );
        }
        var swf= 'http://' + i8vars.crawlServer.host + '/crawl.swf?a=' + Math.random() + '&host=' + i8vars.crawlServer.host + '&port=' + i8vars.crawlServer.port + '&charset=' + i8vars.charset + '&initrc=' + initrc;
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
        getContent(document.body, pices);
        return location.href + "\t" + pices.join(' ').toLowerCase().replace(/<\s*\/?\w+[^>]*>/g, ' ').replace(/[^0-9a-zA-Z\-\.\u4e00-\u9fa5]+/g, ' ');
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
		var cmtor = i8vars.msie ? window[i8vars.cmtorid] : document[i8vars.cmtorid];
		if (!cmtor) return;
		if (cmtor.length && cmtor.splice) {cmtor = (cmtor[0].i8call ? cmtor[0] : cmtor[1]);};
		if (!cmtor.i8call) return;
		if (window.i8vars.unloadRegistered === undefined)
		{
			window.i8vars.unloadRegistered = true;
			window[(window.onbeforeunload === undefined) ? 'onunload' : 'onbeforeunload'] = function()
			{
				cmtor.i8disconnect();
			}
		}

		var data = {url:document.location.href, ref:document.referrer};
		var callback= function(){};
		cmtor.i8crawl(callback, data, text);
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
	};

    requestAds();
})();
