// Copyright 2015 - 2025, GIBIS-UNIFESP and the wiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSettings>

class Settings
{
public:
    Settings() = delete;

    static QString fileName();
    static QVariant value(const QString &key);
    static bool contains(const QString &key);
    static void remove(const QString &key);
    static void setValue(const QString &key, const QVariant &value);

private:
    static QSettings *settingsInstance();
    static QSettings *settings;
};
