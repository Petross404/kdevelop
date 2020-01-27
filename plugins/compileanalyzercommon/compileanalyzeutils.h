/*
 * This file is part of KDevelop
 *
 * Copyright 2018,2020 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef COMPILEANALYZER_COMPILEANALYZEUTILS_H
#define COMPILEANALYZER_COMPILEANALYZEUTILS_H

// lib
#include <compileanalyzercommonexport.h>

class QUrl;
class QString;
class QStringList;

namespace KDevelop
{
class Path;

namespace Utils
{

KDEVCOMPILEANALYZERCOMMON_EXPORT
QString findExecutable(const QString& fallbackExecutablePath);

KDEVCOMPILEANALYZERCOMMON_EXPORT
QStringList filesFromCompilationDatabase(const KDevelop::Path& buildPath,
                                         const QUrl& urlToCheck, bool allFiles,
                                         QString& error);

}

}

#endif