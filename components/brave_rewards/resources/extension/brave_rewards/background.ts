/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import './background/store'
import './background/events/rewardsEvents'
import './background/events/tabEvents'

chrome.browserAction.setBadgeBackgroundColor({ color: '#FF0000' })

chrome.runtime.onInstalled.addListener(function (details) {
  if (details.reason === 'install') {
    const initialNotificationDismissed = 'false'
    chrome.storage.local.set({
      'is_dismissed': initialNotificationDismissed
    }, function () {
      chrome.browserAction.setBadgeText({
        text: '1'
      })
    })
  }
})

chrome.runtime.onStartup.addListener(function () {
  chrome.storage.local.get(['is_dismissed'], function (result) {
    if (result && result['is_dismissed'] === 'false') {
      chrome.browserAction.setBadgeText({
        text: '1'
      })
    }
  })
})

chrome.runtime.onConnect.addListener(function () {
  chrome.storage.local.get(['is_dismissed'], function (result) {
    if (result && result['is_dismissed'] === 'false') {
      chrome.browserAction.setBadgeText({
        text: ''
      })
      chrome.storage.local.remove(['is_dismissed'])
    }
  })
})

// Detect User Tip requests via network calls
// Twitter
const requestFilter = {
  urls: ['https://api.twitter.com/1.1/favorites/create.json']
}
async function requestCallback (details: any) {
  if (details.method !== 'POST' || !details.requestBody || !details.requestBody.formData) {
    console.error('Received a favorite request, but not a POST, was a ' + details.method)
    return
  }
  const { formData } = details.requestBody
  if (!formData.id || !formData.id.length) {
    console.error('Received a favorite request, but no item ID', details.formData)
    return
  }
  const tweetId = formData.id[0]
  console.log(`Tab ${details.tabId} received a favorite request for tweet ${tweetId}`)
  // create and notify the tab content script
  chrome.tabs.executeScript(details.tabId, {
    file: '/out/brave_rewards_content_twitter.bundle.js'
  }, () => {
    chrome.tabs.sendMessage(details.tabId, { userAction: 'USER_TIP', tweetId })
  })
}
chrome.webRequest.onBeforeRequest.addListener(requestCallback, requestFilter, ['requestBody'])

