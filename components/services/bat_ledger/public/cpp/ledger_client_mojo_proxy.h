/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_PUBLIC_CPP_LEDGER_CLIENT_MOJO_PROXY_H_
#define BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_PUBLIC_CPP_LEDGER_CLIENT_MOJO_PROXY_H_

#include <map>

#include "bat/ledger/ledger_client.h"
#include "brave/components/services/bat_ledger/public/interfaces/bat_ledger.mojom.h"

namespace bat_ledger {

class LedgerClientMojoProxy : public mojom::BatLedgerClient {
 public:
   LedgerClientMojoProxy(ledger::LedgerClient* ledger_client);
   ~LedgerClientMojoProxy() override;

   // bat_ledger::mojom::BatLedgerClient
   void Test() override;
 private:
   ledger::LedgerClient* ledger_client_;

   DISALLOW_COPY_AND_ASSIGN(LedgerClientMojoProxy);
};

} // namespace bat_ledger

#endif  // BRAVE_COMPONENTS_SERVICES_BAT_LEDGER_PUBLIC_CPP_LEDGER_CLIENT_MOJO_PROXY_H_
