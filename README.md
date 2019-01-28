与 [Riroaki](https://github.com/Riroaki) 协作完成。

在终端中输入：clang++ -std=c++11 -o server -l pthread main.cpp即可编译。

实验要求

- 服务程序能够正确解析 HTTP 协议，并传回所需的网页文件和图片文件 

- 使用标准的浏览器，如 IE、Chrome 或者 Safari，输入服务程序的 URL后，能够正常显示服务器上的网页文件和图片

- 服务端程序界面不做要求，使用命令行或最简单的窗体即可 

- 功能要求如下: 

  1.服务程序运行后监听在 80 端口或者指定端口
  2.接受浏览器的 TCP 连接(支持多个浏览器同时连接) 3.读取浏览器发送的数据，解析 HTTP 请求头部，找到感兴趣的部分 4.根据 HTTP 头部请求的文件路径，打开并读取服务器磁盘上的文件，以 HTTP 响应格式传回浏览器。要求按照文本、图片文件传送不同的，Content-Type，以便让浏览器能够正常显示。 5.分别使用单个纯文本、只包含文字的 HTML 文件、包含文字和图片 

  的 HTML 文件进行测试，浏览器均能正常显示。 

- 本实验可以在前一个 Socket 编程实验的基础上继续，也可以使用第三方 

  封装好的 TCP 类进行网络数据的收发 

- 本实验要求不使用任何封装 HTTP 接口的类库或组件，也不使用任何服 

  务端脚本程序如 JSP、ASPX、PHP 等 

- 本实验可单独完成或组成两人小组。若组成小组，则一人负责编写服务 

  器 GET 方法的响应，另一人负责编写 POST 方法的响应和服务器主线程。 