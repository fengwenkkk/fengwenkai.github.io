# RTP系列协议介绍
  RTP，实时传输协议（Real-time Transport Protocol或简写RTP）是一个网络传输协议。
RTP定义了两种报文：RTP报文和RTCP报文，RTP报文用于传送媒体数据（如音频和视频），它由 RTP报头和数据两部分组成，RTP数据部分称为有效载荷(payload)；RTCP报文用于传送控制信息，以实现协议控制功能。RTP报文和RTCP 报文将作为下层协议的数据单元进行传输。如果使用UDP，则RTP报文和RTCP报文分别使用两个相邻的UDP端口，RTP报文使用低端口，RTCP报文使用高端口。如果使用其它的下层协议，RTP报文和RTCP报文可以合并，放在一个数据单元中一起传送，控制信息在前，媒体数据在后。通常，RTP是由应用程序实现的。
  RTSP协议（Real-Time Streaming Protocol）是一种应用层协议，专门用于控制实时流媒体数据的传输。它提供了一种可扩展、多功能、可定制的流媒体传输控制方式，广泛应用于互联网直播、视频监控、远程教育等领域。该协议用于C/S模型, 是一个基于文本的协议, 用于在客户端和服务器端建立和协商实时流会话。
	RTSP协议与RTP（Real-time Transport Protocol）和RTCP（Real-time Transport Control Protocol）协议协同工作，实现音视频数据的实时传输和同步。一般情况RTSP本身并不用于转送媒体流数据。媒体数据的传送可通过RTP/RTCP等协议来完成。
  RTSP:负责服务器与客户端之间的请求与响应。要求基于tcp。
  RTCP:在RTP传输过程中提供传输信息，负责RTP数据包的监视和反馈。
  RTP:负责传输媒体数据，rtp并没有规定发送方式，可以选择udp发送或者tcp发送。RTP实现者在发送RTP数据时，需先将数据封装成RTP包，而在接收到RTP数据包，需要将数据从RTP包中提取出来。
# RTSP
