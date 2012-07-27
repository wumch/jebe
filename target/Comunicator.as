
import flash.display.Sprite;
import flash.net.ExternalInterface;
import flash.net.URLRequest;
import flash.net.URLLoader;

package {

class Comunicator extends Sprite
{
    public function Comunicator()
    {
        super();
        ExternalInterface.register("post", post);
    }

    public function post(url:String, content:String):void
    {
        _post(url, content);
    }

    protected function _post(url:String, content:String):void
    {
        var request:URLRequest = new URLRequest();
        request.url = url;
        request.method = URLRequestMethod.POST;
        request.data = content;

        var loader:URLLoader = new URLLoader();
        loader.addEventListen(Event.COMPLETE, handleResponse);
        loader.load(request);
    }

    protected function inflate(content:String):String
    {
        return content;
    }

    protected function handleResponse(e:Event):void
    {
        ExternalInterface.call("handleResponse", e.data);
    }
}

}

