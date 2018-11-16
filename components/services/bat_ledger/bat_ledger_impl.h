/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_IMPL_H_
#define BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_IMPL_H_

#include <memory>

#include "bat/ledger/ledger.h"
#include "brave/components/services/bat_ledger/public/interfaces/bat_ledger.mojom.h"

namespace bat_ledger {

class BatLedgerClientMojoProxy;

class BatLedgerImpl : public mojom::BatLedger {
  public:
    explicit BatLedgerImpl(
        mojom::BatLedgerClientAssociatedPtrInfo client_info);
    ~BatLedgerImpl() override;

    // bat_ledger::mojom::BatLedger
    void Test() override;
  private:
    std::unique_ptr<BatLedgerClientMojoProxy> bat_ledger_client_mojo_proxy_;
    std::unique_ptr<ledger::Ledger> ledger_;

    DISALLOW_COPY_AND_ASSIGN(BatLedgerImpl);
};

} // namespace bat_ledger

#endif // BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_BAT_LEDGER_IMPL_H_
