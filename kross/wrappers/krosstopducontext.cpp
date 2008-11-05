//This is file has been generated by xmltokross, you should not edit this file but the files used to generate it.

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <kross/core/manager.h>
#include <kross/core/wrapperinterface.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/parsingenvironment.h>
#include <language/interfaces/iproblem.h>
#include <language/editor/simplecursor.h>

class KrossKDevelopReferencedTopDUContext : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	public:
		KrossKDevelopReferencedTopDUContext(KDevelop::ReferencedTopDUContext* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::ReferencedTopDUContext"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE KDevelop::ReferencedTopDUContext operator=(const KDevelop::ReferencedTopDUContext& x0) { return wrapped->operator=(x0); }
		Q_SCRIPTABLE KDevelop::TopDUContext* data() const { return wrapped->data(); }
		Q_SCRIPTABLE bool operator==(const KDevelop::ReferencedTopDUContext& x0) const { return wrapped->operator==(x0); }
		Q_SCRIPTABLE bool operator!=(const KDevelop::ReferencedTopDUContext& x0) const { return wrapped->operator!=(x0); }
		Q_SCRIPTABLE KDevelop::TopDUContext* operator->() const { return wrapped->operator->(); }
		Q_SCRIPTABLE unsigned int hash() const { return wrapped->hash(); }
	private:
		KDevelop::ReferencedTopDUContext* wrapped;
};

class KrossKDevelopIndexedTopDUContext : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	public:
		KrossKDevelopIndexedTopDUContext(KDevelop::IndexedTopDUContext* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::IndexedTopDUContext"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE KDevelop::TopDUContext* data() const { return wrapped->data(); }
		Q_SCRIPTABLE bool isLoaded() const { return wrapped->isLoaded(); }
		Q_SCRIPTABLE bool operator==(const KDevelop::IndexedTopDUContext& x0) const { return wrapped->operator==(x0); }
		Q_SCRIPTABLE bool operator!=(const KDevelop::IndexedTopDUContext& x0) const { return wrapped->operator!=(x0); }
		Q_SCRIPTABLE unsigned int index() const { return wrapped->index(); }
		Q_SCRIPTABLE KDevelop::IndexedString url() const { return wrapped->url(); }
	private:
		KDevelop::IndexedTopDUContext* wrapped;
};

class KrossKDevelopTopDUContext : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	Q_ENUMS(Flags)
	Q_FLAGS(Flags NoFlags UpdatingContext LastFlag)

	Q_ENUMS(Features)
	Q_FLAGS(Features VisibleDeclarationsAndContexts AllDeclarationsAndContexts AllDeclarationsContextsAndUses AllDeclarationsContextsAndUsesForRecursive)

	public:
		enum KrossFlags { NoFlags=KDevelop::TopDUContext::NoFlags, UpdatingContext=KDevelop::TopDUContext::UpdatingContext, LastFlag=KDevelop::TopDUContext::LastFlag };
		enum KrossFeatures { VisibleDeclarationsAndContexts=KDevelop::TopDUContext::VisibleDeclarationsAndContexts, AllDeclarationsAndContexts=KDevelop::TopDUContext::AllDeclarationsAndContexts, AllDeclarationsContextsAndUses=KDevelop::TopDUContext::AllDeclarationsContextsAndUses, AllDeclarationsContextsAndUsesForRecursive=KDevelop::TopDUContext::AllDeclarationsContextsAndUsesForRecursive };
		KrossKDevelopTopDUContext(KDevelop::TopDUContext* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::TopDUContext"); }
		void* wrappedObject() const { return wrapped; }

		Q_SCRIPTABLE void deleteSelf() { wrapped->deleteSelf(); }
		Q_SCRIPTABLE KDevelop::TopDUContext* sharedDataOwner() const { return wrapped->sharedDataOwner(); }
		Q_SCRIPTABLE KDevelop::TopDUContext* topContext() const { return wrapped->topContext(); }
		Q_SCRIPTABLE unsigned int ownIndex() const { return wrapped->ownIndex(); }
		Q_SCRIPTABLE KDevelop::IndexedString url() const { return wrapped->url(); }
		Q_SCRIPTABLE KSharedPtr< KDevelop::ParsingEnvironmentFile > parsingEnvironmentFile() const { return wrapped->parsingEnvironmentFile(); }
		Q_SCRIPTABLE bool deleting() const { return wrapped->deleting(); }
		Q_SCRIPTABLE bool inDuChain() const { return wrapped->inDuChain(); }
		Q_SCRIPTABLE void setInDuChain(bool x0) { wrapped->setInDuChain(x0); }
		Q_SCRIPTABLE bool isOnDisk() const { return wrapped->isOnDisk(); }
		Q_SCRIPTABLE QList< KSharedPtr< KDevelop::Problem > > problems() const { return wrapped->problems(); }
		Q_SCRIPTABLE void addProblem(const KSharedPtr< KDevelop::Problem >& x0) { wrapped->addProblem(x0); }
		Q_SCRIPTABLE void clearProblems() { wrapped->clearProblems(); }
		Q_SCRIPTABLE bool imports(const KDevelop::DUContext* x0, const KDevelop::SimpleCursor& x1) const { return wrapped->imports(x0, x1); }
		Q_SCRIPTABLE void importTrace(const KDevelop::TopDUContext* x0, KDevelop::ImportTrace& x1) const { wrapped->importTrace(x0, x1); }
		Q_SCRIPTABLE KDevelop::ImportTrace importTrace(const KDevelop::TopDUContext* x0) const { return wrapped->importTrace(x0); }
		Q_SCRIPTABLE KDevelop::TopDUContext::Features features() const { return wrapped->features(); }
		Q_SCRIPTABLE void setFeatures(KDevelop::TopDUContext::Features x0) { wrapped->setFeatures(x0); }
		Q_SCRIPTABLE int indexForUsedDeclaration(KDevelop::Declaration* x0, bool x1=true) { return wrapped->indexForUsedDeclaration(x0, x1); }
		Q_SCRIPTABLE KDevelop::Declaration* usedDeclarationForIndex(unsigned int x0) const { return wrapped->usedDeclarationForIndex(x0); }
		Q_SCRIPTABLE void clearUsedDeclarationIndices() { wrapped->clearUsedDeclarationIndices(); }
		Q_SCRIPTABLE KDevelop::IndexedString language() const { return wrapped->language(); }
		Q_SCRIPTABLE void setLanguage(KDevelop::IndexedString x0) { wrapped->setLanguage(x0); }
		Q_SCRIPTABLE KDevelop::TopDUContext::Flags flags() const { return wrapped->flags(); }
		Q_SCRIPTABLE void setFlags(KDevelop::TopDUContext::Flags x0) { wrapped->setFlags(x0); }
		Q_SCRIPTABLE void addImportedParentContext(KDevelop::DUContext* x0, const KDevelop::SimpleCursor& x1=KDevelop::SimpleCursor(), bool x2=false, bool x3=false) { wrapped->addImportedParentContext(x0, x1, x2, x3); }
		Q_SCRIPTABLE void addImportedParentContexts(const QList< QPair< KDevelop::TopDUContext*, KDevelop::SimpleCursor > >& x0, bool x1=false) { wrapped->addImportedParentContexts(x0, x1); }
		Q_SCRIPTABLE void removeImportedParentContext(KDevelop::DUContext* x0) { wrapped->removeImportedParentContext(x0); }
		Q_SCRIPTABLE void removeImportedParentContexts(const QList< KDevelop::TopDUContext* >& x0) { wrapped->removeImportedParentContexts(x0); }
		Q_SCRIPTABLE void clearImportedParentContexts() { wrapped->clearImportedParentContexts(); }
		Q_SCRIPTABLE QVector< KDevelop::DUContext::Import > importedParentContexts() const { return wrapped->importedParentContexts(); }
		Q_SCRIPTABLE QVector< KDevelop::DUContext* > importers() const { return wrapped->importers(); }
		Q_SCRIPTABLE QList< KDevelop::DUContext* > loadedImporters() const { return wrapped->loadedImporters(); }
		Q_SCRIPTABLE KDevelop::SimpleCursor importPosition(const KDevelop::DUContext* x0) const { return wrapped->importPosition(x0); }
		Q_SCRIPTABLE bool findDeclarationsInternal(const KDevVarLengthArray< KSharedPtr< KDevelop::DUContext::SearchItem >, 256 >& x0, const KDevelop::SimpleCursor& x1, const TypePtr< KDevelop::AbstractType >& x2, KDevVarLengthArray< KDevelop::Declaration*, 40 >& x3, const KDevelop::TopDUContext* x4, QFlags< KDevelop::DUContext::SearchFlag > x5) const { return wrapped->findDeclarationsInternal(x0, x1, x2, x3, x4, x5); }
	private:
		KDevelop::TopDUContext* wrapped;
};

class KrossKDevelopTopDUContextCache : public QObject, public Kross::WrapperInterface
{
	Q_OBJECT
	public:
		KrossKDevelopTopDUContextCache(KDevelop::TopDUContext::Cache* obj, QObject* parent=0) : QObject(parent), wrapped(obj)		{ setObjectName("KDevelop::TopDUContext::Cache"); }
		void* wrappedObject() const { return wrapped; }

	private:
		KDevelop::TopDUContext::Cache* wrapped;
};

bool topducontext_registerHandler(const QByteArray& name, Kross::MetaTypeHandler::FunctionPtr* handler)
{ Kross::Manager::self().registerMetaTypeHandler(name, handler); return false; }

namespace Handlers
{
QVariant _kDevelopTopDUContextCacheHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::TopDUContext::Cache* t=static_cast<KDevelop::TopDUContext::Cache*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::TopDUContext::Cache*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopTopDUContextCache(t, 0));
}
bool b_kDevelopTopDUContextCache1=topducontext_registerHandler("TopDUContext::Cache*", _kDevelopTopDUContextCacheHandler);
bool b_kDevelopTopDUContextCache=topducontext_registerHandler("KDevelop::TopDUContext::Cache*", _kDevelopTopDUContextCacheHandler);
QVariant kDevelopTopDUContextCacheHandler(KDevelop::TopDUContext::Cache* type){ return _kDevelopTopDUContextCacheHandler(type); }
QVariant kDevelopTopDUContextCacheHandler(const KDevelop::TopDUContext::Cache* type) { return _kDevelopTopDUContextCacheHandler((void*) type); }

QVariant _kDevelopTopDUContextHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::TopDUContext* t=static_cast<KDevelop::TopDUContext*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::TopDUContext*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopTopDUContext(t, 0));
}
bool b_kDevelopTopDUContext1=topducontext_registerHandler("TopDUContext*", _kDevelopTopDUContextHandler);
bool b_kDevelopTopDUContext=topducontext_registerHandler("KDevelop::TopDUContext*", _kDevelopTopDUContextHandler);
QVariant kDevelopTopDUContextHandler(KDevelop::TopDUContext* type){ return _kDevelopTopDUContextHandler(type); }
QVariant kDevelopTopDUContextHandler(const KDevelop::TopDUContext* type) { return _kDevelopTopDUContextHandler((void*) type); }

QVariant _kDevelopIndexedTopDUContextHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::IndexedTopDUContext* t=static_cast<KDevelop::IndexedTopDUContext*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::IndexedTopDUContext*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopIndexedTopDUContext(t, 0));
}
bool b_kDevelopIndexedTopDUContext1=topducontext_registerHandler("IndexedTopDUContext*", _kDevelopIndexedTopDUContextHandler);
bool b_kDevelopIndexedTopDUContext=topducontext_registerHandler("KDevelop::IndexedTopDUContext*", _kDevelopIndexedTopDUContextHandler);
QVariant kDevelopIndexedTopDUContextHandler(KDevelop::IndexedTopDUContext* type){ return _kDevelopIndexedTopDUContextHandler(type); }
QVariant kDevelopIndexedTopDUContextHandler(const KDevelop::IndexedTopDUContext* type) { return _kDevelopIndexedTopDUContextHandler((void*) type); }

QVariant _kDevelopReferencedTopDUContextHandler(void* type)
{
	if(!type) return QVariant();
	KDevelop::ReferencedTopDUContext* t=static_cast<KDevelop::ReferencedTopDUContext*>(type);
	Q_ASSERT(dynamic_cast<KDevelop::ReferencedTopDUContext*>(t));
	return qVariantFromValue((QObject*) new KrossKDevelopReferencedTopDUContext(t, 0));
}
bool b_kDevelopReferencedTopDUContext1=topducontext_registerHandler("ReferencedTopDUContext*", _kDevelopReferencedTopDUContextHandler);
bool b_kDevelopReferencedTopDUContext=topducontext_registerHandler("KDevelop::ReferencedTopDUContext*", _kDevelopReferencedTopDUContextHandler);
QVariant kDevelopReferencedTopDUContextHandler(KDevelop::ReferencedTopDUContext* type){ return _kDevelopReferencedTopDUContextHandler(type); }
QVariant kDevelopReferencedTopDUContextHandler(const KDevelop::ReferencedTopDUContext* type) { return _kDevelopReferencedTopDUContextHandler((void*) type); }

}
#include "topducontext.moc"
