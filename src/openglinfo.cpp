/*
  Copyright (C) 2016 Rohan Garg <rohan@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "openglinfo.h"

#include <QDebug>
#include <kwinglplatform.h>

#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <QX11Info>

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

    if (context.isOpenGLES() || !QX11Info::isPlatformX11()) {
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

    displayServerVersion = KWin::GLPlatform::versionToString(platform->serverVersion());
  }
  else {
      qCritical() <<"Error: makeCurrent() failed\n";
  }

  context.doneCurrent();
}
