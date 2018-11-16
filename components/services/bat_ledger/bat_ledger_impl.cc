/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/services/bat_ledger/bat_ledger_impl.h"

#include "brave/components/services/bat_ledger/bat_ledger_client_mojo_proxy.h"

namespace bat_ledger {

BatLedgerImpl::BatLedgerImpl(
    mojom::BatLedgerClientAssociatedPtrInfo client_info)
  : bat_ledger_client_mojo_proxy_(
      new BatLedgerClientMojoProxy(std::move(client_info))),
    ledger_(
      ledger::Ledger::CreateInstance(bat_ledger_client_mojo_proxy_.get())) {
}

BatLedgerImpl::~BatLedgerImpl() {
}

void BatLedgerImpl::Test() {
  LOG(ERROR) << __PRETTY_FUNCTION__;
  ledger_->Test();
}

} // namespace bat_ledger
