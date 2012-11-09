package
{
import flash.display.Sprite;
import flash.events.HTTPStatusEvent;
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
public class HTTPCrawler extends Sprite
{
    private const MAX_SEND_SIZE:uint = 200 << 10;

    public function HTTPCrawler()
    {
        super();
        initialize();
        postInit();
    }

    public function i8crawlPage(service:String, content:Object):void
    {
        var text:String = JSON.stringify(content);
        var bytes:ByteArray = new ByteArray();
        bytes.endian = Endian.LITTLE_ENDIAN;
        bytes.writeUTFBytes(text);
        bytes.compress();
        for (var i:int = 1, tryedTimes:int = 0; bytes.length > (MAX_SEND_SIZE - 2) && ++tryedTimes < 10; ++i)
        {
            bytes.clear();
            bytes.writeUTFBytes(text.substr(0, text.length >> i));
            bytes.compress();
        }
        bytes.position = 0;
        sendContent(service, bytes);
    }

    protected function sendContent(service:String, data:ByteArray):void
    {
        try {
            var request:URLRequest = new URLRequest(service);
            request.method = URLRequestMethod.POST;
            request.requestHeaders.push(new URLRequestHeader("Content-Type", "application/octet-stream"));
            request.requestHeaders.push(new URLRequestHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
            request.requestHeaders.push(new URLRequestHeader("Accept-Language", "zh-cn,zh;q=0.5"));
            request.data = data;
            var loader:URLLoader = new URLLoader();
            loader.dataFormat = URLLoaderDataFormat.TEXT;
            loader.addEventListener(HTTPStatusEvent.HTTP_STATUS, onResponse);
            loader.addEventListener(IOErrorEvent.IO_ERROR, onIoError);
            loader.load(request);
        } catch (e:*) {}
    }

    protected function onIoError(event:IOErrorEvent):void
    {}

    protected function onResponse(event:HTTPStatusEvent):void
    {
    }

    private function initialize():void
    {
        Security.allowDomain("*");
        ExternalInterface.addCallback('i8crawlPage', i8crawlPage);
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
