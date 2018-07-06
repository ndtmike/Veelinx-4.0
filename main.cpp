/****************************************************************************
**
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: main.cpp
**
** Rebarlinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2018
** Email: mike@ndtjames.com
**
** 6/16/18 Initial Creation
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QSplashScreen>
#include <QString>

#include "mainwindow.h"

/******************************************************************************

  Function: main()

  Description:
  ============
  invokes the qapplication
  loads image from resource file
  displays the splash screen and delays
  displays Mainwindow

******************************************************************************/

int main(int argc, char *argv[])
{
    QString programname ( "Veelinx" );//can not define string in project needs to go here
    QString version = QString( "Version %1.%2.%3" ).arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH);
    QString display = QString( "%1: %2" ).arg(programname).arg(version);

#ifdef R_DEBUG
    const int delay_seconds = 1;
#else
    const int delay_seconds = 3;
#endif
    const int pixel_size = 38;
//    const int app_width = 1200;
//    const int app_height = 800;


    QApplication a(argc, argv);

    QFont splashFont; //increase the font size for the splash screen
    splashFont.setFamily("Arial");
    splashFont.setBold(true);
    splashFont.setItalic(true);
    splashFont.setPixelSize(pixel_size);

    QPixmap pixmap(":/images/splash.png"); //loads the image from the resource file
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->setFont(splashFont);
    splash->show();
    splash->showMessage(display,Qt::AlignHCenter|Qt::AlignBottom,Qt::black);
    a.thread()->sleep(delay_seconds); // wait for just 1 second and then show main window
    a.processEvents();

    MainWindow w;

    if (!QGuiApplication::styleHints()->showIsFullScreen() && !QGuiApplication::styleHints()->showIsMaximized()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(&w);
        w.resize(availableGeometry.width() * 2 / 3, availableGeometry.height() * 2 / 3);
        w.move((availableGeometry.width() - w.width()) / 2,
                    (availableGeometry.height() - w.height()) / 2);
    }
    w.show();

    splash->finish( &w );
    return a.exec();
}
