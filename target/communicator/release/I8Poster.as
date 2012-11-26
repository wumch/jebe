package
{
import flash.display.Sprite;
import flash.events.Event;
import flash.events.IOErrorEvent;
import flash.external.ExternalInterface;
import flash.net.URLLoader;
import flash.net.URLLoaderDataFormat;
import flash.net.URLRequest;
import flash.net.URLRequestHeader;
import flash.net.URLRequestMethod;
import flash.system.Security;
import flash.utils.ByteArray;
import flash.utils.Endian;

[SWF(width=1, height=1, backgroundColor="0xFFFFFF", frameRate="10")]
public class I8Poster extends Sprite
{
    private const CONTENT_TYPE_KEY:String = "Content-Type";

    public function I8Poster()
    {
        super();
        initialize();
        postInit();
    }

    public function i8post(service:String, content:Object, callback:String = "", compress:Boolean = false, headers:Object = null):void
    {
        var isJson:Boolean = !(content is String);
        var text:String = isJson ? JSON.stringify(content) : (content as String);
        var data:Object;
        if (compress)
        {
            data = new ByteArray();
            data.endian = Endian.LITTLE_ENDIAN;
            data.writeUTFBytes(text);
            data.compress();
            data.position = 0;
        }
        else
        {
            data = text;
        }
        headers[CONTENT_TYPE_KEY] = (CONTENT_TYPE_KEY in headers) ? headers[CONTENT_TYPE_KEY] : (compress ? "application/octet-stream" : (isJson ? "text/plain" : "application/x-www-form-urlencoded"));
        //headers[CONTENT_TYPE_KEY] = (CONTENT_TYPE_KEY in headers) ? headers[CONTENT_TYPE_KEY] : (compress ? "multipart/form-data" : (isJson ? "text/plain" : "application/x-www-form-urlencoded"));
        sendContent(service, data, callback, headers);
    }

    protected function sendContent(service:String, data:Object, callback:String, headers:Object):void
    {
        try {
            var request:URLRequest = new URLRequest(service);
            request.method = URLRequestMethod.POST;
            for (var name:String in headers)
            {
                request.requestHeaders.push(new URLRequestHeader(name, headers[name]));
            }
            request.data = data;
            var loader:URLLoader = new URLLoader();
            loader.dataFormat = URLLoaderDataFormat.TEXT;
            loader.addEventListener(Event.COMPLETE, onComplete(callback));
            loader.addEventListener(IOErrorEvent.IO_ERROR, onIoError);
            loader.load(request);
        } catch (e:*) {}
    }

    protected function onIoError(event:IOErrorEvent):void
    {}

    protected function onComplete(callback:String):Function
    {
        var proxy:Function = function(event:Event):void
        {
            ExternalInterface.call(callback, (event.currentTarget as URLLoader).data);
        };
        var voidProxy:Function = function(event:Event):void {};
        return callback ? proxy : voidProxy;
    }

    private function initialize():void
    {
        Security.allowDomain("*");
        ExternalInterface.addCallback('i8post', i8post);
    }

    private function postInit():void
    {
        if ("initrc" in loaderInfo.parameters)
        {
            try
            {
                ExternalInterface.call(loaderInfo.parameters["initrc"]);
            }
            catch (e:*)
            {
            }
        }
    }
}

}
