#ifndef __LEO_NET_ADDRESS_H__
#define __LEO_NET_ADDRESS_H__

#include <string>
#include <sstream>

#ifdef WIN32
# include <winsock.h>
#else
# include <netinet/in.h>
#endif

#include "leo_utilities.h"

class LEO_EXPORT NetAddress
{
public:
  NetAddress(std::string ip, unsigned short port)
  {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  }

  NetAddress(const struct sockaddr_in& addr)
    : addr_(addr)
  {

  } 

  /*NetAddress(NetAddress&&) = delete;
  NetAddress(const NetAddress&) = delete;*/

public:
  unsigned short toPort() const
  {
    return ntohs(addr_.sin_port);
  }

  std::string toIp() const
  {
    return std::string(inet_ntoa(addr_.sin_addr));
  }

  std::string toIpPort() const
  {
    std::stringstream ss;
    ss << toIp() << ":" << toPort();

    return ss.str();
  }
  
private:
  struct sockaddr_in addr_;
};

#endif /* __LEO_NET_ADDRESS_H__ */