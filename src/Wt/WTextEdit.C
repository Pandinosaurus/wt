/*
 * Copyright (C) 2008 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/WAny.h"
#include "Wt/WApplication.h"
#include "Wt/WEnvironment.h"
#include "Wt/WException.h"
#include "Wt/WTextEdit.h"
#include "Wt/WLogger.h"

#include "DomElement.h"
#include "WebUtils.h"

#include <iostream>
#include <fstream>
#include <regex>

#ifndef WT_DEBUG_JS
#include "js/WTextEdit.min.js"
#endif

namespace Wt {

LOGGER("WTextEdit");

typedef std::map<std::string, cpp17::any> SettingsMapType;

WTextEdit::WTextEdit()
  : onChange_(this, "change"),
    onRender_(this, "render"),
    initialised_(false)
{
  init();
}

WTextEdit::WTextEdit(const WT_USTRING& text)
  : WTextArea(text),
    onChange_(this, "change"),
    onRender_(this, "render"),
    initialised_(false)
{
  init();
}

void WTextEdit::init()
{
  WApplication *app = WApplication::instance();

  setInline(false);

  initTinyMCE();

  version_ = getTinyMCEVersion();

#ifndef WT_TARGET_JAVA
  if (!verifyTinyMCEVersion(version_)){
    LOG_WARN("Version of TinyMCE does not seem to match the version given in the config.");
  }
#endif

  setJavaScriptMember(" WTextEdit", "new " WT_CLASS ".WTextEdit("
                      + app->javaScriptClass() + "," + jsRef() + ");");

  setJavaScriptMember
    (WT_RESIZE_JS,
     "function(e, w, h, s) { var obj = " + jsRef() + ".wtObj; "
     "obj.wtResize(e, w, h, s); };");

  std::string direction
    = app->layoutDirection() == LayoutDirection::LeftToRight ? "ltr" : "rtl";
  setConfigurationSetting("directionality", direction);

  std::string toolbar;
  if (version_ < 4)
    toolbar = "fontselect,|,bold,italic,underline,|,fontsizeselect,|"
      ",forecolor,backcolor,|"
      ",justifyleft,justifycenter,justifyright,justifyfull,|,anchor,|"
      ",numlist,bullist";
  else
    toolbar = "undo redo | styleselect | bold italic | link";

  setToolBar(0, toolbar);
  for (int i = 1; i <= 3; i++)
    setToolBar(i, std::string());

  setConfigurationSetting("doctype", wApp->docType());
  setConfigurationSetting("relative_urls", true);

  if (version_ < 4) {
    //this setting is no longer mentioned in the tinymce documenation though...
    setConfigurationSetting("button_tile_map", true);
    setConfigurationSetting("theme", std::string("advanced"));
    setConfigurationSetting("theme_advanced_toolbar_location",
                            std::string("top"));
    setConfigurationSetting("theme_advanced_toolbar_align",
                            std::string("left"));
  }

  onChange_.connect(this, &WTextEdit::propagateOnChange);
}

WTextEdit::~WTextEdit()
{ }

void WTextEdit::propagateOnChange()
{
  changed().emit();
}

void WTextEdit::setStyleSheet(const std::string& uri)
{
  setConfigurationSetting("content_css", uri);
}

const std::string WTextEdit::styleSheet() const
{
  return asString(configurationSetting("content_css")).toUTF8();
}

void WTextEdit::setExtraPlugins(const std::string& plugins)
{
  setConfigurationSetting("plugins", plugins);
}

const std::string WTextEdit::extraPlugins() const
{
  return asString(configurationSetting("plugins")).toUTF8();
}

void WTextEdit::setToolBar(int i, const std::string& config)
{
  std::string setting;
  if (version_ < 4)
    setting = "theme_advanced_buttons";
  else
    setting = "toolbar";

  setConfigurationSetting(setting + std::to_string(i + 1), config);
}

const std::string WTextEdit::toolBar(int i) const
{
  std::string setting;
  if (version_ < 4)
    setting = "theme_advanced_buttons";
  else
    setting = "toolbar";

  return
    asString(configurationSetting(setting + std::to_string(i + 1))).toUTF8();
}

std::string WTextEdit::renderRemoveJs(bool recursive)
{
  if (isRendered()) {
    std::string result = jsRef() + ".ed.remove();";
    if (!recursive)
      result += WT_CLASS ".remove('" + id() + "');";
    return result;
  } else
    return WTextArea::renderRemoveJs(recursive);
}

int WTextEdit::getTinyMCEVersion()
{
  std::string version = "6";
  WApplication::readConfigurationProperty("tinyMCEVersion", version);
  return Utils::stoi(version);
}

#ifndef WT_TARGET_JAVA
bool WTextEdit::verifyTinyMCEVersion(int version)
{
  std::string path = getTinyMCEPath();
  std::ifstream s(path.c_str(), std::ios::in | std::ios::binary);
  if (!s) {
    return false;
  }

  std::regex correctLineRegex("^( \\* .*[Vv]ersion:?|\\/\\/) \\d*\\.\\d*\\.\\d* ");
  std::regex versionRegex("\\d*\\.\\d*\\.\\d*");

  std::string line;
  while (std::getline(s, line)) {
    if (std::regex_search(line, correctLineRegex)) {
      std::smatch match;
      std::regex_search(line, match, versionRegex);

      std::string strVesion = match.str();
      std::regex_search(strVesion, match, std::regex("\\d*"));
      int expectedVersion = Utils::stoi(match.str());
      return expectedVersion == version;
    }
  }
  return version == 3;
}
#endif //WT_TARGET_JAVA

std::string WTextEdit::getTinyMCEPath()
{
  std::string tinyMCEURL;
  WApplication::readConfigurationProperty("tinyMCEURL", tinyMCEURL);
  if (tinyMCEURL.empty()) {
    int version = getTinyMCEVersion();

    std::string folder;
    std::string jsFile;
    if (version < 3) {
      folder = "tinymce/";
      jsFile = "tinymce.js";
    } else if (version == 3) {
      folder = "tiny_mce/";
      jsFile = "tiny_mce.js";

#ifdef WT_TARGET_JAVA
    } else if (version == 6) {
      folder = "tinymce6/";
      jsFile = "tinymce.min.js";
#endif // WT_TARGET_JAVA

    } else {
      folder = "tinymce/";
      jsFile = "tinymce.min.js";
    }

    std::string tinyMCEBaseURL = WApplication::relativeResourcesUrl() + folder;
    WApplication::readConfigurationProperty("tinyMCEBaseURL", tinyMCEBaseURL);

    if (!tinyMCEBaseURL.empty()
        && tinyMCEBaseURL[tinyMCEBaseURL.length()-1] != '/') {
      tinyMCEBaseURL += '/';
    }
    tinyMCEURL = tinyMCEBaseURL + jsFile;
  }
  return tinyMCEURL;
}

void WTextEdit::initTinyMCE()
{
  const char *THIS_JS = "js/WTextEdit.js";

  WApplication *app = WApplication::instance();

  if (!app->javaScriptLoaded(THIS_JS)) {
    if (app->environment().ajax())
      app->doJavaScript("window.tinyMCE_GZ = { loaded: true };", false);

    std::string tinyMCEURL = getTinyMCEPath();

    app->require(tinyMCEURL, "window['tinyMCE']");
    app->styleSheet().addRule(".mceEditor",
                              "display: block; position: absolute;");

    LOAD_JAVASCRIPT(app, THIS_JS, "WTextEdit", wtjs1);
  }
}

void WTextEdit::setReadOnly(bool readOnly)
{
  if (initialised_ && version_ < 5) {
    LOG_WARN("Using setReadOnly after the WTextEdit initialisation does not work with TinyMCE version lower than 5");
  }

  WTextArea::setReadOnly(readOnly);

  if (readOnly) {
    setConfigurationSetting("readonly", true);
  } else {
    setConfigurationSetting("readonly", cpp17::any());
  }
  flags_.set(BIT_READONLY_CHANGED);
}

void WTextEdit::propagateSetEnabled(bool enabled)
{
  WTextArea::propagateSetEnabled(enabled);

  setReadOnly(!enabled);
}

void WTextEdit::setPlaceholderText(WT_MAYBE_UNUSED const WString& placeholder)
{
  if (version_ < 5){
    throw WException("WTextEdit::setPlaceholderText() is not implemented for TinyMCE version lower than 5.");
  }

  WTextArea::setPlaceholderText(placeholder);
}

void WTextEdit::resize(const WLength& width, const WLength& height)
{
  WTextArea::resize(width, height);
}

void WTextEdit::setText(const WT_USTRING& text)
{
  WTextArea::setText(text);
  flags_.set(BIT_CONTENT_CHANGED);
}

std::string WTextEdit::plugins() const
{
  std::string plugins = extraPlugins();

  if (version_ == 3) {
    if (!plugins.empty())
      plugins += ",";
    plugins += "safari";
  }

  return plugins;
}

void WTextEdit::updateDom(DomElement& element, bool all)
{
  WTextArea::updateDom(element, all);

  if (element.type() == DomElementType::TEXTAREA)
    element.removeProperty(Property::StyleDisplay);

  // we are creating the actual element
  if (all && element.type() == DomElementType::TEXTAREA) {
    initialised_ = true;
    std::stringstream config;
    config << "{";

    bool first = true;

    for (SettingsMapType::const_iterator it = configurationSettings_.begin();
         it != configurationSettings_.end(); ++it) {
      if (it->first == "plugins")
        continue;

      if (!first)
        config << ',';

      first = false;

      config << it->first << ": "
             << Impl::asJSLiteral(it->second, TextFormat::UnsafeXHTML);
    }

    if (!first)
      config << ',';

    config << "plugins: '" << plugins() << "'";

    config <<
      ",init_instance_callback: obj.init"
      "}";

    DomElement dummy(DomElement::Mode::Update, DomElementType::TABLE);
    updateDom(dummy, true);

    element.callJavaScript("(function() { "
                           """var obj = " + jsRef() + ".wtObj;"
                           """obj.render(" + config.str() + ","
                           + jsStringLiteral(dummy.cssStyle()) + ","
                           + (changed().isConnected() ? "true" : "false")
                           + ");"
                           "})();");

    flags_.reset(BIT_CONTENT_CHANGED);
    flags_.reset(BIT_READONLY_CHANGED);
  }
  if (!all) {
    if (version_ > 4 && flags_.test(BIT_READONLY_CHANGED)) {
      if (isReadOnly()) {
        doJavaScript(jsRef() + ".ed.mode.set('readonly');");
      } else {
        doJavaScript(jsRef() + ".ed.mode.set('design');");
      }
      flags_.reset(BIT_READONLY_CHANGED);
    }

    if (flags_.test(BIT_CONTENT_CHANGED)) {
      element.callJavaScript(jsRef() + ".ed.load();");
      flags_.reset(BIT_CONTENT_CHANGED);
    }
  }
}

void WTextEdit::getDomChanges(std::vector<DomElement *>& result,
                              WApplication *app)
{
  /*
   * We apply changes directly to the table element, except of the textarea
   * contents. Therefore we first update the TABLE element, then collect
   * the contentChange to the TEXTAREA element, but reverse the order in
   * which they get applied since the load() statement expects the contents
   * to be set in the textarea first.
   */

  /*
   * Problem! ed.render() returns before the element is actually rendered,
   * and therefore, the _tbl element may not yet be available.
   *
   * This causes fail when a text edit is progressively enhanced. The solution
   * is to listen for the onInit() event -> we should be able to add a
   * wrapping ... .onInit(function(ed) { .... }) around the changes
   *
   * New version of TinyMCE uses divs instead of table and removing the _tbl
   * makes it work on all version
   */
  DomElement *e = DomElement::getForUpdate(formName()/* + "_tbl" */ ,
                                           DomElementType::TABLE);
  updateDom(*e, false);

  WTextArea::getDomChanges(result, app);

  result.push_back(e);
}

bool WTextEdit::domCanBeSaved() const
{
  return false;
}

int WTextEdit::boxPadding(WT_MAYBE_UNUSED Orientation orientation) const
{
  return 0;
}

int WTextEdit::boxBorder(WT_MAYBE_UNUSED Orientation orientation) const
{
  return 0;
}

void WTextEdit::setConfigurationSetting(const std::string& name,
                                        const cpp17::any& value)
{
  if (cpp17::any_has_value(value))
    configurationSettings_[name] = value;
  else
    configurationSettings_.erase(name);
}

cpp17::any WTextEdit::configurationSetting(const std::string& name) const
{
  SettingsMapType::const_iterator it = configurationSettings_.find(name);

  if (it != configurationSettings_.end())
    return it->second;
  else
    return cpp17::any();
}

}
