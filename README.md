# Net
A C++ network library including Socket and HTTP.

## Build
To build the project, all you need to do is to open build/Net.sln with Visual Studio 2015 and build it.

## Tutorial
### TCP Socket Example
Let's start with a simple echo server. *(To make examples simple, we ignore all error checks.)*
```cpp
#include <iostream>

#include "net/socket/ServerSocket.h"

int main()
{
    net::ServerSocket server(8989);
    auto ss = server.Accept();

    char buffer[256] = { 0 };
    int n = ss->Receive(buffer, sizeof(buffer) - 1);
    std::cout << "Client Message: " << buffer << std::endl;
    
    ss->Send(buffer, n);

    return 0;
}
```
Well, we can create a ServerSocket with the default ctor, then call **_Bind_** and **_Listen_**.
```cpp
net::ServerSocket server
server.Bind(8989);
server.Listen();
```

Now let's make a simple client.
```cpp
#include <iostream>

#include "net/socket/StreamSocket.h"

int main()
{
    net::StreamSocket ss;
    ss.Connect(net::SocketAddress("127.0.0.1", 8989));

    std::string msg = "Hello World";
    ss.Send(msg.c_str(), msg.length());
 
    char buffer[256]{ 0 };
    ss.Receive(buffer, sizeof(buffer) - 1);

    std::cout << "Echo Message: " << buffer << std::endl;
    return 0;
}
```

### UDP Socket Example
Like TCP Example, firstly, let's build a UDP echo server.
```cpp
#include <iostream>

#include "net/socket/DatagramSocket.h"

int main()
{
    net::DatagramSocket server;
    server.Bind(net::SocketAddress("", 9191));

    net::SocketAddress sender;
    char buffer[256]{ 0 };
    int n = server.ReceiveFrom(buffer, sizeof(buffer) - 1, sender);
    std::cout << "Received Message: " << buffer << std::endl;

    server.SendTo(buffer, n, sender);

    return 0;
}
```
Here we can build two kinds of client.

One, we use Send and Receive.

*Codes are as follows:*
```cpp
#include <iostream>

#include "net/socket/DatagramSocket.h"

int main()
{
    net::DatagramSocket ds;
    ds.Connect(net::SocketAddress("127.0.0.1", 9191));

    std::string msg = "Hello World";
    ds.Send(msg.c_str(), msg.length());

    char buffer[256]{ 0 };
    ds.Receive(buffer, sizeof(buffer) - 1);
    std::cout << "Echo Message: " << buffer << std::endl;
    return 0;
}
```
The other, we use SendTo and ReceiveFrom.

*Here are the codes:*
```cpp
#include <iostream>

#include "net/socket/DatagramSocket.h"

int main()
{
    net::DatagramSocket ds;

    std::string msg = "Hello World";
    ds.SendTo(msg.c_str(), msg.length(), net::SocketAddress("127.0.0.1", 9191));

    char buffer[256]{ 0 };
    net::SocketAddress sa;
    ds.ReceiveFrom(buffer, sizeof(buffer) - 1, sa);
    std::cout << "Echo Message: " << buffer << std::endl;
    return 0;
}
```

### HTTP Client Example
To do a http request, all we need to do are as follows:
```cpp
#include <iostream>

#include "net/http/client.h"

int main()
{
    auto client = net::http::Client::Create();
    auto response = client->Get("http://www.cppreference.com/");
    if (nullptr == response)
        return 0;
    std::cout << response->GetBody() << std::endl;

    // We can make use of the existing client instance.
    response = client->Get("http://en.cppreference.com/w/cpp/language/ascii");
    if (nullptr == response)
        return 0;
    std::cout << response->GetBody() << std::endl;
    return 0;
}
``` 
The Client provides **_Get_**, **_Head_**, **_Post_**, **_PostForm_** and **_Do_** methods.
We can choose one of them to do what we want. 

Currently, we haven't supplied **~~HTTPS~~** service.

### HTTP Server Example
To build a http server, what we can do is like follows:
```cpp
#include <iostream>

#include "net/http/handler.h"
#include "net/http/server.h"

using namespace net::http;

class SimpleHttpHandler : public Handler
{
public:
    virtual void ServeHTTP(std::shared_ptr<Context> ctx) override
    {
        std::cout << "Received Url: " << ctx->GetRequest()->GetUrl().ToString() << std::endl;
        ctx->Write("Hello World");
    }
};

int main()
{
    auto server = Server::Create(8080);
    server->SetHandler(std::make_shared<SimpleHttpHandler>());
    server->ListenAndServe();
    return 0;
}
```

Currently, we haven't supplied **~~HTTPS~~** service.
