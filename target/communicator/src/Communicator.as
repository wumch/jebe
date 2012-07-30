package
{

import flash.display.Sprite;
import flash.external.ExternalInterface;
import flash.text.TextField;
import flash.text.TextFormat;
import com.rimusdesign.flexzmq.ZMQ;

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

import com.rimusdesign.flexzmq.SocketOption;
import com.rimusdesign.flexzmq.ZMQ;
import com.rimusdesign.flexzmq.ZMQEvent;

import flash.display.LoaderInfo;
import flash.external.ExternalInterface;

class Config
{
    public var host:String;
    public var port:uint;
    public var policy:String;
    public var pageCharset:String;

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
            policy = info.parameters["policy"];
        }
        catch (err:Error)
        {
//            throw err;
        }
    }
}

import flash.net.Socket;
import flash.events.Event;
import flash.utils.ByteArray;
import flash.utils.Endian;

class Gate
{
    protected var config:Config;
    protected var sock:ZMQ;

    public function Gate(config:Config)
    {
        this.config = config;
        sock = new ZMQ(ZMQ.REQ);
    }

    // connect to server.
    public function connect():void
    {
        sock.connect(config.host, config.port);
    }

    public function pageExists(url:String):void
    {
        alert(url);
    }

    // send data to server.
    public function request(data:String, charset:String):void
    {
        sock.send([data]);
    }

    // just make "ping" call exists, so that proxy.send("ping") can success.
    public function ping():void
    {
    }

    protected function iconv(data:String, fromCharset:String):ByteArray
    {
        var res:ByteArray = new ByteArray();
        res.endian = Endian.LITTLE_ENDIAN;
        var _data:String = data;
        if (fromCharset !== config.REQUIRED_CHARSET)
        {
            var bytes:ByteArray = new ByteArray();
            res.endian = Endian.LITTLE_ENDIAN;
            bytes.writeMultiByte(data, fromCharset);
            _data = bytes.readMultiByte(bytes.length, config.REQUIRED_CHARSET);
        }
        res.writeMultiByte(_data, config.REQUIRED_CHARSET);
        return res;
    }
}

import flash.net.LocalConnection;
import flash.events.StatusEvent;

//
class Gather extends LocalConnection
{
    protected var conName:String;

    protected var _isRecver:Boolean = false;

    public function Gather(config:Config, cli:Gate)
    {
        super();
        conName = config.LC_CON_NAME;
        client = cli;
        init();
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
            default:
                _isRecver = false;
                break;
        }
    }

    protected function makeRecver():void
    {
        alert('makeRecver');
        _isRecver = true;
        connect(conName);
        (client as Gate).connect();
    }

    public function call(method:String, ...args):void
    {
        args.unshift(method);
        args.unshift(conName);
        alert(args);
        send.apply(this, args);
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