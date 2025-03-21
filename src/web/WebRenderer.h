// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WEBRENDERER_H_
#define WEBRENDERER_H_

#include <string>
#include <vector>
#include <set>

#include "Wt/WDateTime.h"
#include "Wt/WEnvironment.h"
#include "Wt/WStatelessSlot.h"
#include "Wt/Http/Cookie.h"

namespace Wt {

class WebRequest;
class WebResponse;
class WebStream;
class DomElement;
class EscapeOStream;
class FileServe;
class MetaHeader;

class WApplication;
class WWidget;
class WWebWidget;
class WObject;
class WResource;
class WStatelessSlot;
class WWidget;

class WT_API WebRenderer : public Wt::SlotLearnerInterface
{
public:
  typedef std::map<std::string, WObject *> FormObjectsMap;

  WebRenderer(WebSession& session);

  void setTwoPhaseThreshold(int bytes);

  bool visibleOnly() const { return visibleOnly_; }
  void setVisibleOnly(bool how) { visibleOnly_ = how; }
  bool isRendered() const { return rendered_; }
  void setRendered(bool how);

  void needUpdate(WWidget *w, bool laterOnly);
  void doneUpdate(WWidget *w);
  void updateFormObjects(WWebWidget *w, bool checkDescendants);

  void updateFormObjectsList(WApplication *app);
  const FormObjectsMap& formObjects() const;

  void saveChanges();
  void discardChanges();
  void letReloadJS(WebResponse& request, bool newSession,
                   bool embedded = false);
  void letReloadHTML(WebResponse& request, bool newSession);

  bool isDirty() const;
  unsigned int scriptId() const { return scriptId_; }
  int pageId() const { return pageId_; }

  void serveResponse(WebResponse& request);
  void serveError(int status, WebResponse& request,
                  const std::string& message);
  void serveLinkedCss(WebResponse& request);

  void setCookie(const Http::Cookie& cookie);
  void removeCookie(const Http::Cookie& cookie);

  bool preLearning() const { return learning_; }
  void learningIncomplete();

  void updateLayout() { updateLayout_ = true; }

  enum AckState {
    CorrectAck,
    ReasonableAck,
    BadAck
  };

  AckState ackUpdate(unsigned int updateId);

  void streamRedirectJS(WStringStream& out, const std::string& redirect);

  bool checkResponsePuzzle(const WebRequest& request);

  bool jsSynced() const;

  void setJSSynced(bool invisibleToo);

  void setStatelessSlotNotStateless() { currentStatelessSlotIsActuallyStateless_ = false; }

#ifndef WT_TARGET_JAVA
  static std::string renderCookieHttpHeader(const Http::Cookie& cookie, WebSession& session);
#endif

private:
  WebSession& session_;

  bool visibleOnly_, rendered_, initialStyleRendered_;
  int twoPhaseThreshold_, pageId_, ackErrs_;
  unsigned int expectedAckId_, scriptId_;
  int linkedCssCount_;
  std::string solution_;

  bool currentStatelessSlotIsActuallyStateless_;

  std::vector<Http::Cookie> cookiesToSet_;

  FormObjectsMap currentFormObjects_;
  std::string currentFormObjectsList_;
  bool formObjectsChanged_;
  bool updateLayout_;

  std::vector<int> wsRequestsToHandle_;
  bool cookieUpdateNeeded_;

  void setHeaders(WebResponse& request, const std::string mimeType);
  void addNoCacheHeaders(WebResponse& response);

  void serveJavaScriptUpdate(WebResponse& response);
  void serveMainscript(WebResponse& response);
  void serveBootstrap(WebResponse& request);
  void serveMainpage(WebResponse& response);
  void serveMainAjax(WStringStream& out);
  void serveWidgetSet(WebResponse& request);
  void collectJavaScript();

  void collectChanges(std::vector<DomElement *>& changes);

  void collectJavaScriptUpdate(WStringStream& out);
  void loadStyleSheet(WStringStream& out, WApplication *app,
                      const WLinkedCssStyleSheet& sheet);
  void loadStyleSheets(WStringStream& out, WApplication *app);
  void removeStyleSheets(WStringStream& out, WApplication *app);
  int loadScriptLibraries(WStringStream& out, WApplication *app,
                          int count = -1);
  void updateLoadIndicator(WStringStream& out, WApplication *app, bool all);
  void renderSetServerPush(WStringStream& out);
  void renderStyleSheet(WStringStream& out, const WLinkedCssStyleSheet& sheet,
                        WApplication *app);

  std::string createFormObjectsList(WApplication *app);

  void preLearnStateless(WApplication *app, WStringStream& out);
  WStringStream collectedJS1_, collectedJS2_, invisibleJS_, statelessJS_,
    beforeLoadJS_;
  void collectJS(WStringStream *js);

  void setPageVars(FileServe& page, const std::string& nonce);
  void streamBootContent(WebResponse& response,
                         FileServe& boot, bool hybrid);
  void addResponseAckPuzzle(WStringStream& out);
  void addContainerWidgets(WWebWidget *w,
                           std::vector<WContainerWidget *>& v);

  std::string headDeclarations() const;
  std::string bodyClassRtl() const;
  std::string sessionUrl() const;

  typedef std::set<WWidget *> UpdateMap;
  UpdateMap updateMap_;
  bool learning_, learningIncomplete_, moreUpdates_;

  std::string safeJsStringLiteral(const std::string& value);

  void addWsRequestId(int wsRqId);
  void renderWsRequestsDone(WStringStream &out);

  void updateMultiSessionCookie(const WebRequest &request);
  void renderCookieUpdate(WStringStream &out);

  /*
   * See how large the invisible changes are, perhaps we can
   * send them along in the first request.
   */
  void preCollectInvisibleChanges();

public:
  virtual std::string learn(WStatelessSlot* slot) final override;

  friend class WApplication;
  friend class WebSession;
};

}

#endif // WEBRENDERER_H_
