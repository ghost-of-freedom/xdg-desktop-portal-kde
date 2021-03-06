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

#ifndef XDG_DESKTOP_PORTAL_KDE_SCREENCHOOSER_DIALOG_H
#define XDG_DESKTOP_PORTAL_KDE_SCREENCHOOSER_DIALOG_H

#include <QDialog>
#include "screencast.h"

namespace Ui
{
class ScreenChooserDialog;
}
class QItemSelection;

class ScreenChooserDialog : public QDialog
{
    Q_OBJECT
public:
    ScreenChooserDialog(const QString &appName, bool multiple, QDialog *parent = nullptr, Qt::WindowFlags flags = {});
    ~ScreenChooserDialog();

    void setSourceTypes(ScreenCastPortal::SourceTypes types);

    QList<quint32> selectedScreens() const;
    QList<QByteArray> selectedWindows() const;

private:
    void selectionChanged(const QItemSelection &selected);

    const bool m_multiple;
    Ui::ScreenChooserDialog *m_dialog;
};

#endif // XDG_DESKTOP_PORTAL_KDE_SCREENCHOOSER_DIALOG_H
