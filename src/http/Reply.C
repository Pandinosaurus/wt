/*
 * Copyright (C) 2008 Emweb bv, Herent, Belgium.
 *
 * All rights reserved.
 */
//
// reply.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2006 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "Configuration.h"
#include "Connection.h"
#include "Reply.h"
#include "Request.h"
#include "Server.h"

#include "Wt/WLogger.h"
#include "web/DateUtils.h"

#include <cassert>
#include <cctype>
#include <chrono>
#include <string>

#ifdef WT_BUILDING
#  define LOG_ACCESS_S(s,m) (s)->log("access") << WT_LOGGER << ": " << m
#  define LOG_ACCESS(m) do { \
    if ( WT_LOGGING("access", WT_LOGGER)) \
      WT_LOG("access") << WT_LOGGER << ": " << m; \
    }  while(0)
#endif

namespace {
bool ichar_equals(char a, char b)
{
  return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
}
}

namespace Wt {
  LOGGER("wthttp");
}

namespace http {
namespace server {

template <typename T>
inline asio::const_buffer asio_cstring_buf(T str);

template <std::size_t N>
inline asio::const_buffer asio_cstring_buf(const char (&s) [N])
{
  return asio::const_buffer(s, N-1);
}

namespace status_strings {

template<class S>
void toText(S& stream, Reply::status_type status)
{
  switch (status)
  {
  case Reply::switching_protocols:
    stream << "101 Switching Protocol\r\n";
    break;
  case Reply::ok:
    stream << "200 OK\r\n";
    break;
  case Reply::created:
    stream << "201 Created\r\n";
    break;
  case Reply::accepted:
    stream << "202 Accepted\r\n";
    break;
  case Reply::no_content:
    stream << "204 No Content\r\n";
    break;
  case Reply::partial_content:
    stream << "206 Partial Content\r\n";
    break;
  case Reply::multiple_choices:
    stream << "300 Multiple Choices\r\n";
    break;
  case Reply::moved_permanently:
    stream << "301 Moved Permanently\r\n";
    break;
  case Reply::found:
    stream << "302 Found\r\n";
    break;
  case Reply::see_other:
    stream << "303 See Other\r\n";
    break;
  case Reply::not_modified:
    stream << "304 Not Modified\r\n";
    break;
  case Reply::moved_temporarily:
    stream << "307 Moved Temporarily\r\n";
    break;
  case Reply::bad_request:
    stream << "400 Bad Request\r\n";
    break;
  case Reply::unauthorized:
    stream << "401 Unauthorized\r\n";
    break;
  case Reply::forbidden:
    stream << "403 Forbidden\r\n";
    break;
  case Reply::not_found:
    stream << "404 Not Found\r\n";
    break;
  case Reply::request_entity_too_large:
    stream << "413 Request Entity too Large\r\n";
    break;
  case Reply::requested_range_not_satisfiable:
    stream << "416 Requested Range Not Satisfiable\r\n";
    break;
  case Reply::not_implemented:
    stream << "501 Not Implemented\r\n";
    break;
  case Reply::bad_gateway:
    stream << "502 Bad Gateway\r\n";
    break;
  case Reply::service_unavailable:
    stream << "503 Service Unavailable\r\n";
    break;
  case Reply::version_not_supported:
    stream << "505 HTTP Version Not Supported\r\n";
    break;
  case Reply::no_status:
  case Reply::internal_server_error:
    stream << "500 Internal Server Error\r\n";
    break;
  default:
    stream << (int) status << " Unknown\r\n";
  }
}

} // namespace status_strings

Reply::Reply(Request& request,
        const Configuration& config,
        const Wt::Configuration* wtConfig)
  : request_(request),
    configuration_(config),
    wtConfig_(wtConfig),
    status_(no_status),
    transmitting_(false),
    closeConnection_(false),
    chunkedEncoding_(false),
    gzipEncoding_(false),
    contentSent_(0),
    contentOriginalSize_(0)
#ifdef WTHTTP_WITH_ZLIB
    , gzipBusy_(false)
#endif // WTHTTP_WITH_ZLIB
{
  addDefaultHeaders();
}

Reply::~Reply()
{
  LOG_DEBUG("~Reply");
#ifdef WTHTTP_WITH_ZLIB
  if (gzipBusy_)
    deflateEnd(&gzipStrm_);
#endif // WTHTTP_WITH_ZLIB
}

void Reply::writeDone(WT_MAYBE_UNUSED bool success)
{ }

void Reply::reset(WT_MAYBE_UNUSED const Wt::EntryPoint* ep)
{
#ifdef WTHTTP_WITH_ZLIB
  if (gzipBusy_) {
    deflateEnd(&gzipStrm_);
    gzipBusy_ = false;
  }
#endif // WTHTTP_WITH_ZLIB

  headers_.clear();
  addDefaultHeaders();
  status_ = no_status;
  transmitting_ = false;
  closeConnection_ = false;
  chunkedEncoding_ = false;
  gzipEncoding_ = false;
  contentSent_ = 0;
  contentOriginalSize_ = 0;

  relay_.reset();
}

void Reply::setStatus(status_type status)
{
  status_ = status;
}

bool Reply::consumeWebSocketMessage(WT_MAYBE_UNUSED ws_opcode opcode, WT_MAYBE_UNUSED const char* begin,
                                    WT_MAYBE_UNUSED const char* end, WT_MAYBE_UNUSED Request::State state)
{
  LOG_ERROR("Reply::consumeWebSocketMessage() is pure virtual");
  assert(false);
  return false;
}

std::string Reply::location()
{
  return std::string();
}

void Reply::addHeader(const std::string name, const std::string value)
{
  headers_.push_back(std::make_pair(name, value));
}

void Reply::insertHeader(const std::string name, const std::string value)
{
  auto foundIterator = std::find_if(headers_.begin(), headers_.end(), [name](const std::pair<std::string, std::string>& item) {
    return std::equal(item.first.begin(), item.first.end(), name.begin(), name.end(), ichar_equals);
  });

  if (foundIterator == headers_.end()) {
    if (!value.empty()) {
      headers_.push_back(std::make_pair(name, value));
    }
  } else {
    if (value.empty()) {
      headers_.erase(foundIterator);
    } else {
      foundIterator->second = value;
    }
  }
}

namespace {

inline char hexLookup(int n) {
  return "0123456789abcdef"[(n & 0xF)];
}

/*
 * Encode an integer as a hex std::string
 *
 * NOTE: Only works for *positive* integers
 */
inline std::string hexEncode(int n) {
  assert(n >= 0);
  if (n == 0)
    return "0";
  char buffer[sizeof(int) * 2];
  int i = sizeof(int) * 2;
  while (n != 0) {
    --i;
    buffer[i] = hexLookup(n);
    n >>= 4;
  }
  return std::string(buffer + i, sizeof(int) * 2 - i);
}

}

bool Reply::nextWrappedContentBuffers(std::vector<asio::const_buffer>& result) {
  std::vector<asio::const_buffer> contentBuffers;
  int originalSize;
  int encodedSize;

  bool lastData = encodeNextContentBuffer(contentBuffers, originalSize,
                                          encodedSize);

  contentSent_ += encodedSize;
  contentOriginalSize_ += originalSize;

  if (chunkedEncoding_) {
    if (encodedSize || lastData) {
      buf_ << hexEncode(encodedSize);
      buf_ << "\r\n";

      buf_.asioBuffers(result);

      if (encodedSize) {
        result.insert(result.end(),
            contentBuffers.begin(), contentBuffers.end());
        postBuf_ << "\r\n";

        if (lastData) {
          postBuf_ << "0\r\n\r\n";
        }
      } else {
        postBuf_ << "\r\n";
      }

      postBuf_.asioBuffers(result);
    } else
      buf_.asioBuffers(result);

    return lastData;
  } else {
    buf_.asioBuffers(result);
    result.insert(result.end(), contentBuffers.begin(), contentBuffers.end());
    return lastData;
  }
}

bool Reply::nextBuffers(std::vector<asio::const_buffer>& result)
{
  bufs_.clear();
  buf_.clear();
  postBuf_.clear();

  if (relay_.get())
    return relay_->nextBuffers(result);
  else {
    if (!transmitting_) {
      transmitting_ = true;
      bool http10 = (request_.http_version_major == 1)
        && (request_.http_version_minor == 0);

      closeConnection_ = closeConnection_ || request_.closeConnection();

      /*
       * Status line.
       */

      if (http10) {
        buf_ << "HTTP/1.0 ";
      } else {
        buf_ << "HTTP/1.1 ";
      }

      status_strings::toText(buf_, status_);

      if (!http10 && status_ != switching_protocols) {
        /*
         * Date header (current time)
         */
        buf_ << "Date: ";
        Wt::DateUtils::httpDateBuf(std::chrono::system_clock::now(), buf_);
        buf_ << "\r\n";
      }

      /*
       * Content type or location
       */

      std::string ct;
      if (status_ >= 300 && status_ < 400) {
        if (!location().empty()) {
          buf_ << "Location: " << location() << "\r\n";
        }
      } else if (status_ != not_modified && status_ != switching_protocols) {
        ct = contentType();
        buf_ << "Content-Type: " << ct << "\r\n";
      }

      /*
       * Other provided headers
       */
      bool haveContentEncoding = false;
      for (unsigned i = 0; i < headers_.size(); ++i) {
        if (headers_[i].first == "Content-Encoding")
          haveContentEncoding = true;
        buf_ << headers_[i].first << ": " << headers_[i].second << "\r\n";
      }

      ::int64_t cl = -1;

      if (status_ != not_modified)
        cl = contentLength();
      else
        cl = 0;

      /*
       * We would need to figure out the content length based on the
       * response data, but this doesn't work: WtReply reuses the
       * same buffers over and over, expecting them to be sent
       * inbetween each call to nextContentBuffer()
       */
      if ((cl == -1) && http10)
        closeConnection_ = true;

      /*
       * Connection
       */
      if (closeConnection_ && request_.type == Request::HTTP) {
        buf_ << "Connection: close\r\n";
      } else {
        if (http10) {
          buf_ << "Connection: keep-alive\r\n";
        }
      }

      if (status_ != not_modified) {
#ifdef WTHTTP_WITH_ZLIB
        /*
         * Content-Encoding: gzip ?
         */
        gzipEncoding_ =
             !haveContentEncoding
          && configuration_.compression()
          && request_.acceptGzipEncoding()
          && (cl == -1)
          && (ct.find("text/html") != std::string::npos
              || ct.find("text/plain") != std::string::npos
              || ct.find("text/javascript") != std::string::npos
              || ct.find("text/css") != std::string::npos
              || ct.find("application/xhtml+xml")!= std::string::npos
              || ct.find("image/svg+xml")!= std::string::npos
              || ct.find("application/octet")!= std::string::npos
              || ct.find("text/x-json") != std::string::npos);

        if (gzipEncoding_) {
          buf_ << "Content-Encoding: gzip\r\n";

          initGzip();
        }
#endif

        /*
         * We do not need to determine the length of the response...
         * Transmit only header first.
         */
        if (cl != -1) {
          buf_ << "Content-Length: " << (long long)cl << "\r\n";
          chunkedEncoding_ = false;
        } else
          if (closeConnection_)
            chunkedEncoding_ = false; // should be false
          else
            if (!http10 && status_ != switching_protocols)
              chunkedEncoding_ = true;

        if (chunkedEncoding_) {
          buf_ << "Transfer-Encoding: chunked\r\n";
        }

        buf_ << "\r\n";

        return nextWrappedContentBuffers(result);
      } else { // status_ == not-modified
        buf_ << "\r\n";

        buf_.asioBuffers(result);
        return true;
      }
    } else { // transmitting (data)
      return nextWrappedContentBuffers(result);
    }
  }

  assert(false);

  return true;
}

bool Reply::closeConnection() const
{
  if (closeConnection_)
    return true;

  if (relay_.get())
    return relay_->closeConnection();
  else
    return false;
}

void Reply::setConnection(ConnectionPtr connection)
{
  connection_ = connection;

  if (relay_.get())
    relay_->setConnection(connection);
}

void Reply::receive()
{
  asio::post(connection_->strand(),
             std::bind(&Connection::readMore,
                       connection_,
                       shared_from_this(),
                       120));
}

void Reply::send()
{
  if (connection_->waitingResponse())
    connection_->setHaveResponse();
  else {
    LOG_DEBUG("Reply: send(): scheduling write response.");

    // We post this since we want to avoid growing the stack indefinitely
    asio::post(connection_->server()->service(),
               connection_->strand().wrap(
                  std::bind(&Connection::startWriteResponse,
                            connection_,
                            shared_from_this())));
  }
}

void Reply::detectDisconnect(const std::function<void()>& callback)
{
  connection_->detectDisconnect(shared_from_this(), callback);
}

void Reply::setRelay(ReplyPtr reply)
{
  if (!transmitting_) {
    relay_ = reply;
    relay_->connection_ = connection_;
  }
}

void Reply::logReply(Wt::WLogger& logger)
{
  if (relay_.get())
    return relay_->logReply(logger);

  if (logger.logging("access", WT_LOGGER)) {
    Wt::WStringStream msg;
    msg << request_.remoteIP << " "
        << /* rfc931 << */ " "
        << /* authuser << */ " "
        << request_.method.str() << ' '
        << request_.uri.str() << " HTTP/"
        << request_.http_version_major << '.'
        << request_.http_version_minor << " "
        << status_ << " "
        << std::to_string(contentSent_);

    if (configuration_.accessLog().empty()) {
      LOG_ACCESS(msg.str());
    } else {
      Wt::WLogEntry e = logger.entry("access");
      e << Wt::WLogger::timestamp << Wt::WLogger::sep
        << getpid() << Wt::WLogger::sep
        << /* sessionId << */ Wt::WLogger::sep
        << "[access]" << Wt::WLogger::sep
        << WT_LOGGER << ": " << msg.str();
    }
  }
  /*
     if (gzipEncoding_)
     std::cerr << " <" << contentOriginalSize_ << ">";
     */
}

asio::const_buffer Reply::buf(const std::string &s)
{
  bufs_.push_back(s);
  return asio::buffer(bufs_.back());
}

#ifdef WTHTTP_WITH_ZLIB
void Reply::initGzip()
{
  gzipStrm_.zalloc = Z_NULL;
  gzipStrm_.zfree = Z_NULL;
  gzipStrm_.opaque = Z_NULL;
  gzipStrm_.next_in = Z_NULL;
  int r = 0;
  r = deflateInit2(&gzipStrm_, Z_DEFAULT_COMPRESSION,
                   Z_DEFLATED, 15+16, 8, Z_DEFAULT_STRATEGY);
  gzipBusy_ = true;
  assert(r == Z_OK);
}
#endif

bool Reply::encodeNextContentBuffer(
       std::vector<asio::const_buffer>& result, int& originalSize,
       int& encodedSize)
{
  std::vector<asio::const_buffer> buffers;
  bool lastData = nextContentBuffers(buffers);

  originalSize = 0;

#ifdef WTHTTP_WITH_ZLIB
  if (gzipEncoding_) {
    encodedSize = 0;

    if (lastData && buffers.empty())
      buffers.push_back(asio::buffer((void *)(&encodedSize), 0));

    for (unsigned i = 0; i < buffers.size(); ++i) {
      const asio::const_buffer& b = buffers[i];
      int bs = buffer_size(b); // std::size_t ?
      originalSize += bs;

      gzipStrm_.avail_in = bs;
      gzipStrm_.next_in = const_cast<unsigned char*>(
        static_cast<const unsigned char*>(b.data()));

      unsigned char out[16*1024];
      do {
        gzipStrm_.next_out = out;
        gzipStrm_.avail_out = sizeof(out);

        int r = 0;
        r = deflate(&gzipStrm_,
                    lastData && (i == buffers.size() - 1) ?
                    Z_FINISH : Z_NO_FLUSH);

        assert(r != Z_STREAM_ERROR);

        unsigned have = sizeof(out) - gzipStrm_.avail_out;

        if (have) {
          encodedSize += have;
          result.push_back(buf(std::string((char *)out, have)));
        }
      } while (gzipStrm_.avail_out == 0);
    }

    if (lastData) {
      deflateEnd(&gzipStrm_);
      gzipBusy_ = false;
    }
  } else {
#endif
    for (unsigned i = 0; i < buffers.size(); ++i) {
      const asio::const_buffer& b = buffers[i];
      int bs = buffer_size(b); // std::size_t ?
      originalSize += bs;

      if (bs)
        result.push_back(b);
    }

    encodedSize = originalSize;
#ifdef WTHTTP_WITH_ZLIB
    return lastData;
  }
#endif

  return lastData;
}

void Reply::addDefaultHeaders()
{
  if (wtConfig_) {
    const std::vector<Wt::HttpHeader>& defaultHeaders = wtConfig_->httpHeaders();
    for (size_t i = 0; i < defaultHeaders.size(); ++i) {
      const Wt::HttpHeader& header = defaultHeaders[i];
      addHeader(header.name(), header.contents());
    }
  }
}

} // namespace server
} // namespace http
