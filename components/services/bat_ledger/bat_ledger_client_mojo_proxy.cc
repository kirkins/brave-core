/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/services/bat_ledger/bat_ledger_client_mojo_proxy.h"

namespace bat_ledger {

BatLedgerClientMojoProxy::BatLedgerClientMojoProxy(
    mojom::BatLedgerClientAssociatedPtrInfo client_info) {
  bat_ledger_client_.Bind(std::move(client_info));
}

BatLedgerClientMojoProxy::~BatLedgerClientMojoProxy() {
}

std::string BatLedgerClientMojoProxy::GenerateGUID() const {
  return "";
}

std::unique_ptr<ledger::LedgerURLLoader> BatLedgerClientMojoProxy::LoadURL(
    const std::string& url,
    const std::vector<std::string>& headers,
    const std::string& content,
    const std::string& contentType,
    const ledger::URL_METHOD& method,
    ledger::LedgerCallbackHandler* handler) {
  return nullptr;
}

std::string BatLedgerClientMojoProxy::URIEncode(const std::string& value) {
  return "";
}

void BatLedgerClientMojoProxy::OnExcludedSitesChanged() {
//  bat_ledger_client_->OnExcludedSitesChanged();
}

void BatLedgerClientMojoProxy::Test() {
  bat_ledger_client_->Test();
}

} // namespace bat_ledger
