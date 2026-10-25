#include "util/fxmacrodata.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
void Expect(const std::string& actual, const std::string& expected) {
  if (actual != expected) throw std::runtime_error("unexpected request URL");
}
}  // namespace

int main() {
  const std::string base = "https://example.invalid/v1";
  trader::FxMacroDataClient client("sample-test-key", base);
  const auto invalidBase = [](const std::string& url) {
    try {
      trader::FxMacroDataClient invalid("sample-test-key", url);
      (void)invalid;
    } catch (const std::invalid_argument& error) {
      if (std::string(error.what()).find("sample-test-key") != std::string::npos)
        throw std::runtime_error("base URL diagnostic contains a credential");
      return;
    }
    throw std::runtime_error("invalid base URL accepted");
  };
  const std::vector<std::pair<std::string, std::function<void()>>> tests = {
      {"canonical URL", [&]() {
         Expect(client.BuildUrl("calendar/usd", {{"limit", "2"}}),
                base + "/calendar/usd?limit=2");
       }},
      {"query encoding", [&]() {
         Expect(client.BuildUrl("/calendar/usd", {{"note", "a & b"}}),
                base + "/calendar/usd?note=a%20%26%20b");
       }},
      {"authenticated transport", [&]() {
         trader::FxMacroDataClient signed_client(
             "sample-test-key", base,
             [&](const std::string& method, const std::string& url,
                 const trader::FxMacroDataClient::Headers& headers,
                 const std::string& body) {
               Expect(method, "GET");
               Expect(url, base + "/calendar/usd");
               if (headers.size() != 2 || !body.empty()) {
                 throw std::runtime_error("unexpected transport fields");
               }
               Expect(headers[1].first, "X-API-Key");
               Expect(headers[1].second, "sample-test-key");
               return std::string("response");
             });
         Expect(signed_client.Calendar("USD"), "response");
       }},
      {"existing query", [&]() {
         Expect(client.BuildUrl("/calendar/usd?indicator=inflation", {{"limit", "2"}}),
                base + "/calendar/usd?indicator=inflation&limit=2");
       }},
      {"fragment without query", [&]() {
         Expect(client.BuildUrl("/calendar/usd#view", {{"limit", "2"}}),
                base + "/calendar/usd?limit=2#view");
       }},
      {"query and fragment", [&]() {
         Expect(client.BuildUrl("/calendar/usd?indicator=inflation#view", {{"limit", "2"}}),
                base + "/calendar/usd?indicator=inflation&limit=2#view");
       }},
      {"trailing question mark", [&]() {
         Expect(client.BuildUrl("/calendar/usd?", {{"limit", "2"}}),
                base + "/calendar/usd?limit=2");
       }},
      {"trailing ampersand", [&]() {
         Expect(client.BuildUrl("/calendar/usd?x=1&", {{"limit", "2"}}),
                base + "/calendar/usd?x=1&limit=2");
       }},
      {"unchanged URL without new parameters", [&]() {
         Expect(client.BuildUrl("/calendar/usd?indicator=inflation#view"),
                base + "/calendar/usd?indicator=inflation#view");
       }},
      {"empty base", [&]() { invalidBase(""); }},
      {"non-HTTP base", [&]() { invalidBase("ftp://example.invalid/v1/"); }},
      {"missing host", [&]() { invalidBase("https:///v1/"); }},
      {"credential-bearing base", [&]() { invalidBase("https://user:sample-test-key@example.invalid/v1/"); }},
      {"query-bearing base", [&]() { invalidBase("https://example.invalid/v1/?token=sample-test-key"); }},
      {"fragment-bearing base", [&]() { invalidBase("https://example.invalid/v1/#sample-test-key"); }},
      {"control character in base", [&]() { invalidBase("https://example.invalid/v1/\n"); }},
      {"backslash in base", [&]() { invalidBase("https://example.invalid\\other/v1/"); }},
      {"invalid port", [&]() { invalidBase("https://example.invalid:abc/v1/"); }},
      {"unclosed IPv6 base", [&]() { invalidBase("https://[::1/v1/"); }},
      {"invalid percent escape", [&]() { invalidBase("https://example.invalid/%xx/"); }},
      {"HTTP port", [&]() { Expect(trader::FxMacroDataClient("sample-test-key", "http://127.0.0.1:8080/v1/").BuildUrl("/calendar/usd"), "http://127.0.0.1:8080/v1/calendar/usd"); }},
      {"IPv6 port", [&]() { Expect(trader::FxMacroDataClient("sample-test-key", "https://[::1]:8443/v1/").BuildUrl("/calendar/usd"), "https://[::1]:8443/v1/calendar/usd"); }},
      {"upper-case scheme", [&]() { Expect(trader::FxMacroDataClient("sample-test-key", "HTTPS://example.invalid/v1/").BuildUrl("/calendar/usd"), "HTTPS://example.invalid/v1/calendar/usd"); }},
      {"escaped base path", [&]() { Expect(trader::FxMacroDataClient("sample-test-key", "https://example.invalid/a%20b/v1/").BuildUrl("/calendar/usd"), "https://example.invalid/a%20b/v1/calendar/usd"); }},
  };
  int failed = 0;
  for (const auto& test : tests) {
    try {
      test.second();
      std::cout << "PASS " << test.first << '\n';
    } catch (const std::exception&) {
      ++failed;
      std::cout << "FAIL " << test.first << '\n';
    }
  }
  std::cout << "SUMMARY passed=" << tests.size() - failed << " failed=" << failed << '\n';
  return failed ? 1 : 0;
}
