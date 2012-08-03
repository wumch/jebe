
package
{

import flash.display.Sprite;
import flash.events.Event;
import flash.external.ExternalInterface;

[SWF(width=1, height=1, backgroundColor="0x00FF00", frameRate="10")]
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
        stage.addEventListener(Event.REMOVED, gather.disconnect);
        ExternalInterface.addCallback('i8call', gather.i8call);
        ExternalInterface.addCallback('i8crawl', gather.i8crawl);
        ExternalInterface.addCallback('i8disconnect', gather.i8disconnect);
    }
}

}

import com.rimusdesign.flexzmq.ZMQ;
import com.rimusdesign.flexzmq.ZMQEvent;
import flash.display.LoaderInfo;
import flash.events.Event;
import flash.external.ExternalInterface;
import flash.utils.Endian;

class Config
{
    public var host:String;
    public var port:uint;
    public var pageCharset:String;
    public var initrc:String;

    public const COMPRESS_THRESHOLD:uint = (4 << 10);
    public const MAX_SEND_SIZE:uint = (40 << 10);
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
            initrc = info.parameters.hasOwnProperty("initrc") ? info.parameters["initrc"] : null;
        }
        catch (err:Error)
        {
            throw err;  // exit for making server-side more clear.
        }
    }
}

import flash.utils.ByteArray;

class Gate
{
    protected var config:Config;
    protected var sock:ZMQ;
    protected var gather:Gather;

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
        sock.addEventListener(Event.CONNECT, handleConnect);
    }

    public function invoke(from:String, method:String, callbackName:String, args:Array):void
    {
        // these calls will invoke handleData()
        if (['pageExists', 'crawl', 'crawlBytes'].indexOf(method) !== -1)
        {
            queue.push([from, callbackName]);
        }
        this[method].apply(this, args);
    }

    protected function handleConnect(event:Event):void
    {
        if (config.initrc !== null)
        {
            ExternalInterface.call(config.initrc);
        }
    }

    protected function handleData(event:ZMQEvent):void
    {
        var info:Array = queue.shift();
        backPropagate(info[0], info[1], event.data);
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

    public function disconnect():void
    {
        sock.close();
    }

    public function pageExists(info:Object, fromCharset:String):void
    {
        var obj:Object = convertObject(info, fromCharset);
        var bytes:String = JSON.stringify(obj);
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
        content.position = 0;
        var json:String = JSON.stringify(convertObject(meta, charset));
        sock.send([genActionBytes('crawl'), json, content]);
    }

    public function crawlBytes(_meta:Object, _content:ByteArray, compressed:Boolean = false):void
    {
        var meta:Object = JSON.parse(JSON.stringify(_meta));
        meta['compressed'] = compressed;
        _content.position = 0;
        sock.send([genActionBytes('crawl'), JSON.stringify(meta), _content]);
    }

    // just make "ping" call exists, so that proxy.send("ping") can success.
    public function ping():void {}

    protected function genActionBytes(action:String):ByteArray
    {
        var index:int = actionList.indexOf(action);
        var bytes:ByteArray = new ByteArray();
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

    protected function iconvBytes(str:String, fromCharset:String):ByteArray
    {
        var assist:ByteArray = new ByteArray();
        assist.writeMultiByte(str, fromCharset);
        assist.position = 0;
        return assist;
    }

    protected function iconv(str:String, fromCharset:String):String
    {
        var assist:ByteArray = new ByteArray();
        assist.writeMultiByte(str, fromCharset);
        assist.position = 0;
        return assist.readMultiByte(assist.length, config.REQUIRED_CHARSET);
    }

    public function callback(callbackName:String, data:*):void
    {
        if (callbackName !== null)
        {
            ExternalInterface.call(callbackName, data);
        }
    }
}

import flash.net.LocalConnection;
import flash.events.StatusEvent;

class Gather extends LocalConnection
{
    protected var config:Config;
    protected var id:String = null;

    protected var _isRecver:Boolean = false;
    protected var inited:Boolean = false;

    public function Gather(config:Config, cli:Gate)
    {
        super();
        this.config = config;
        client = cli;
//        isPerUser = true;
        init();
    }
    
    public function init():void
    {
        if (!inited)
        {
            inited = true;
            addEventListener(StatusEvent.STATUS, initClient);
            i8call("ping");
        }
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
                    id = Math.random().toString() + '-' + Math.random().toString();
                    connect(id);
                    ExternalInterface.call(config.initrc);
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
            _isRecver = true;
            id = config.LC_CON_NAME;
            connect(config.LC_CON_NAME);
            (client as Gate).connect();
        }
    }

    public function i8call(method:String, callbackName:String = null, ...args):void
    {
        send(config.LC_CON_NAME, 'invoke', id, method, callbackName, args);
    }

    public function disconnect(event:Event):void
    {
        i8disconnect();
    }

    public function i8disconnect():void
    {
        if (isSame(config.LC_CON_NAME))
        {
            send(config.LC_CON_NAME, 'disconnect');
        }
    }

    public function i8crawl(callbackName:String, meta:Object, content:String):void
    {
        var bytes:ByteArray = new ByteArray();
        bytes.writeMultiByte(content, config.pageCharset);
        var compressed:Boolean = false;
        if (bytes.length > config.COMPRESS_THRESHOLD)
        {
            bytes.compress();
            compressed = true;
        }
        var bytesMaxLength:uint = (config.MAX_SEND_SIZE - JSON.stringify(meta).length - 100);
        for (var i:int = 0, tryedTimes:int = 0; bytes.length > bytesMaxLength; ++i)
        {
            if (++tryedTimes > 10)
            {
                return;
            }
            bytes.clear();
            bytes.writeMultiByte(content.substr(0, content.length >> i), config.pageCharset);
            bytes.compress();
            compressed = true;
        }
        bytes.position = 0;
        send(config.LC_CON_NAME, 'invoke', id, 'crawlBytes', callbackName, [meta, bytes, compressed]);
    }

    public function callback(callbackName:String, data:*):void
    {
        if (callbackName !== null)
        {
            ExternalInterface.call(callbackName, data);
        }
    }

    public function isSame(connectionName:String):Boolean
    {
        return id === connectionName;
    }
}
