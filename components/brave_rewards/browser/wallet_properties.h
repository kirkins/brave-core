/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_PAYMENTS_WALLET_PROPERTIES_
#define BRAVE_BROWSER_PAYMENTS_WALLET_PROPERTIES_

#include <string>
#include <map>
#include <vector>
#include "brave/components/brave_rewards/browser/grant.h"

namespace brave_rewards {
  struct WalletProperties {
    WalletProperties();
    ~WalletProperties();
    WalletProperties(const WalletProperties& properties);

    std::string probi;
    double balance;
    double monthly_amount;
    std::map<std::string, double> rates;
    std::vector<double> parameters_choices;
    std::vector<double> parameters_range;
    unsigned int parameters_days;
    std::vector<Grant> grants;
  };

}  // namespace brave_rewards

#endif //BRAVE_BROWSER_PAYMENTS_WALLET_PROPERTIES_
