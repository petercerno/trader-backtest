// Copyright © 2026 Peter Cerno. All rights reserved.

#include "util/fxmacrodata.h"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace trader {
namespace {

std::string NormalizeBaseUrl(std::string baseUrl) {
  const auto invalid = []() {
    throw std::invalid_argument("FXMacroData base URL must be an HTTP(S) URL with a host and no credentials, query or fragment");
  };
  const std::size_t schemeEnd = baseUrl.find("://");
  if (schemeEnd == std::string::npos)
  {
    invalid();
  }
  std::string scheme = baseUrl.substr(0, schemeEnd);
  for (char& ch : scheme)
  {
    if (ch >= 'A' && ch <= 'Z') ch = static_cast<char>(ch + ('a' - 'A'));
  }
  if (scheme != "http" && scheme != "https") invalid();
  for (std::size_t i = 0; i < baseUrl.size(); ++i)
  {
    const unsigned char ch = static_cast<unsigned char>(baseUrl[i]);
    if (ch <= 0x20 || ch >= 0x7f || ch == '\\' || ch == '?' || ch == '#') invalid();
    if (ch == '%')
    {
      const auto hex = [](char value) {
        return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F');
      };
      if (i + 2 >= baseUrl.size() || !hex(baseUrl[i + 1]) || !hex(baseUrl[i + 2])) invalid();
    }
  }
  const std::size_t hostStart = schemeEnd + 3;
  const std::size_t pathStart = baseUrl.find('/', hostStart);
  const std::string authority = baseUrl.substr(hostStart, pathStart - hostStart);
  if (authority.empty() || authority.find('@') != std::string::npos) invalid();
  std::string host;
  std::string port;
  bool hasPort = false;
  if (authority[0] == '[')
  {
    const std::size_t close = authority.find(']');
    if (close == std::string::npos || close == 1) invalid();
    host = authority.substr(1, close - 1);
    if (host.find(':') == std::string::npos) invalid();
    for (char ch : host)
    {
      if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || ch == ':' || ch == '.')) invalid();
    }
    if (close + 1 < authority.size())
    {
      if (authority[close + 1] != ':') invalid();
      hasPort = true;
      port = authority.substr(close + 2);
    }
  }
  else
  {
    const std::size_t colon = authority.find(':');
    host = authority.substr(0, colon);
    hasPort = colon != std::string::npos;
    if (hasPort) port = authority.substr(colon + 1);
    if (host.empty()) invalid();
    for (char ch : host)
    {
      if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '.' || ch == '-' || ch == '_')) invalid();
    }
  }
  if (hasPort)
  {
    if (port.empty() || port.size() > 5) invalid();
    unsigned int number = 0;
    for (char ch : port)
    {
      if (ch < '0' || ch > '9') invalid();
      number = number * 10 + static_cast<unsigned int>(ch - '0');
    }
    if (number == 0 || number > 65535) invalid();
  }
  while (!baseUrl.empty() && baseUrl.back() == '/')
  {
    baseUrl.pop_back();
  }
  return baseUrl;
}

std::string EnsureLeadingSlash(const std::string& path) {
  if (!path.empty() && path.front() == '/') {
    return path;
  }
  return "/" + path;
}

std::string Lower(std::string value) {
  for (char& ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

std::string Encode(const std::string& value) {
  std::ostringstream out;
  out << std::uppercase << std::hex;
  for (unsigned char ch : value) {
    if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      out << static_cast<char>(ch);
    } else {
      out << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }
  }
  return out.str();
}

std::string ApiKeyFromEnv() {
  const char* value = std::getenv("FXMACRODATA_API_KEY");
  return value == nullptr ? "" : value;
}

}  // namespace

FxMacroDataClient::FxMacroDataClient(std::string api_key, std::string base_url,
                                     Transport transport)
    : api_key_(api_key.empty() ? ApiKeyFromEnv() : std::move(api_key)),
      base_url_(NormalizeBaseUrl(std::move(base_url))),
      transport_(std::move(transport)) {}

std::string FxMacroDataClient::BuildUrl(const std::string& path,
                                        QueryParams params) const {
  std::ostringstream url;
  const std::string endpoint = base_url_ + EnsureLeadingSlash(path);
  const auto fragment_pos = endpoint.find('#');
  const std::string query_url = endpoint.substr(0, fragment_pos);
  const std::string fragment = fragment_pos == std::string::npos
                                   ? ""
                                   : endpoint.substr(fragment_pos);
  url << query_url;
  char separator = query_url.find('?') == std::string::npos ? '?' : '&';
  if (!query_url.empty() &&
      (query_url.back() == '?' || query_url.back() == '&')) {
    separator = '\0';
  }
  for (const auto& param : params) {
    if (separator != '\0') {
      url << separator;
    }
    url << Encode(param.first) << '=' << Encode(param.second);
    separator = '&';
  }
  url << fragment;
  return url.str();
}

std::string FxMacroDataClient::Send(const std::string& method,
                                    const std::string& path, QueryParams params,
                                    const std::string& body) const {
  if (!transport_) {
    throw std::runtime_error("FxMacroDataClient requires a transport callback");
  }
  return transport_(method, BuildUrl(path, std::move(params)), BuildHeaders(),
                    body);
}

FxMacroDataClient::Headers FxMacroDataClient::BuildHeaders() const {
  Headers headers = {{"Accept", "application/json"}};
  if (!api_key_.empty()) {
    headers.push_back({"X-API-Key", api_key_});
  }
  return headers;
}

std::string FxMacroDataClient::DataCatalogue(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/data_catalogue/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::Announcements(const std::string& currency,
                                             const std::string& indicator,
                                             QueryParams params) const {
  return Send(
      "GET",
      "/announcements/" + Encode(Lower(currency)) + "/" + Encode(indicator),
      std::move(params));
}

std::string FxMacroDataClient::LatestAnnouncements(const std::string& currency,
                                                   QueryParams params) const {
  return Send("GET", "/announcements/" + Encode(Lower(currency)) + "/latest",
              std::move(params));
}

std::string FxMacroDataClient::AnnouncementChanges(QueryParams params) const {
  return Send("GET", "/announcements/changes", std::move(params));
}

std::string FxMacroDataClient::Calendar(const std::string& currency,
                                        QueryParams params) const {
  return Send("GET", "/calendar/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::Predictions(const std::string& currency,
                                           const std::string& indicator,
                                           QueryParams params) const {
  return Send(
      "GET",
      "/predictions/" + Encode(Lower(currency)) + "/" + Encode(indicator),
      std::move(params));
}

std::string FxMacroDataClient::Forex(const std::string& base,
                                     const std::string& quote,
                                     QueryParams params) const {
  return Send("GET",
              "/forex/" + Encode(Lower(base)) + "/" + Encode(Lower(quote)),
              std::move(params));
}

std::string FxMacroDataClient::Cot(const std::string& currency,
                                   QueryParams params) const {
  return Send("GET", "/cot/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::Commodity(const std::string& indicator,
                                         QueryParams params) const {
  return Send("GET", "/commodities/" + Encode(indicator), std::move(params));
}

std::string FxMacroDataClient::CommoditiesLatest(QueryParams params) const {
  return Send("GET", "/commodities/latest", std::move(params));
}

std::string FxMacroDataClient::Curves(const std::string& currency,
                                      QueryParams params) const {
  return Send("GET", "/curves/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::CurveProxies(const std::string& currency,
                                            QueryParams params) const {
  return Send("GET", "/curve_proxies/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::ForwardCurves(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/forward_curves/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::RateDifferentials(const std::string& base,
                                                 const std::string& quote,
                                                 QueryParams params) const {
  return Send(
      "GET",
      "/rate_differentials/" + Encode(Lower(base)) + "/" + Encode(Lower(quote)),
      std::move(params));
}

std::string FxMacroDataClient::ForwardDifferentials(const std::string& base,
                                                    const std::string& quote,
                                                    QueryParams params) const {
  return Send("GET",
              "/forward_differentials/" + Encode(Lower(base)) + "/" +
                  Encode(Lower(quote)),
              std::move(params));
}

std::string FxMacroDataClient::MarketSessions(QueryParams params) const {
  return Send("GET", "/market_sessions", std::move(params));
}

std::string FxMacroDataClient::RiskSentiment(QueryParams params) const {
  return Send("GET", "/risk_sentiment", std::move(params));
}

std::string FxMacroDataClient::News(const std::string& currency,
                                    QueryParams params) const {
  return Send("GET", "/news/" + Encode(Lower(currency)), std::move(params));
}

std::string FxMacroDataClient::PressReleases(const std::string& currency,
                                             QueryParams params) const {
  return Send("GET", "/press-releases/" + Encode(Lower(currency)),
              std::move(params));
}

std::string FxMacroDataClient::Request(const std::string& path,
                                       QueryParams params, std::string method,
                                       std::string body) const {
  return Send(std::move(method), path, std::move(params), body);
}

}  // namespace trader
