/*
 * Copyright © 2017-2019 Red Hat, Inc
 * Copyright © 2020 Harald Sitter <sitter@kde.org>
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

#include "appchooserdialog.h"
#include "ui_appchooserdialog.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QQuickItem>

#include <QDir>
#include <QStandardPaths>
#include <QSettings>

#include <KApplicationTrader>
#include <KProcess>
#include <KService>
#include <kdeclarative/kdeclarative.h>

AppChooserDialog::AppChooserDialog(const QStringList &choices, const QString &defaultApp, const QString &fileName, const QString &mimeName, QDialog *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_dialog(new Ui::AppChooserDialog)
    , m_defaultChoices(choices)
    , m_defaultApp(defaultApp)
    , m_mimeName(mimeName)
{
    m_dialog->setupUi(this);

    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_dialog->quickWidget->engine());
    kdeclarative.setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
    kdeclarative.setupEngine(m_dialog->quickWidget->engine());
    kdeclarative.setupContext();

    m_model = new AppModel(this);
    m_model->setPreferredApps(choices);

    AppFilterModel *filterModel = new AppFilterModel(this);
    filterModel->setSourceModel(m_model);

    auto *data = new AppChooserData(this);
    data->setFileName(fileName);
    data->setDefaultApp(defaultApp);

    qmlRegisterSingletonInstance<AppFilterModel>("org.kde.xdgdesktopportal", 1, 0, "AppModel", filterModel);
    qmlRegisterSingletonInstance<AppChooserData>("org.kde.xdgdesktopportal", 1, 0, "AppChooserData", data);
    m_dialog->quickWidget->setClearColor(Qt::transparent);
    m_dialog->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_dialog->quickWidget->setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("xdg-desktop-portal-kde/qml/AppChooserDialog.qml"))));

    connect(data, &AppChooserData::openDiscover, this, &AppChooserDialog::onOpenDiscover);
    connect(data, &AppChooserData::applicationSelected, this, &AppChooserDialog::onApplicationSelected);

    setWindowTitle(i18n("Open with..."));
}

AppChooserDialog::~AppChooserDialog()
{
    delete m_dialog;
}

QString AppChooserDialog::selectedApplication() const
{
    return m_selectedApplication;
}

void AppChooserDialog::onApplicationSelected(const QString& desktopFile)
{
    m_selectedApplication = desktopFile;
    QDialog::accept();
}

void AppChooserDialog::onOpenDiscover()
{
    QStringList args;
    if (!m_mimeName.isEmpty()) {
        args << QStringLiteral("--mime") << m_mimeName;
    }
    KProcess::startDetached(QStringLiteral("plasma-discover"), args);
}

void AppChooserDialog::updateChoices(const QStringList &choices)
{
    m_model->setPreferredApps(choices);
}

ApplicationItem::ApplicationItem(const QString &name, const QString &icon, const QString &desktopFileName)
    : m_applicationName(name)
    , m_applicationIcon(icon)
    , m_applicationDesktopFile(desktopFileName)
    , m_applicationCategory(AllApplications)
{
}

QString ApplicationItem::applicationName() const
{
    return m_applicationName;
}

QString ApplicationItem::applicationIcon() const
{
    return m_applicationIcon;
}

QString ApplicationItem::applicationDesktopFile() const
{
    return m_applicationDesktopFile;
}

void ApplicationItem::setApplicationCategory(ApplicationItem::ApplicationCategory category)
{
    m_applicationCategory = category;
}

ApplicationItem::ApplicationCategory ApplicationItem::applicationCategory() const
{
    return m_applicationCategory;
}

bool ApplicationItem::operator==(const ApplicationItem &item) const
{
    return item.applicationDesktopFile() == applicationDesktopFile();
}

AppFilterModel::AppFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::DescendingOrder);
}

AppFilterModel::~AppFilterModel()
{
}

void AppFilterModel::setShowOnlyPrefferedApps(bool show)
{
    m_showOnlyPreferredApps = show;

    invalidate();
}

bool AppFilterModel::showOnlyPreferredApps() const
{
    return m_showOnlyPreferredApps;
}

void AppFilterModel::setFilter(const QString &text)
{
    m_filter = text;

    invalidate();
}

QString AppFilterModel::filter() const
{
    return m_filter;
}

bool AppFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    ApplicationItem::ApplicationCategory category = static_cast<ApplicationItem::ApplicationCategory>(sourceModel()->data(index, AppModel::ApplicationCategoryRole).toInt());
    QString appName = sourceModel()->data(index, AppModel::ApplicationNameRole).toString();

    if (m_showOnlyPreferredApps)
        return category == ApplicationItem::PreferredApplication;

    if (category == ApplicationItem::PreferredApplication)
        return true;

    if (m_filter.isEmpty())
        return true;

    return appName.toLower().contains(m_filter);
}

bool AppFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    ApplicationItem::ApplicationCategory leftCategory = static_cast<ApplicationItem::ApplicationCategory>(sourceModel()->data(left, AppModel::ApplicationCategoryRole).toInt());
    ApplicationItem::ApplicationCategory rightCategory = static_cast<ApplicationItem::ApplicationCategory>(sourceModel()->data(right, AppModel::ApplicationCategoryRole).toInt());
    QString leftName = sourceModel()->data(left, AppModel::ApplicationNameRole).toString();
    QString rightName = sourceModel()->data(right, AppModel::ApplicationNameRole).toString();

    if (leftCategory < rightCategory) {
        return false;
    } else if (leftCategory > rightCategory) {
        return true;
    }

    return QString::localeAwareCompare(leftName, rightName) > 0;
}

QString AppChooserData::defaultApp() const
{
    return m_defaultApp;
}

void AppChooserData::setDefaultApp(const QString &defaultApp)
{
    m_defaultApp = defaultApp;
    Q_EMIT defaultAppChanged();
}

AppChooserData::AppChooserData(QObject *parent)
    : QObject(parent)
{
}

QString AppChooserData::fileName() const
{
    return m_fileName;
}

void AppChooserData::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    Q_EMIT fileNameChanged();
}

AppModel::AppModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadApplications();
}

AppModel::~AppModel()
{
}

void AppModel::setPreferredApps(const QStringList &list)
{
    for (ApplicationItem &item : m_list) {
        bool changed = false;

        // First reset to initial type
        if (item.applicationCategory() != ApplicationItem::AllApplications) {
            item.setApplicationCategory(ApplicationItem::AllApplications);
            changed = true;
        }

        if (list.contains(item.applicationDesktopFile())) {
            item.setApplicationCategory(ApplicationItem::PreferredApplication);
            changed = true;
        }

        if (changed) {
            const int row = m_list.indexOf(item);
            if (row >= 0) {
                QModelIndex index = createIndex(row, 0, AppModel::ApplicationCategoryRole);
                Q_EMIT dataChanged(index, index);
            }
        }
    }
}

QVariant AppModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_list.count()) {
        ApplicationItem item = m_list.at(row);

        switch (role) {
            case ApplicationNameRole:
                return item.applicationName();
            case ApplicationIconRole:
                return item.applicationIcon();
            case ApplicationDesktopFileRole:
                return item.applicationDesktopFile();
            case ApplicationCategoryRole:
                return static_cast<int>(item.applicationCategory());
            default:
                break;
        }
    }

    return QVariant();
}

int AppModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_list.count();
}

QHash<int, QByteArray> AppModel::roleNames() const
{
    return {
        {ApplicationNameRole, QByteArrayLiteral("applicationName")},
        {ApplicationIconRole, QByteArrayLiteral("applicationIcon")},
        {ApplicationDesktopFileRole, QByteArrayLiteral("applicationDesktopFile")},
        {ApplicationCategoryRole, QByteArrayLiteral("applicationCategory")}
    };
}

void AppModel::loadApplications()
{
    const KService::List appServices = KApplicationTrader::query([](const KService::Ptr &service) -> bool {
         return service->isValid() && !service->noDisplay() /* includes platform and desktop considerations */;
    });
    for (const KService::Ptr &service : appServices) {
        const QString fullName = service->property(QStringLiteral("X-GNOME-FullName"), QVariant::String).toString();
        const QString name = fullName.isEmpty() ? service->name() : fullName;
        ApplicationItem appItem(name, service->icon(), service->desktopEntryName());
        if (!m_list.contains(appItem)) {
            m_list.append(appItem);
        }
    }
}
