// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "filechooserqmlcallback.h"

FileChooserQmlCallback::FileChooserQmlCallback(QObject *parent)
    : QObject(parent),
      m_selectMultiple(true),
      m_selectExisting(true),
      m_nameFilters(QStringList() << QStringLiteral("*")),
      m_mimeTypeFilters(),
      m_selectFolder(false)
{
}

QString FileChooserQmlCallback::title() const
{
    return m_title;
}

void FileChooserQmlCallback::setTitle(const QString &title)
{
    m_title = title;
    emit titleChanged();
}

bool FileChooserQmlCallback::selectMultiple() const
{
    return m_selectMultiple;
}

void FileChooserQmlCallback::setSelectMultiple(bool selectMultiple)
{
    m_selectMultiple = selectMultiple;
    emit selectMultipleChanged();
}

bool FileChooserQmlCallback::selectExisting() const
{
    return m_selectExisting;
}

void FileChooserQmlCallback::setSelectExisting(bool selectExisting)
{
    m_selectExisting = selectExisting;
    emit selectExistingChanged();
}

QStringList FileChooserQmlCallback::nameFilters() const
{
    return m_nameFilters;
}

void FileChooserQmlCallback::setNameFilters(const QStringList &nameFilters)
{
    m_nameFilters = nameFilters;
    emit nameFiltersChanged();
}

QStringList FileChooserQmlCallback::mimeTypeFilters() const
{
    return m_mimeTypeFilters;
}

void FileChooserQmlCallback::setMimeTypeFilters(const QStringList &mimeTypeFilters)
{
    m_mimeTypeFilters = mimeTypeFilters;
    emit mimeTypeFiltersChanged();
}

QUrl FileChooserQmlCallback::folder() const
{
    return m_folder;
}

void FileChooserQmlCallback::setFolder(const QUrl &folder)
{
    m_folder = folder;
    emit folderChanged();
}

QString FileChooserQmlCallback::currentFile() const
{
    return m_currentFile;
}

void FileChooserQmlCallback::setCurrentFile(const QString &currentFile)
{
    m_currentFile = currentFile;
    emit currentFileChanged();
}

QString FileChooserQmlCallback::acceptLabel() const
{
    return m_acceptLabel;
}

void FileChooserQmlCallback::setAcceptLabel(const QString &acceptLabel)
{
    m_acceptLabel = acceptLabel;
    emit acceptLabelChanged();
}

bool FileChooserQmlCallback::selectFolder() const
{
    return m_selectFolder;
}

void FileChooserQmlCallback::setSelectFolder(bool selectFolder)
{
    m_selectFolder = selectFolder;
    emit selectFolderChanged();
}
