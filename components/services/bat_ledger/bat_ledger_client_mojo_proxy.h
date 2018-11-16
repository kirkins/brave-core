/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_CLIENT_MOJO_PROXY_H_
#define BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_CLIENT_MOJO_PROXY_H_

#include <map>

#include "bat/ledger/ledger_client.h"
#include "brave/components/services/bat_ledger/public/interfaces/bat_ledger.mojom.h"
#include "chrome/browser/bitmap_fetcher/bitmap_fetcher_service.h"

class SkBitmap;

namespace bat_ledger {

class BatLedgerClientMojoProxy : public ledger::LedgerClient {
 public:
  BatLedgerClientMojoProxy(
      mojom::BatLedgerClientAssociatedPtrInfo client_info);
  ~BatLedgerClientMojoProxy() override;

  std::string GenerateGUID() const override;
  void OnWalletInitialized(ledger::Result result) override {}
  void OnWalletProperties(ledger::Result result,
                          std::unique_ptr<ledger::WalletInfo> info) override {}
  void OnGrant(ledger::Result result, const ledger::Grant& grant) override {}
  void OnGrantCaptcha(const std::string& image, const std::string& hint) override {}
  void OnRecoverWallet(ledger::Result result,
                      double balance,
                      const std::vector<ledger::Grant>& grants) override {}
  void OnReconcileComplete(ledger::Result result,
                           const std::string& viewing_id,
                           ledger::PUBLISHER_CATEGORY category,
                           const std::string& probi) override {}
  void OnGrantFinish(ledger::Result result,
                     const ledger::Grant& grant) override {}
  void LoadLedgerState(ledger::LedgerCallbackHandler* handler) override {}
  void LoadPublisherState(ledger::LedgerCallbackHandler* handler) override {}
  void SaveLedgerState(const std::string& ledger_state,
                       ledger::LedgerCallbackHandler* handler) override {}
  void SavePublisherState(const std::string& publisher_state,
                          ledger::LedgerCallbackHandler* handler) override {}

  void SavePublisherInfo(std::unique_ptr<ledger::PublisherInfo> publisher_info,
                         ledger::PublisherInfoCallback callback) override {}
  void LoadPublisherInfo(ledger::PublisherInfoFilter filter,
                         ledger::PublisherInfoCallback callback) override {}
  void LoadPublisherInfoList(
      uint32_t start,
      uint32_t limit,
      ledger::PublisherInfoFilter filter,
      ledger::PublisherInfoListCallback callback) override {}
  void LoadCurrentPublisherInfoList(
      uint32_t start,
      uint32_t limit,
      ledger::PublisherInfoFilter filter,
      ledger::PublisherInfoListCallback callback) override {}
  void SavePublishersList(const std::string& publishers_list,
                          ledger::LedgerCallbackHandler* handler) override {}
  void SetTimer(uint64_t time_offset, uint32_t& timer_id) override {}
  void LoadPublisherList(ledger::LedgerCallbackHandler* handler) override {}

  std::unique_ptr<ledger::LedgerURLLoader> LoadURL(const std::string& url,
      const std::vector<std::string>& headers,
      const std::string& content,
      const std::string& contentType,
      const ledger::URL_METHOD& method,
      ledger::LedgerCallbackHandler* handler) override;

  void RunIOTask(std::unique_ptr<ledger::LedgerTaskRunner> task) override {}
  void OnExcludedSitesChanged() override;
  void OnPublisherActivity(ledger::Result result,
                          std::unique_ptr<ledger::PublisherInfo> info,
                          uint64_t windowId) override {}
  void FetchFavIcon(const std::string& url,
                    const std::string& favicon_key,
                    ledger::FetchIconCallback callback) override {}
  void SaveContributionInfo(const std::string& probi,
                            const int month,
                            const int year,
                            const uint32_t date,
                            const std::string& publisher_key,
                            const ledger::PUBLISHER_CATEGORY category) override {}
  void GetRecurringDonations(ledger::PublisherInfoListCallback callback) override {}
  void Log(ledger::LogLevel level, const std::string& text) override {}
  void LoadMediaPublisherInfo(
      const std::string& media_key,
      ledger::PublisherInfoCallback callback) override {}
  void SaveMediaPublisherInfo(const std::string& media_key, const std::string& publisher_id) override {}

  void FetchWalletProperties() override {}
  void FetchGrant(const std::string& lang, const std::string& paymentId) override {}
  void GetGrantCaptcha() override {}

  std::string URIEncode(const std::string& value) override;

  void SetContributionAutoInclude(
    std::string publisher_key, bool excluded, uint64_t windowId) override {}

  void Test() override;

 private:
  void LoadNicewareList(ledger::GetNicewareListCallback callback) override {}
  void OnRemoveRecurring(const std::string& publisher_key, ledger::RecurringRemoveCallback callback) override {}

  mojom::BatLedgerClientAssociatedPtr bat_ledger_client_;

  DISALLOW_COPY_AND_ASSIGN(BatLedgerClientMojoProxy);
};

} // namespace bat_ledger

#endif // BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_CLIENT_MOJO_PROXY_H_
