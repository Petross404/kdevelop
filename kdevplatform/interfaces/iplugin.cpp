/* This file is part of the KDE project
 Copyright 2002 Simon Hausmann <hausmann@kde.org>
 Copyright 2002 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 Copyright 2002 Harald Fernengel <harry@kdevelop.org>
 Copyright 2002 Falk Brettschneider <falkbr@kdevelop.org>
 Copyright 2003 Julian Rockey <linux@jrockey.com>
 Copyright 2003 Roberto Raggi <roberto@kdevelop.org>
 Copyright 2003 Jens Dagerbo <jens.dagerbo@swipnet.se>
 Copyright 2003 Mario Scalas <mario.scalas@libero.it>
 Copyright 2003-2004,2007 Alexander Dymo <adymo@kdevelop.org>
 Copyright 2006 Adam Treat <treat@kde.org>
 Copyright 2007 Andreas Pakulat <apaku@gmx.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*/

#include "iplugin.h"

#include <KActionCollection>
#include <KMainWindow>
#include <KXmlGuiWindow>
#include <KXMLGUIFactory>

#include "icore.h"
#include "iplugincontroller.h"
#include "iprojectcontroller.h"
#include "iproject.h"
#include "contextmenuextension.h"

namespace KDevelop
{

class IPluginPrivate
{
public:
    explicit IPluginPrivate(IPlugin *q)
        : q(q)
    {}

    ~IPluginPrivate()
    {
    }

    void guiClientAdded(KXMLGUIClient *client)
    {
        if (client != q)
            return;

        q->initializeGuiState();
        updateState();
    }

    void updateState()
    {
        const int projectCount =
            ICore::self()->projectController()->projectCount();

        IPlugin::ReverseStateChange reverse = IPlugin::StateNoReverse;
        if (! projectCount)
            reverse = IPlugin::StateReverse;

        q->stateChanged(QStringLiteral("has_project"), reverse);
    }

    IPlugin *q;
    ICore *core;
    QString m_errorDescription;
};

IPlugin::IPlugin( const QString &componentName, QObject *parent )
    : QObject(parent)
    , KXMLGUIClient()
    , d(new IPluginPrivate(this))
{
    // The following cast is safe, there's no component in KDevPlatform that
    // creates plugins except the plugincontroller. The controller passes
    // Core::self() as parent to KServiceTypeTrader::createInstanceFromQuery
    // so we know the parent is always a Core* pointer.
    // This is the only way to pass the Core pointer to the plugin during its
    // creation so plugins have access to ICore during their creation.
    Q_ASSERT(qobject_cast<KDevelop::ICore*>(parent));
    d->core = static_cast<KDevelop::ICore*>(parent);

    auto metaData = core()->pluginController()->infoForPluginId(componentName);
    setComponentName(componentName, metaData.name());

    auto clientAdded = [=] (KXMLGUIClient* client) {
        d->guiClientAdded(client);
    };
    foreach (KMainWindow* mw, KMainWindow::memberList()) {
        auto* guiWindow = qobject_cast<KXmlGuiWindow*>(mw);
        if (! guiWindow)
            continue;

        connect(guiWindow->guiFactory(), &KXMLGUIFactory::clientAdded,
                this, clientAdded);
    }

    auto updateState = [=] { d->updateState(); };
    connect(ICore::self()->projectController(), &IProjectController::projectOpened,
            this, updateState);
    connect(ICore::self()->projectController(), &IProjectController::projectClosed,
            this, updateState);
}

IPlugin::~IPlugin() = default;

void IPlugin::unload()
{
}

ICore *IPlugin::core() const
{
    return d->core;
}

}

KDevelop::ContextMenuExtension KDevelop::IPlugin::contextMenuExtension(KDevelop::Context* context, QWidget* parent)
{
    Q_UNUSED(context);
    Q_UNUSED(parent);
    return KDevelop::ContextMenuExtension();
}

void KDevelop::IPlugin::initializeGuiState()
{ }

class CustomXmlGUIClient : public KXMLGUIClient
{
public:
    explicit CustomXmlGUIClient(const KDevelop::IPlugin* plugin)
    {
        // TODO KF5: Get rid off this
        setComponentName(plugin->componentName(), plugin->actionCollection()->componentDisplayName());
    }
    using KXMLGUIClient::setXMLFile;
};

KXMLGUIClient* KDevelop::IPlugin::createGUIForMainWindow(Sublime::MainWindow* window)
{
    QScopedPointer<CustomXmlGUIClient> ret(new CustomXmlGUIClient(this));

    QString file;
    createActionsForMainWindow(window, file, *ret->actionCollection());
    if (ret->actionCollection()->isEmpty()) {
        return nullptr;
    }

    Q_ASSERT(!file.isEmpty()); //A file must have been set
    ret->setXMLFile(file);
    return ret.take();
}

void KDevelop::IPlugin::createActionsForMainWindow( Sublime::MainWindow* /*window*/, QString& /*xmlFile*/, KActionCollection& /*actions*/ )
{
}

bool KDevelop::IPlugin::hasError() const
{
    return !d->m_errorDescription.isEmpty();
}

void KDevelop::IPlugin::setErrorDescription(const QString& description)
{
    d->m_errorDescription = description;
}


QString KDevelop::IPlugin::errorDescription() const
{
    return d->m_errorDescription;
}

int KDevelop::IPlugin::configPages() const
{
    return 0;
}

KDevelop::ConfigPage* KDevelop::IPlugin::configPage (int, QWidget*)
{
    return nullptr;
}

int KDevelop::IPlugin::perProjectConfigPages() const
{
    return 0;
}

KDevelop::ConfigPage* KDevelop::IPlugin::perProjectConfigPage(int, const ProjectConfigOptions&, QWidget*)
{
    return nullptr;
}
#include "moc_iplugin.cpp"
