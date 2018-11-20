/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/files/file.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/threading/thread_restrictions.h"
#include "brave/browser/extensions/brave_component_loader.h"
#include "brave/browser/extensions/brave_extension_functional_test.h"
#include "brave/common/brave_switches.h"
#include "brave/common/extensions/extension_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/common/pdf_util.h"
#include "chrome/common/pref_names.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/component_updater/component_updater_paths.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/test_utils.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/constants.h"
#include "extensions/common/manifest_handlers/mime_types_handler.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/zlib/google/zip.h"

using namespace extensions;

namespace {

const std::string kTestPdfjsExtensionId("kpbdcmcgkedhpbcpfndimofjnefgjidd");
const std::string kTestPdfjsExtensionBase64PublicKey(
  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoCrAa1pOkCauGmUgZ5teW/S48PdgKLd1"
  "zyhhBI+QIi8Eea1X5N7Drdr59xm1ueS8Dp3QSYOFjrcQG6hEUbFPxm2UF3hQiyb68CuqNv98K32p"
  "83Uemi711+U7EGyWfz3ZY/D0GbXCz0GeGzyPSyF/56KtUDGqi1aYjrTq+4egHfYrVYPhcEMGjO5J"
  "v0yh5+a+MvzjHHZ8m8lfqYmpKbbeq0o0XGTTB4cOTZfuCMo7oWI6piAYit0pDDt/53uYSMJpn69Y"
  "YN08rZOfSLPV4wu/5oc2dw8rYWkG+j+Gbdt7gk3MjPuUavLc3aiCxmj9s3uLTSTFxnVFSFETWk6E"
  "nEP36wIDAQAB");
const std::string kTestPdfjsExtensionVersion("2.0.466");

class UnzipTestPdfjsComponent {
public:
  UnzipTestPdfjsComponent() = default;
  ~UnzipTestPdfjsComponent() {
    Cleanup();
  }

  bool UnzipTo(const base::FilePath& test_data_dir) {
    base::ScopedAllowBlockingForTesting allow_blocking;
    base::FilePath base_component_dir;
    if (!base::PathService::Get(component_updater::DIR_COMPONENT_USER,
                                &base_component_dir))
      return false;
    component_dir_ = base_component_dir.AppendASCII(kTestPdfjsExtensionId);
    base::FilePath unzip_dir =
        component_dir_.AppendASCII(kTestPdfjsExtensionVersion);

    base::File::Error error = base::File::FILE_OK;
    EXPECT_TRUE(base::CreateDirectoryAndGetError(unzip_dir, &error));

    base::FilePath zip_file = test_data_dir.AppendASCII("pdfjs.crx");
    return zip::Unzip(zip_file, unzip_dir);
  }

  void Cleanup() const {
    if (base::PathExists(component_dir_))
      base::DeleteFile(component_dir_, true);
  }

private:
  base::FilePath component_dir_;
};

class DownloadManagerObserverHelper
    : public content::DownloadManager::Observer {
 public:
  DownloadManagerObserverHelper(Browser* browser)
      : browser_(browser), download_manager_(nullptr) {
    // Set preference to not ask where to save downloads.
    browser_->profile()->GetPrefs()->SetBoolean(prefs::kPromptForDownload,
                                                false);
  }
  
  ~DownloadManagerObserverHelper() override {
    if (download_manager_)
      download_manager_->RemoveObserver(this);
  }

  void Observe() {
    // Register a download observer.
    content::WebContents* contents =
      browser_->tab_strip_model()->GetActiveWebContents();
    content::BrowserContext* browser_context = contents->GetBrowserContext();
    download_manager_ =
        content::BrowserContext::GetDownloadManager(browser_context);
    download_manager_->AddObserver(this);
  }

  void CheckForDownload(const GURL& url) {
    // Wait for the url to load and check that it loaded.
    ASSERT_EQ(url, GetLastUrl());
    // Check that one download has been initiated.
    DCHECK(download_manager_);
    std::vector<download::DownloadItem*> downloads;
    download_manager_->GetAllDownloads(&downloads);
    ASSERT_EQ(1u, downloads.size());
    // Cancel download to shut down cleanly.
    downloads[0]->Cancel(false);
  }

  const GURL& GetLastUrl() {
    // Wait until the download has been created.
    download_run_loop_.Run();
    return last_url_;
  }

  // content::DownloadManager::Observer implementation.
  void OnDownloadCreated(content::DownloadManager* manager,
                         download::DownloadItem* item) override {
    last_url_ = item->GetURL();
    download_run_loop_.Quit();
  }

 private:
  Browser* browser_;
  content::DownloadManager* download_manager_;
  content::NotificationRegistrar registrar_;
  base::RunLoop download_run_loop_;
  GURL last_url_;
};

class PdfjsExtensionExtensionRegistryObserver
  : public extensions::ExtensionRegistryObserver {
public:
  explicit PdfjsExtensionExtensionRegistryObserver(
    extensions::ExtensionRegistry* registry)
    : message_loop_runner_(new content::MessageLoopRunner),
    added_(false),
    removed_(false),
    timeout_(0),
    elapsed_(0),
    registry_(registry) {
    registry_->AddObserver(this);
  }

  ~PdfjsExtensionExtensionRegistryObserver() override {
    registry_->RemoveObserver(this);
  }

  bool added() const { return added_; }
  bool removed() const { return removed_; }

  bool Wait(int timeout_secs = 0) {
    if (added_ || removed_)
      return true;
    if (timeout_secs) {
      timeout_ = timeout_secs;
      elapsed_ = 0;
      timer_.Start(FROM_HERE, base::TimeDelta::FromSeconds(1), this,
                   &PdfjsExtensionExtensionRegistryObserver::OnTimer);
    }
    message_loop_runner_->Run();
    return (added_ || removed_);
  }

  void OnTimer() {
    if (++elapsed_ >= timeout_ || added_ || removed_)
      timer_.Stop();
    if (message_loop_runner_->loop_running())
      base::ThreadTaskRunnerHandle::Get()->PostTask(
          FROM_HERE, message_loop_runner_->QuitClosure());
  }

  void Reset() {
    added_ = false;
    removed_ = false;
  }

  // ExtensionRegistryObserver implementation:
  void OnExtensionLoaded(content::BrowserContext* browser_context,
    const extensions::Extension* extension) override {
    ASSERT_STREQ(extension->id().c_str(), kTestPdfjsExtensionId.c_str());
    CHECK(!added_);
    added_ = true;
    if (added_ && message_loop_runner_->loop_running()) {
      base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, message_loop_runner_->QuitClosure());
    }
  }

  void OnExtensionUnloaded(
    content::BrowserContext* browser_context,
    const extensions::Extension* extension,
    extensions::UnloadedExtensionReason reason) override {
    ASSERT_STREQ(extension->id().c_str(), kTestPdfjsExtensionId.c_str());
    CHECK(!removed_);
    removed_ = true;
    if (removed_ && message_loop_runner_->loop_running()) {
      base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, message_loop_runner_->QuitClosure());
    }
  }

private:
  scoped_refptr<content::MessageLoopRunner> message_loop_runner_;
  bool added_;
  bool removed_;
  int timeout_;
  int elapsed_;
  base::RepeatingTimer timer_;
  extensions::ExtensionRegistry* registry_;
};

} //namespace

class BravePDFExtensionTest : public ExtensionFunctionalTest {
public:
  BravePDFExtensionTest() {
    // Configure BraveComponentLoader to use test pdfjs.
    BraveComponentLoader::SetPdfjsExtensionIdAndBase64PublicKeyForTest(
      kTestPdfjsExtensionId, kTestPdfjsExtensionBase64PublicKey);
  }
  ~BravePDFExtensionTest() override = default;

protected:
  bool ShouldEnableInstallVerification() override {
    // Simulates behavior in chrome/browser/extensions/install_verifier.cc's
    // GetExperimentStatus.
#if (defined(OS_WIN) || defined(OS_MACOSX))
    return true;
#else
    return false;
#endif
  }

  void SetDownloadPDFs(bool value) {
    DCHECK(browser());
    browser()->profile()->GetPrefs()->SetBoolean(
      prefs::kPluginsAlwaysOpenPdfExternally, value);
  }

  void LoadPDFJSComponent(int timeout = 0, bool expect_loaded = true) {
    ExtensionService* service =
      ExtensionSystem::Get(browser()->profile())->extension_service();
    service->component_loader()->set_ignore_whitelist_for_testing(true);
    // In browser test BraveComponentLoader doesn't actually load PDFJS
    // component, so instead we unzip it where the component loader would
    // expect to find the component and the toggle the preference to load it.
    base::FilePath test_data_dir;
    GetTestDataDir(&test_data_dir);
    EXPECT_FALSE(test_data_dir.empty());
    ASSERT_TRUE(unzipTestPdfjsComponent.UnzipTo(test_data_dir));

    SetDownloadPDFs(true);

    DCHECK(browser());
    ExtensionRegistry* registry = ExtensionRegistry::Get(browser()->profile());
    PdfjsExtensionExtensionRegistryObserver observer(registry);
    SetDownloadPDFs(false);
    bool result = observer.Wait(timeout);
    if (expect_loaded)
      ASSERT_TRUE(result && observer.added());
    else
      ASSERT_FALSE(result);
  }

  void CheckPDFJSExtensionLoaded(ExtensionRegistry* registry) {
    // Verify that the PDFJS extension is loaded.
    ASSERT_TRUE(registry->enabled_extensions().Contains(kTestPdfjsExtensionId));
  }

  void CheckCantHandlePDFs(const ExtensionRegistry* registry) const {
    // Verify that there are no extensions that can handle PDFs, except for the
    // Chromium PDF extension which won't be considered due to what is verified
    // in the VerifyChromiumPDFExntension test below.
    const ExtensionSet& enabled_extensions = registry->enabled_extensions();
    for (const auto& extension : enabled_extensions) {
      ASSERT_NE(extension->id(), kTestPdfjsExtensionId);
      MimeTypesHandler* handler = MimeTypesHandler::GetHandler(extension.get());
      if (handler && handler->CanHandleMIMEType(kPDFMimeType))
        ASSERT_STREQ(handler->extension_id().c_str(), extension_misc::kPdfExtensionId);
    }
  }

private:
  UnzipTestPdfjsComponent unzipTestPdfjsComponent;
};

IN_PROC_BROWSER_TEST_F(BravePDFExtensionTest, VerifyChromiumPDFExntension) {
  // On Win and Mac extension install verification puts blacklisted
  // Chromium PDF extension into disabled extensions.
#if (defined(OS_WIN) || defined(OS_MACOSX))
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser()->profile());
  ASSERT_TRUE(
    registry->disabled_extensions().Contains(extension_misc::kPdfExtensionId));
#endif
  // On all platforms we modified whitelist to make sure Chromium PDF
  // extension is not considered for PDF handling.
  const std::vector<std::string> whitelist =
    MimeTypesHandler::BraveGetMIMETypeWhitelist();
  ASSERT_EQ(std::find(whitelist.begin(), whitelist.end(),
    extension_misc::kPdfExtensionId),
    whitelist.end());
}

IN_PROC_BROWSER_TEST_F(BravePDFExtensionTest, ToggleDownloadPDFs) {
  LoadPDFJSComponent();
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser()->profile());
  CheckPDFJSExtensionLoaded(registry);

  // Set preference to always download PDFs and check that we can't handle PDFs.
  PdfjsExtensionExtensionRegistryObserver observer(registry);
  SetDownloadPDFs(true);
  observer.Wait();
  ASSERT_TRUE(observer.removed());
  CheckCantHandlePDFs(registry);

  // Register a download observer.
  DownloadManagerObserverHelper helper(browser());
  helper.Observe();
  // Navigate to a pdf.
  GURL url(embedded_test_server()->GetURL("/test.pdf"));
  ui_test_utils::NavigateToURL(browser(), url);
  // Check that one download has been initiated.
  helper.CheckForDownload(url);

  // Toggle the preference to view PDFs in the browser.
  observer.Reset();
  SetDownloadPDFs(false);
  observer.Wait();
  ASSERT_TRUE(observer.added());
  CheckPDFJSExtensionLoaded(registry);
}

class BravePDFExtensionNotLoadedTest : public BravePDFExtensionTest {
public:
  BravePDFExtensionNotLoadedTest() = default;
  ~BravePDFExtensionNotLoadedTest() override = default;

protected:
  void SetUpCommandLine(base::CommandLine* command_line) override {
    ExtensionFunctionalTest::SetUpCommandLine(command_line);
    // Disable loading of our PDF extension.
    command_line->AppendSwitch(switches::kDisablePDFJSExtension);
  }
};

IN_PROC_BROWSER_TEST_F(BravePDFExtensionNotLoadedTest, AlwaysDownloadPDFs) {
  LoadPDFJSComponent(5, false);
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser()->profile());
  CheckCantHandlePDFs(registry);

  // Register a download observer.
  DownloadManagerObserverHelper helper(browser());
  helper.Observe();
  // Navigate to a pdf.
  GURL url(embedded_test_server()->GetURL("/test.pdf"));
  ui_test_utils::NavigateToURL(browser(), url);
  // Check that one download has been initiated.
  helper.CheckForDownload(url);
}
