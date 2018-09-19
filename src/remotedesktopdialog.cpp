/*
 * Copyright © 2018 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Jan Grulich <jgrulich@redhat.com>
 */

#include "remotedesktopdialog.h"
#include "ui_remotedesktopdialog.h"

#include <QLoggingCategory>
#include <QStandardPaths>
#include <QSettings>
#include <QPushButton>

Q_LOGGING_CATEGORY(XdgDesktopPortalKdeRemoteDesktopDialog, "xdp-kde-remote-desktop-dialog")

RemoteDesktopDialog::RemoteDesktopDialog(const QString &appName, RemoteDesktopPortal::DeviceTypes deviceTypes, bool screenSharingEnabled,
                                         bool multiple, QDialog *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_dialog(new Ui::RemoteDesktopDialog)
{
    m_dialog->setupUi(this);

    m_dialog->screenCastWidget->setVisible(screenSharingEnabled);
    if (screenSharingEnabled) {
        connect(m_dialog->screenCastWidget, &QListWidget::itemDoubleClicked, this, &RemoteDesktopDialog::accept);

        if (multiple) {
            m_dialog->screenCastWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        }
    }

    // TODO

    connect(m_dialog->buttonBox, &QDialogButtonBox::accepted, this, &RemoteDesktopDialog::accept);
    connect(m_dialog->buttonBox, &QDialogButtonBox::rejected, this, &RemoteDesktopDialog::reject);

    m_dialog->buttonBox->button(QDialogButtonBox::Ok)->setText(i18n("Share"));

    QString applicationName;
    const QString desktopFile = appName + QLatin1String(".desktop");
    const QStringList desktopFilesLocations = QStandardPaths::locateAll(QStandardPaths::ApplicationsLocation, desktopFile, QStandardPaths::LocateFile);
    QSettings settings(desktopFile, QSettings::IniFormat);
    settings.beginGroup(QLatin1String("Desktop Entry"));
    if (settings.contains(QLatin1String("X-GNOME-FullName"))) {
        applicationName = settings.value(QLatin1String("X-GNOME-FullName")).toString();
    } else {
        applicationName = settings.value(QLatin1String("Name")).toString();
    }

    if (applicationName.isEmpty()) {
        setWindowTitle(i18n("Select what to share with the requesting application"));
    } else {
        setWindowTitle(i18n("Select what to share with %1").arg(applicationName));
    }
}

RemoteDesktopDialog::~RemoteDesktopDialog()
{
    delete m_dialog;
}

QList<quint32> RemoteDesktopDialog::selectedScreens() const
{
    return m_dialog->screenCastWidget->selectedScreens();
}

RemoteDesktopPortal::DeviceTypes RemoteDesktopDialog::deviceTypes() const
{
    return 0;
}