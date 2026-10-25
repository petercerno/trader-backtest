// Copyright © 2026 Peter Cerno. All rights reserved.

#ifndef UTIL_FXMACRODATA_H
#define UTIL_FXMACRODATA_H

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace trader {

class FxMacroDataClient {
 public:
  using QueryParams = std::vector<std::pair<std::string, std::string>>;
  using Headers = std::vector<std::pair<std::string, std::string>>;
  using Transport = std::function<std::string(
      const std::string& method, const std::string& url, const Headers& headers,
      const std::string& body)>;

  // If api_key is empty, the FXMACRODATA_API_KEY environment variable is used.
  // The key is sent in the X-API-Key header. USD announcements, the USD
  // calendar and the USD data catalogue also work without a key.
  //
  // List endpoints (announcements, predictions, forex, COT, commodities)
  // return 20 rows by default and at most 100 per request, newest first.
  // Pass {"limit", "100"} and {"offset", "..."} in params and follow
  // pagination.next_offset while pagination.has_more is true.
  explicit FxMacroDataClient(
      std::string api_key = "",
      std::string base_url = "https://api.fxmacrodata.com/v1",
      Transport transport = nullptr);

  std::string DataCatalogue(const std::string& currency,
                            QueryParams params = {}) const;
  std::string Announcements(const std::string& currency,
                            const std::string& indicator,
                            QueryParams params = {}) const;
  std::string LatestAnnouncements(const std::string& currency,
                                  QueryParams params = {}) const;
  std::string AnnouncementChanges(QueryParams params = {}) const;
  std::string Calendar(const std::string& currency,
                       QueryParams params = {}) const;
  std::string Predictions(const std::string& currency,
                          const std::string& indicator,
                          QueryParams params = {}) const;
  std::string Forex(const std::string& base, const std::string& quote,
                    QueryParams params = {}) const;
  std::string Cot(const std::string& currency, QueryParams params = {}) const;
  std::string Commodity(const std::string& indicator,
                        QueryParams params = {}) const;
  std::string CommoditiesLatest(QueryParams params = {}) const;
  std::string Curves(const std::string& currency,
                     QueryParams params = {}) const;
  std::string CurveProxies(const std::string& currency,
                           QueryParams params = {}) const;
  std::string ForwardCurves(const std::string& currency,
                            QueryParams params = {}) const;
  std::string RateDifferentials(const std::string& base,
                                const std::string& quote,
                                QueryParams params = {}) const;
  std::string ForwardDifferentials(const std::string& base,
                                   const std::string& quote,
                                   QueryParams params = {}) const;
  std::string MarketSessions(QueryParams params = {}) const;
  std::string RiskSentiment(QueryParams params = {}) const;
  std::string News(const std::string& currency, QueryParams params = {}) const;
  std::string PressReleases(const std::string& currency,
                            QueryParams params = {}) const;
  std::string Request(const std::string& path, QueryParams params = {},
                      std::string method = "GET", std::string body = "") const;

  std::string BuildUrl(const std::string& path, QueryParams params = {}) const;
  Headers BuildHeaders() const;

 private:
  std::string api_key_;
  std::string base_url_;
  Transport transport_;

  std::string Send(const std::string& method, const std::string& path,
                   QueryParams params, const std::string& body = "") const;
};

}  // namespace trader

#endif  // UTIL_FXMACRODATA_H
