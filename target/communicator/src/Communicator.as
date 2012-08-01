
package
{

import flash.display.Sprite;
import flash.external.ExternalInterface;
import flash.text.TextField;
import flash.text.TextFormat;

[SWF(width=1000, height= 600, backgroundColor="0x00FF00", frameRate="20")]
public class Communicator extends Sprite
{
    protected var config:Config;
    protected var gather:Gather;
    public var gate:Gate;

    public function Communicator()
    {
        super();
        initialize();
    }

    public function initialize():void
    {
        config = new Config(loaderInfo);
        gate = new Gate(config);
        gather = new Gather(config, gate);
        gate.setGather(gather);
        ExternalInterface.addCallback('call', gather.call);

        // test only
        var text:TextField = new TextField();
        text.borderColor = 0xFF0000;
        text.backgroundColor = 0x00FF00;
        text.textColor = 0xFF0000;
        text.border = true;
        text.setTextFormat(new TextFormat(null, 30, 0xFF0000));
        text.x = 50;
        text.y = 20;
        text.text = config.host + "\n" + config.policy + "\n";
        text.appendText(ExternalInterface.available ? "avail" : "invail");
        addChild(text);
    }
}

}

import com.rimusdesign.flexzmq.ZMQ;
import com.rimusdesign.flexzmq.ZMQEvent;
import flash.display.LoaderInfo;
import flash.external.ExternalInterface;
import flash.utils.Endian;

class Config
{
    public var host:String;
    public var port:uint;
    public var policy:String;
    public var pageCharset:String;

    public const COMPRESS_THRESHOLD:uint = (4 << 10);
    public const LC_CON_NAME:String = "gyads";
    public const REQUIRED_CHARSET:String = "utf-8";

    public function Config(info:LoaderInfo)
    {
        initialize(info);
    }

    protected function initialize(info:LoaderInfo):void
    {
        try
        {
            host = info.parameters["host"];
            port = parseInt(info.parameters["port"]);
            pageCharset = info.parameters["charset"].toLowerCase();
        }
        catch (err:Error)
        {
            throw err;
        }
    }
}

import flash.utils.ByteArray;

class Gate
{
    protected var config:Config;
    protected var sock:ZMQ;
    protected var gather:Gather;

    protected static const ERR_OK:String = 'y';
    protected static const ERR_ERR:String = 'n';

    protected var queue:Array;

    protected var actionList:Array = [
        '',     // hold the index of `0` by a invalid.
        // Ask server wheather page exists or not, and tell server where it's from.
        // Should carry enough data for targeting ad.
        'pageExists',
        // Send page content to crawler.
        'crawl',
        // receive responsed ads.
        'showAds',
    ];

    public function Gate(config:Config)
    {
        this.config = config;
        queue = new Array();    // single zmq client-server pair can guarantee FIFO.
        sock = new ZMQ(ZMQ.REQ);
        prepareReceiver();
    }

    // TODO: design smells.
    public function setGather(gather:Gather):void
    {
        this.gather = gather;
    }

    protected function prepareReceiver():void
    {
        sock.addEventListener(ZMQEvent.MESSAGE_RECEIVED, handleData);
    }

    public function invoke(from:String, method:String, callbackName:String, args:Array):void
    {
        alert("nmethod: " + method + "\nfrom: " + from);
        queue.push([from, callbackName]);
        this[method].apply(this, args);
    }

    protected function handleData(event:ZMQEvent):void
    {
        var info:Array = queue.shift();
        backPropagate(info[0], info[1], event.data);
        alert("alert in master page: " + event.data);
    }

    protected function backPropagate(to:String, callbackName:String, data:*):void
    {
        if (to !== null && callbackName !== null)
        {
            if (gather.isSame(to))
            {
                callback(callbackName, data);
            }
            else
            {
                gather.send(to, 'callback', callbackName, data);
            }
        }
    }

    // connect to server.
    public function connect():void
    {
        sock.connect(config.host, config.port);
    }

    public function pageExists(info:Object, fromCharset:String):void
    {
        var obj:Object = convertObject(info, fromCharset);
        alert("obj.url: " + obj.url);
        var bytes:String = JSON.stringify(obj);
        alert("json: " + bytes);
        sock.send([genActionBytes('pageExists'), bytes]);
    }

    // request an action with extra data.
    public function crawl(_meta:Object, _content:String, charset:String = config.REQUIRED_CHARSET):void
    {
        var meta:Object = JSON.parse(JSON.stringify(_meta));    // clone() not supported since _meta is an external object.
        meta['compressed'] = false;
        var content:ByteArray = iconvBytes(_content, charset);
        if (content.length >= config.COMPRESS_THRESHOLD)
        {
            content.compress();
            meta['compressed'] = true;
        }
        var json:String = JSON.stringify(convertObject(meta, charset));
        sock.send([genActionBytes('crawl'), json, content]);
    }

    // just make "ping" call exists, so that proxy.send("ping") can success.
    public function ping():void {}

    protected function genActionBytes(action:String):ByteArray
    {
        var index:int = actionList.indexOf(action);
        var bytes:ByteArray = new ByteArray();
        bytes.endian = Endian.LITTLE_ENDIAN;
        bytes.writeByte(index);
        bytes.position = 0;
        return bytes;
    }

    protected function convertObject(obj:Object, fromCharset:String):Object
    {
        var res:Object = new Object();
        for (var pname:String in obj)
        {
            if (obj[pname] is String)
            {
                res[pname] = iconv(obj[pname], fromCharset);
            }
            else if (obj[pname] is Number || obj[pname] is Boolean)
            {
                res[pname] = obj[pname];
            }
            else if (obj is Object)
            {
                res[pname] = convertObject(obj[pname], fromCharset);
            }
        }
        return res;
    }

    // convert and compress.
    protected function convert(data:String, fromCharset:String):ByteArray
    {
        var bytes:ByteArray = new ByteArray();
        bytes.endian = Endian.LITTLE_ENDIAN;
        var compress:Boolean = (data.length > config.COMPRESS_THRESHOLD);
        bytes.writeByte((compress ? ERR_OK : ERR_ERR).charCodeAt(0));
        if (compress)
        {
            var assist:ByteArray = new ByteArray();
            assist.endian = Endian.LITTLE_ENDIAN;
            assist.writeMultiByte(data, fromCharset);
            assist.compress();
            assist.position = 0;
            bytes.writeBytes(assist);
        }
        else
        {
            bytes.writeMultiByte(data, fromCharset);
        }
        alert("bytes.length: " + (bytes.length));
        bytes.position = 0;
        alert("str.length:" + (bytes.readMultiByte(bytes.length, config.REQUIRED_CHARSET)));
        bytes.position = 0;
        return bytes;//.readMultiByte(bytes.length, config.REQUIRED_CHARSET);
    }

    protected function iconvBytes(str:String, fromCharset:String):ByteArray
    {
        var assist:ByteArray = new ByteArray();
        assist.endian = Endian.LITTLE_ENDIAN;
        assist.writeMultiByte(str, fromCharset);
        assist.position = 0;
        return assist;
    }

    protected function iconv(str:String, fromCharset:String):String
    {
        var assist:ByteArray = new ByteArray();
        assist.endian = Endian.LITTLE_ENDIAN;
        assist.writeMultiByte(str, fromCharset);
        assist.position = 0;
        return assist.readMultiByte(assist.length, config.REQUIRED_CHARSET);
    }

    public function callback(callbackName:String, data:*):void
    {
        ExternalInterface.call(callbackName, data);
    }
}

import flash.net.LocalConnection;
import flash.events.StatusEvent;

class Gather extends LocalConnection
{
    protected var busName:String;
    protected var id:String = null;

    protected var _isRecver:Boolean = false;
    protected var inited:Boolean = false;

    public function Gather(config:Config, cli:Gate)
    {
        super();
        busName = config.LC_CON_NAME;
        client = cli;
        isPerUser = true;
    }
    
    public function init():void
    {
        addEventListener(StatusEvent.STATUS, initClient);
        call("ping");
    }

    protected function initClient(event:StatusEvent):void
    {
        switch (event.level)
        {
            case "error":
                makeRecver();
                break;
            case "status":
                if (id === null)
                {
                    id = ExternalInterface.call("getPageId") as String;
                    connect(id);
                    alert("current gather id: [" + id + "]");
                }
                break;
            default:
                break;
        }
    }

    protected function makeRecver():void
    {
        if (!_isRecver)
        {
            if (id !== null)
            {
                close();
            }
            alert('makeRecver');
            _isRecver = true;
            id = busName;
            connect(busName);
            (client as Gate).connect();
        }
    }

    public function call(method:String, callbackName:String = null, ...args):void
    {
        if (!inited)
        {
            inited = true;
            init();
        }
        send(busName, 'invoke', id, method, callbackName, args);
    }

    public function callback(callbackName:String, data:*):void
    {
        ExternalInterface.call(callbackName, data);
    }

    public function isSame(connectionName:String):Boolean
    {
        return id === connectionName;
    }
}

function alert(...args):void
{
    if (args.length > 1)
    {
        ExternalInterface.call('alert', args.join(",\n"));
    }
    else
    {
        ExternalInterface.call('alert', args[0]);
    }
}
