/*
    SPDX-FileCopyrightText: 2009 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openwithplugin.h"

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMimeDatabase>
#include <QMimeType>
#include <QVariantList>

#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMessageBox_KDevCompat>
#include <KApplicationTrader>
#include <KParts/MainWindow>
#include <KParts/PartLoader>
#include <KPluginFactory>
#include <KOpenWithDialog>
#include <KIO/ApplicationLauncherJob>
#include <kio_version.h>
#if KIO_VERSION < QT_VERSION_CHECK(5, 98, 0)
#include <KIO/JobUiDelegate>
#else
#include <KIO/JobUiDelegateFactory>
#endif

#include <interfaces/contextmenuextension.h>
#include <interfaces/context.h>
#include <project/projectmodel.h>
#include <util/path.h>

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/idocumentcontroller.h>

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(KDevOpenWithFactory, "kdevopenwith.json", registerPlugin<OpenWithPlugin>();)

namespace {
bool isTextPart(const QString& pluginId)
{
    return pluginId == QLatin1String("katepart");
}

bool isDirectory(const QString& mimeType)
{
    return mimeType == QLatin1String("inode/directory");
}

QString defaultIdForMimeType(const QString& mimeType)
{
    const auto config = KSharedConfig::openConfig()->group("Open With Defaults");
    return config.readEntry(mimeType, QString());
}

bool sortActions(QAction* left, QAction* right)
{
    return left->text() < right->text();
}

QList<QAction*> sortedActions(QList<QAction*> actions, QAction* standardAction)
{
    std::sort(actions.begin(), actions.end(), sortActions);
    if (standardAction) {
        actions.removeOne(standardAction);
        actions.prepend(standardAction);
    }
    return actions;
}

QAction* createAction(const QString& name, const QString& iconName, QWidget* parent, bool isDefault)
{
    auto* action = new QAction(QIcon::fromTheme(iconName), name, parent);
    if (isDefault) {
        QFont font = action->font();
        font.setBold(true);
        action->setFont(font);
    }
    return action;
}

bool canOpenDefault(const QString& mimeType)
{
    if (defaultIdForMimeType(mimeType).isEmpty() && isDirectory(mimeType)) {
        // potentially happens in non-kde environments apparently, see https://git.reviewboard.kde.org/r/122373
        return KApplicationTrader::preferredService(mimeType);
    } else {
        return true;
    }
}
}

OpenWithPlugin::OpenWithPlugin ( QObject* parent, const QVariantList& )
    : IPlugin ( QStringLiteral("kdevopenwith"), parent )
{
}

OpenWithPlugin::~OpenWithPlugin()
{
}

KDevelop::ContextMenuExtension OpenWithPlugin::contextMenuExtension(KDevelop::Context* context, QWidget* parent)
{
    // do not recurse
    if (context->hasType(KDevelop::Context::OpenWithContext)) {
        return ContextMenuExtension();
    }

    m_urls.clear();

    auto* filectx = dynamic_cast<FileContext*>( context );
    auto* projctx = dynamic_cast<ProjectItemContext*>( context );
    if ( filectx && filectx->urls().count() > 0 ) {
        m_urls = filectx->urls();
    } else if ( projctx && projctx->items().count() > 0 ) {
        // For now, let's handle *either* files only *or* directories only
        const auto items = projctx->items();
        const int wantedType = items.at(0)->type();
        for (ProjectBaseItem* item : items) {
            if (wantedType == ProjectBaseItem::File && item->file()) {
                m_urls << item->file()->path().toUrl();
            } else if ((wantedType == ProjectBaseItem::Folder || wantedType == ProjectBaseItem::BuildFolder) && item->folder()) {
                m_urls << item->folder()->path().toUrl();
            }
        }
    }

    if (m_urls.isEmpty()) {
        return KDevelop::ContextMenuExtension();
    }

    // Ok, lets fetch the mimetype for the !!first!! url and the relevant services
    // TODO: Think about possible alternatives to using the mimetype of the first url.
    QMimeType mimetype = QMimeDatabase().mimeTypeForUrl(m_urls.first());
    m_mimeType = mimetype.name();

    auto partActions = actionsForParts(parent);
    auto appActions = actionsForApplications(parent);

    OpenWithContext subContext(m_urls, mimetype);
    const QList<ContextMenuExtension> extensions = ICore::self()->pluginController()->queryPluginsForContextMenuExtensions( &subContext, parent);
    for (const ContextMenuExtension& ext : extensions) {
        appActions += ext.actions(ContextMenuExtension::OpenExternalGroup);
        partActions += ext.actions(ContextMenuExtension::OpenEmbeddedGroup);
    }

    {
        auto other = new QAction(i18nc("@item:menu", "Other..."), parent);
        connect(other, &QAction::triggered, this, [this] {
            auto dialog = new KOpenWithDialog(m_urls, ICore::self()->uiController()->activeMainWindow());
            if (dialog->exec() == QDialog::Accepted && dialog->service()) {
                openApplication(dialog->service());
            }
        });
        appActions << other;
    }

    // Now setup a menu with actions for each part and app
    auto* menu = new QMenu(i18nc("@title:menu", "Open With"), parent);
    auto documentOpenIcon = QIcon::fromTheme( QStringLiteral("document-open") );
    menu->setIcon( documentOpenIcon );

    if (!partActions.isEmpty()) {
        menu->addSection(i18nc("@title:menu", "Embedded Editors"));
        menu->addActions( partActions );
    }
    if (!appActions.isEmpty()) {
        menu->addSection(i18nc("@title:menu", "External Applications"));
        menu->addActions( appActions );
    }

    KDevelop::ContextMenuExtension ext;

    if (canOpenDefault(m_mimeType)) {
        auto* openAction = new QAction(i18nc("@action:inmenu", "Open"), parent);
        openAction->setIcon( documentOpenIcon );
        connect( openAction, &QAction::triggered, this, &OpenWithPlugin::openDefault );
        ext.addAction( KDevelop::ContextMenuExtension::FileGroup, openAction );
    }

    ext.addAction(KDevelop::ContextMenuExtension::FileGroup, menu->menuAction());
    return ext;
}

QList<QAction*> OpenWithPlugin::actionsForParts(QWidget* parent)
{
    if (isDirectory(m_mimeType)) {
        // Don't return any parts for directories, KDevelop can only open files, not folders, thus
        // we wouldn't ever actually do anything with the parts shown to the user here.
        // Note that we can open a folder in an external application just fine though.
        return {};
    }

    const auto parts = KParts::PartLoader::partsForMimeType(m_mimeType);
    const auto preferredPluginId = parts.value(0).pluginId();
    const auto defaultId = defaultIdForMimeType(m_mimeType);

    QList<QAction*> actions;
    actions.reserve(parts.size());

    QAction* standardAction = nullptr;
    for (const auto& part : parts) {
        const auto pluginId = part.pluginId();
        const auto isTextEditor = isTextPart(pluginId);

        auto* action =
            createAction(isTextEditor ? i18nc("@item:inmenu", "Default Editor") : part.name(), part.iconName(), parent,
                         pluginId == defaultId || (defaultId.isEmpty() && isTextEditor));
        connect(action, &QAction::triggered, this, [this, action, pluginId]() {
            openPart(pluginId, action->text());
        });
        actions << action;

        if (isTextEditor || pluginId == preferredPluginId) {
            standardAction = action;
        }
    }

    return sortedActions(std::move(actions), standardAction);
}

QList<QAction*> OpenWithPlugin::actionsForApplications(QWidget* parent)
{
    const auto preferredService = KApplicationTrader::preferredService(m_mimeType);
    const auto services = KApplicationTrader::queryByMimeType(m_mimeType);
    const auto defaultId = defaultIdForMimeType(m_mimeType);

    QList<QAction*> actions;
    actions.reserve(services.size());

    QAction* standardAction = nullptr;
    for (const auto& service : services) {
        const auto storageId = service->storageId();

        auto* action = createAction(service->name(), service->icon(), parent, storageId == defaultId);
        connect(action, &QAction::triggered, this, [this, service]() {
            openApplication(service);
        });
        actions << action;

        if (preferredService && storageId == preferredService->storageId()) {
            standardAction = action;
        }
    }

    return sortedActions(std::move(actions), standardAction);
}

void OpenWithPlugin::openDefault()
{
    //  check preferred handler
    const auto defaultId = defaultIdForMimeType(m_mimeType);
    if (!defaultId.isEmpty()) {
        if (auto service = KService::serviceByStorageId(defaultId); service && service->isApplication())
            openApplication(service);
        else
            openPart(defaultId, {});
        return;
    }

    // default handlers
    if (isDirectory(m_mimeType)) {
        KService::Ptr service = KApplicationTrader::preferredService(m_mimeType);
        auto* job = new KIO::ApplicationLauncherJob(service);
        job->setUrls(m_urls);
#if KIO_VERSION < QT_VERSION_CHECK(5, 98, 0)
        job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled,
#else
        job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled,
#endif
                                                  ICore::self()->uiController()->activeMainWindow()));
        job->start();
    } else {
        for (const QUrl& u : qAsConst(m_urls)) {
            ICore::self()->documentController()->openDocument( u );
        }
    }
}

void OpenWithPlugin::openPart(const QString& pluginId, const QString& name)
{
    auto prefName = pluginId;
    if (isTextPart(pluginId)) {
        // If the user chose a KTE part, lets make sure we're creating a TextDocument instead of
        // a PartDocument by passing no preferredpart to the documentcontroller
        // TODO: Solve this rather inside DocumentController
        prefName.clear();
    }
    for (const QUrl& u : qAsConst(m_urls)) {
        ICore::self()->documentController()->openDocument(u, prefName);
    }

    if (!name.isEmpty()) {
        rememberDefaultChoice(pluginId, name);
    }
}

void OpenWithPlugin::openApplication(const KService::Ptr& service)
{
    Q_ASSERT(service->isApplication());

    auto* job = new KIO::ApplicationLauncherJob(service);
    job->setUrls(m_urls);
#if KIO_VERSION < QT_VERSION_CHECK(5, 98, 0)
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled,
#else
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled,
#endif
                                              ICore::self()->uiController()->activeMainWindow()));
    job->start();

    rememberDefaultChoice(service->storageId(), service->name());
}

void OpenWithPlugin::openFilesInternal( const QList<QUrl>& files )
{
    if (files.isEmpty()) {
        return;
    }

    m_urls = files;
    m_mimeType = QMimeDatabase().mimeTypeForUrl(m_urls.first()).name();
    openDefault();
}

void OpenWithPlugin::rememberDefaultChoice(const QString& defaultId, const QString& name)
{
    auto config = KSharedConfig::openConfig()->group("Open With Defaults");
    if (defaultId == config.readEntry(m_mimeType, QString())) {
        return;
    }

    const auto setDefault = KMessageBox::questionTwoActions(
        qApp->activeWindow(),
        i18nc("%1: mime type name, %2: app/part name", "Do you want to open all '%1' files by default with %2?",
              m_mimeType, name),
        i18nc("@title:window", "Set as Default?"),
        KGuiItem(i18nc("@action:button", "Set as Default"), QStringLiteral("dialog-ok")),
        KGuiItem(i18nc("@action:button", "Do Not Set"), QStringLiteral("dialog-cancel")),
        QStringLiteral("OpenWith-%1").arg(m_mimeType));

    if (setDefault == KMessageBox::PrimaryAction) {
        config.writeEntry(m_mimeType, defaultId);
    }
}

#include "openwithplugin.moc"
#include "moc_openwithplugin.cpp"
