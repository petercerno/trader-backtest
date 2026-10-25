// Copyright © 2026 Peter Cerno. All rights reserved.

#include "util/fxmacrodata.h"

#include <string>

#include "gtest/gtest.h"

namespace trader {
namespace {

using Headers = FxMacroDataClient::Headers;

std::string FindHeader(const Headers& headers, const std::string& name) {
  for (const auto& header : headers) {
    if (header.first == name) {
      return header.second;
    }
  }
  return "";
}

}  // namespace

TEST(FxMacroDataClientTest, BuildsAuthenticatedRestRequests) {
  const std::string response =
      "{\"currency\":\"USD\",\"indicator\":\"inflation\",\"data\":"
      "[{\"date\":\"2026-08-31\",\"val\":3.4,"
      "\"announcement_datetime\":1789129800}]}";
  std::string method;
  std::string url;
  Headers headers;
  std::string body;
  FxMacroDataClient client("test-key", "https://api.fxmacrodata.com/v1/",
                           [&](const std::string& m, const std::string& u,
                               const Headers& h, const std::string& b) {
                             method = m;
                             url = u;
                             headers = h;
                             body = b;
                             return response;
                           });

  EXPECT_EQ(client.Announcements(
                "USD", "inflation",
                {{"start_date", "2026-01-01"}, {"end_date", "2026-09-28"}}),
            response);
  EXPECT_EQ(method, "GET");
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/announcements/usd/inflation"
            "?start_date=2026-01-01&end_date=2026-09-28");
  EXPECT_EQ(FindHeader(headers, "X-API-Key"), "test-key");
  EXPECT_EQ(FindHeader(headers, "Accept"), "application/json");
  EXPECT_TRUE(body.empty());
  EXPECT_EQ(url.find("api_key"), std::string::npos);

  client.Calendar("USD", {{"indicator", "inflation"}});
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/calendar/usd?indicator=inflation");

  client.RateDifferentials("EUR", "USD", {{"measure", "spread"}});
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/rate_differentials/eur/usd"
            "?measure=spread");

  client.Forex("EUR", "USD",
               {{"start_date", "2026-01-01"},
                {"limit", "100"},
                {"offset", "200"}});
  EXPECT_EQ(url,
            "https://api.fxmacrodata.com/v1/forex/eur/usd"
            "?start_date=2026-01-01&limit=100&offset=200");

  client.MarketSessions();
  EXPECT_EQ(url, "https://api.fxmacrodata.com/v1/market_sessions");
}

TEST(FxMacroDataClientTest, OmitsKeyHeaderWithoutKey) {
  Headers headers;
  FxMacroDataClient client("", "https://api.fxmacrodata.com/v1",
                           [&](const std::string&, const std::string&,
                               const Headers& h, const std::string&) {
                             headers = h;
                             return std::string("{\"data\":[]}");
                           });
  if (!FindHeader(client.BuildHeaders(), "X-API-Key").empty()) {
    GTEST_SKIP() << "FXMACRODATA_API_KEY is set in the environment";
  }

  EXPECT_EQ(client.DataCatalogue("usd"), "{\"data\":[]}");
  EXPECT_EQ(FindHeader(headers, "X-API-Key"), "");
}

}  // namespace trader
