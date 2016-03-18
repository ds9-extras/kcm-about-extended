#include "openglinfo.h"

#include <QDebug>
#include <kwinglplatform.h>

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

#include <wayland-version.h>

OpenGLInfo::OpenGLInfo()
{
  QOpenGLContext context;
  QSurfaceFormat format;
  QOffscreenSurface surface;
  surface.create();
  format.setMajorVersion(3);
  format.setMinorVersion(2);
  context.setFormat(format);
  if (!context.create()) {
      qCritical() << "Could not create QOpenGLContext";
      return;
  }

  if (context.makeCurrent(&surface)) {
    KWin::GLPlatform *platform = KWin::GLPlatform::instance();
    platform->detect(KWin::GlxPlatformInterface);

    // When renderer string is empty, try falling back to the EGL Platform
    if (platform->glRendererString().isEmpty()) {
      platform->detect(KWin::EglPlatformInterface);
    }

    if (platform->glRendererString().isEmpty()) {
      qCritical() << "Neither GLX or EGL detection worked!";
    }

    openGLRenderer = QString::fromUtf8(platform->glRendererString());
    openGLVersion = QString::fromUtf8(platform->glVersionString());
    qDebug() << platform->glRendererString().isEmpty();
    if (platform->driver() == KWin::Driver::Driver_Unknown) {
      kwinDriver = (char *) glGetString(GL_VENDOR);
    } else {
      kwinDriver = KWin::GLPlatform::driverToString(platform->driver());
    }
    displayServerVersion = KWin::GLPlatform::versionToString(platform->serverVersion());
  }
  else {
      qCritical() <<"Error: makeCurrent() failed\n";
  }

  context.doneCurrent();

  // FIXME: Make this a runtime thing
  waylandVersion = WAYLAND_VERSION;
}
