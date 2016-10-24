#include "openglinfo.h"

#include <QDebug>
#include <kwinglplatform.h>

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

OpenGLInfo::OpenGLInfo()
{
  QOpenGLContext context;
  QOffscreenSurface surface;
  surface.create();
  if (!context.create()) {
      qCritical() << "Could not create QOpenGLContext";
      return;
  }

  if (context.makeCurrent(&surface)) {
    KWin::GLPlatform *platform = KWin::GLPlatform::instance();

    if (context.isOpenGLES()) {
      platform->detect(KWin::EglPlatformInterface);
    } else {
      platform->detect(KWin::GlxPlatformInterface);
    }

    if (platform->glRendererString().isEmpty()) {
      qCritical() << "Neither GLX or EGL detection worked!";
    }

    openGLRenderer = QString::fromUtf8(platform->glRendererString());
    openGLVersion = QString::fromUtf8(platform->glVersionString());
    mesaVersion = KWin::GLPlatform::versionToString(platform->mesaVersion());
    if (platform->driver() == KWin::Driver::Driver_Unknown) {
      kwinDriver = platform->glVendorString();
    } else {
      kwinDriver = KWin::GLPlatform::driverToString(platform->driver());
    }
    //FIXME: This will fail on Wayland systems
    displayServerVersion = KWin::GLPlatform::versionToString(getXServerVersion());
  }
  else {
      qCritical() <<"Error: makeCurrent() failed\n";
  }

  context.doneCurrent();
}

qint64 OpenGLInfo::getXServerVersion()
{
    qint64 major, minor, patch;
    major = 0;
    minor = 0;
    patch = 0;

    if (xcb_connection_t *c = QX11Info::connection()) {
        auto setup = xcb_get_setup(c);
        const QByteArray vendorName(xcb_setup_vendor(setup), xcb_setup_vendor_length(setup));
        if (vendorName.contains("X.Org")) {
            const int release = setup->release_number;
            major = (release / 10000000);
            minor = (release /   100000) % 100;
            patch = (release /     1000) % 100;
        }
    }

    return kVersionNumber(major, minor, patch);
}
