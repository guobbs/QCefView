﻿#include <QCefView.h>

#pragma region qt_headers
#include <QVBoxLayout>
#include <QWindow>
#include <QResizeEvent>
#include <QPoint>
#pragma endregion qt_headers

#include "details/QCefEventPrivate.h"
#include "details/QCefViewPrivate.h"

QCefView::QCefView(const QString url, QWidget* parent /*= 0*/)
  : QWidget(parent)
{
  // initialize the layout
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  // create the window
  QCefWindow* pCefWindow = new QCefWindow(windowHandle(), this);
  pCefWindow->create();

  // create window container
  QWidget* windowContainer = createWindowContainer(pCefWindow, this);
  layout->addWidget(windowContainer);

  // create the implementation
  d_ptr.reset(new QCefViewPrivate(url, this, pCefWindow));

  // If we're already part of a window, we'll install our event handler
  // If our parent changes later, this will be handled in QCefView::changeEvent()
  if (this->window())
    this->window()->installEventFilter(this);
}

QCefView::QCefView(QWidget* parent /*= 0*/)
  : QCefView("about:blank", parent)
{}

QCefView::~QCefView()
{
  disconnect();
}

void
QCefView::addLocalFolderResource(const QString& path, const QString& url, int priority /* = 0*/)
{
  QCefViewPrivate::folderMappingList_.push_back({ path, url, priority });
}

void
QCefView::addArchiveResource(const QString& path, const QString& url, const QString& password /* = ""*/)
{
  QCefViewPrivate::archiveMappingList_.push_back({ path, url, password });
}

void
QCefView::addCookie(const QString& name, const QString& value, const QString& domain, const QString& url)
{
  Q_D(QCefView);

  d->addCookie(name, value, domain, url);
}

void
QCefView::navigateToString(const QString& content)
{
  Q_D(QCefView);

  d->navigateToString(content);
}

void
QCefView::navigateToUrl(const QString& url)
{
  Q_D(QCefView);

  d->navigateToUrl(url);
}

bool
QCefView::browserCanGoBack()
{
  Q_D(QCefView);

  return d->browserCanGoBack();
}

bool
QCefView::browserCanGoForward()
{
  Q_D(QCefView);

  return d->browserCanGoForward();
}

void
QCefView::browserGoBack()
{
  Q_D(QCefView);

  d->browserGoBack();
}

void
QCefView::browserGoForward()
{
  Q_D(QCefView);

  d->browserGoForward();
}

bool
QCefView::browserIsLoading()
{
  Q_D(QCefView);

  return d->browserIsLoading();
}

void
QCefView::browserReload()
{
  Q_D(QCefView);

  d->browserReload();
}

void
QCefView::browserStopLoad()
{
  Q_D(QCefView);

  d->browserStopLoad();
}

bool
QCefView::triggerEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, CefViewBrowserHandler::MAIN_FRAME);
}

bool
QCefView::triggerEvent(const QCefEvent& event, int frameId)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, frameId);
}

bool
QCefView::broadcastEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, CefViewBrowserHandler::ALL_FRAMES);
}

bool
QCefView::responseQCefQuery(const QCefQuery& query)
{
  Q_D(QCefView);

  return d->responseQCefQuery(query);
}

void
QCefView::setContextMenuHandler(CefContextMenuHandler* handler)
{
  Q_D(QCefView);

  return d->setContextMenuHandler(handler);
}

void
QCefView::setDialogHandler(CefDialogHandler* handler)
{
  Q_D(QCefView);

  return d->setDialogHandler(handler);
}

void
QCefView::setDisplayHandler(CefDisplayHandler* handler)
{
  Q_D(QCefView);

  return d->setDisplayHandler(handler);
}

void
QCefView::setDownloadHandler(CefDownloadHandler* handler)
{
  Q_D(QCefView);

  return d->setDownloadHandler(handler);
}

void
QCefView::setJSDialogHandler(CefJSDialogHandler* handler)
{
  Q_D(QCefView);

  return d->setJSDialogHandler(handler);
}

void
QCefView::setKeyboardHandler(CefKeyboardHandler* handler)
{
  Q_D(QCefView);

  return d->setKeyboardHandler(handler);
}

void
QCefView::onLoadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward)
{}

void
QCefView::onLoadStart()
{}

void
QCefView::onLoadEnd(int httpStatusCode)
{}

void
QCefView::onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool& handled)
{}

void
QCefView::onDraggableRegionChanged(const QRegion& draggableRegion, const QRegion& nonDraggableRegion)
{}

void
QCefView::onConsoleMessage(const QString& message, int level)
{}

void
QCefView::onTakeFocus(bool next)
{}

void
QCefView::onQCefUrlRequest(const QString& url)
{}

void
QCefView::onQCefQueryRequest(const QCefQuery& query)
{}

void
QCefView::onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments)
{}

void
QCefView::changeEvent(QEvent* event)
{
  if (QEvent::ParentAboutToChange == event->type()) {
    if (this->window())
      this->window()->removeEventFilter(this);
  } else if (QEvent::ParentChange == event->type()) {
    if (this->window())
      this->window()->installEventFilter(this);
  }
  QWidget::changeEvent(event);
}

bool
QCefView::eventFilter(QObject* watched, QEvent* event)
{
  Q_D(QCefView);

  if (watched == this->window()) {
    if (QEvent::Resize == event->type() || QEvent::Move == event->type()) {
      d->onToplevelWidgetMoveOrResize();
    }
  }

  if (watched == this) {
    if (QEvent::MouseButtonPress == event->type()) {
      QMouseEvent* e = (QMouseEvent*)event;
      if (e->button() == Qt::LeftButton) {
        d->bIsDragging_ = true;
        d->mLastMousePosition_ = e->pos();
      }
    }
  }

  return QWidget::eventFilter(watched, event);
}

void
QCefView::mousePressEvent(QMouseEvent* event)
{
  Q_D(QCefView);

  if (event->button() == Qt::LeftButton) {
    d->bIsDragging_ = true;
    d->mLastMousePosition_ = event->pos();
  }
  QWidget::mousePressEvent(event);
}

void
QCefView::mouseMoveEvent(QMouseEvent* event)
{
  Q_D(QCefView);

  if (event->buttons().testFlag(Qt::LeftButton) && d->bIsDragging_) {
    this->move(this->pos() + (event->pos() - d->mLastMousePosition_));
    d->mLastMousePosition_ = event->pos();
  }
  QWidget::mouseMoveEvent(event);
}

void
QCefView::mouseReleaseEvent(QMouseEvent* event)
{
  Q_D(QCefView);

  if (event->button() == Qt::LeftButton) {
    d->bIsDragging_ = false;
  }
  QWidget::mouseReleaseEvent(event);
}
