/***************************************************************************
                          cclasstreehandler.cpp  -  implementation
                             -------------------
    begin                : Fri Mar 19 1999
    copyright            : (C) 1999 by Jonas Nordin
    email                : jonas.nordin@cenacle.se
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cclasstreehandler.h"

#include <qlistview.h>
#include <assert.h>


/*********************************************************************
 *                                                                   *
 *                     CREATION RELATED METHODS                      *
 *                                                                   *
 ********************************************************************/

/*--------------------------- CClassTreeHandler::CClassTreeHandler()
 * CClassTreeHandler()
 *   Constructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassTreeHandler::CClassTreeHandler()
  : CTreeHandler()
{
}

/*--------------------------- CClassTreeHandler::~CClassTreeHandler()
 * ~CClassTreeHandler()
 *   Destructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassTreeHandler::~CClassTreeHandler()
{
}

/*********************************************************************
 *                                                                   *
 *                    METHODS TO SET ATTRIBUTE VALUES                *
 *                                                                   *
 ********************************************************************/

/*--------------------------------------- CClassTreeHandler::setStore()
 * setStore()
 *   Set the classtore.
 *
 * Parameters:
 *   aStore         The store.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::setStore( ClassStore *aStore )
{
  assert( aStore != NULL );

  store = aStore;
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC METHODS                           *
 *                                                                   *
 ********************************************************************/

/*------------------------------------ CClassTreeHandler::addScopes()
 * addScopes()
 *   Add a list of scope to add to the view.
 *
 * Parameters:
 *   list            List of scopes.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addScopes( QList<ParsedScopeContainer> *list,
                                    QListViewItem *parent )
{
  ParsedScopeContainer *scope;

  for( scope = list->first(); scope != NULL; scope = list->next())
    addScope( scope, parent );
}

/*------------------------------------- CClassTreeHandler::addScope()
 * addScope()
 *   Add a scope to the view.
 *
 * Parameters:
 *   aMethod         Method to add
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addScope( ParsedScopeContainer *aScope,
                                  QListViewItem *parent )
{
  assert( aScope != NULL );
  assert( parent != NULL );

  QList<ParsedScopeContainer> *scopeList;
  QList<ParsedClass> *classList;
  QList<ParsedStruct> *structList;
  QList<ParsedMethod> *methodList;
  QList<ParsedAttribute> *attrList;

  QListViewItem *item = addItem( aScope->name(), THSCOPE, parent );

  // Add namespaces
  scopeList = aScope->getSortedScopeList();
  addScopes( scopeList, item );
  delete scopeList;

  // Add classes
  classList = aScope->getSortedClassList();
  addClasses( classList, item );
  delete classList;

  // Add structs
  structList = aScope->getSortedStructList();
  addStructs( structList, item, CTHALL );
  delete structList;

  // Add functions
  methodList = aScope->getSortedMethodList();
  addMethods( methodList, item, CTHALL );
  delete methodList;

  // Add variables
  attrList = aScope->getSortedAttributeList();
  addAttributes( attrList, item, CTHALL );
  delete attrList;
}

/*---------------------------------- CClassTreeHandler::updateClass()
 * updateClass()
 *   Update the class in the view using the supplied parent.
 *
 * Parameters:
 *   aClass       Class to update.
 *   parent       The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::updateClass( ParsedClass *aClass,
                                     QListViewItem *parent )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  QListViewItem *current;
  QListViewItem *next;

  current = parent->firstChild();

  // Remove all items belonging to this class.
  while( current != NULL )
  {
    next = current->nextSibling();
    parent->removeItem( current );
    current = next;
  }

  // Add parts of the class
  addSubclassesFromClass( aClass, parent );
  addStructsFromClass( aClass, parent, CTHALL );
  addMethodsFromClass( aClass, parent, CTHALL );
  addSlotsFromClass( aClass, parent );
  addSignalsFromClass( aClass, parent );
  addAttributesFromClass( aClass, parent, CTHALL );
}

/*---------------------------------- CClassTreeHandler::addClasses()
 * addClasses()
 *   Add a list of classes to the view.
 *
 * Parameters:
 *   list         List of classes to add.
 *   parent       The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addClasses( QList<ParsedClass> *list, QListViewItem *parent )
{
  QListViewItem *item;
  ParsedClass *aPC;

  for( aPC = list->first();
       aPC != NULL;
       aPC = list->next() )
  {
    item = addClass( aPC, parent );
    updateClass( aPC, item );
    setLastItem( item );
  }
}

/*---------------------------------- CClassTreeHandler::addClass()
 * addClass()
 *   Add a class to the view.
 *
 * Parameters:
 *   aClass       Class to add.
 *   parent       The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
QListViewItem *CClassTreeHandler::addClass( ParsedClass *aClass,
                                            QListViewItem *parent )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  return addItem( aClass->name(), THCLASS, parent );
}

/*---------------------------------- CClassTreeHandler::addClass()
 * addClass()
 *   Add a class to the view.
 *
 * Parameters:
 *   aName        Class to add.
 *   parent       The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
QListViewItem *CClassTreeHandler::addClass( const char *aName,
                                            QListViewItem *parent )
{
  assert( aName != NULL );
  assert( parent != NULL );

  return addItem( aName, THCLASS, parent );
}

/*------------------------ CClassTreeHandler::addSubclassesFromClass()
 * addSubclassesFromClass()
 *   Add all subclasses from the class to the view.
 *
 * Parameters:
 *   aClass       Class with subclasses to add.
 *   parent       The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addSubclassesFromClass( ParsedClass *aClass,
                                                QListViewItem *parent )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  QListViewItem *ci;

  for( aClass->classIterator.toFirst();
       aClass->classIterator.current();
       ++aClass->classIterator )
  {
    ci = addClass( aClass->classIterator.current()->name(), parent );
    updateClass( aClass->classIterator.current(), ci );
    setLastItem( ci );
  }
}

/*-------------------------- CClassTreeHandler::addStructsFromClass()
 * addStructsFromClass()
 *   Add a the selected structures from the class to the view.
 *
 * Parameters:
 *   aClass       Class with structs to add.
 *   parent       The parent item.
 *   filter       The selection.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addStructsFromClass( ParsedClass *aClass,
                                             QListViewItem *parent,
                                             CTHFilter filter )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  QList<ParsedStruct> *list;

  list = aClass->getSortedStructList();
  addStructs( list, parent, filter );
  delete list;
}

/*------------------------------------ CClassTreeHandler::addStructs()
 * addStructs()
 *   Add all structs from a list to the view.
 * Parameters:
 *   list            List with all structs to add
 *   parent          The parent item.
 *   filter          The selection.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addStructs( QList<ParsedStruct> *list,
                                    QListViewItem *parent,
                                    CTHFilter filter )
{
  assert( list != NULL );
  assert( parent != NULL );

  ParsedStruct *aStruct;

  // Add the structures
  for( aStruct = list->first();
       aStruct != NULL;
       aStruct = list->next() )
  {
    if( filter == CTHALL || filter == (CTHFilter)aStruct->access() )
      addStruct( aStruct, parent );
  }
}

/*------------------------------------ CClassTreeHandler::addStruct()
 * addStruct()
 *   Add a struct to the view.
 *
 * Parameters:
 *   aStruct         Structure to add
 *   parent          The parent item.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addStruct( ParsedStruct *aStruct,
                                   QListViewItem *parent )
{
  QListViewItem *root;
  ParsedAttribute *anAttr;
  ParsedMethod* anMethod; // --- Daniel
  QList<ParsedAttribute> *list;
  QList<ParsedMethod>* method; // --- Daniel
  QList<ParsedStruct> *structList;
  ParsedStruct *childStruct;

  root = addItem( aStruct->name(), THSTRUCT, parent );

  structList = aStruct->getSortedStructList();

  for( childStruct = structList->first();
       childStruct != NULL;
       childStruct = structList->next() )
  {
    addStruct( childStruct, root );
  }

  delete structList;

  list = aStruct->getSortedAttributeList();

  for( anAttr = list->first();
       anAttr != NULL;
       anAttr = list->next() )
  {
    addAttribute( anAttr, root );
  }

  delete list;

  // --- added by Daniel
  method = aStruct->getSortedMethodList( );
  for( anMethod = method->first( );
       anMethod != NULL;
       anMethod = method->next( ) )
    {
      addMethod( anMethod, root );
    }

  delete method;
}

/*-------------------------- CClassTreeHandler::addMethodsFromClass()
 * addMethodsFromClass()
 *   Add a the selected methods from the class to the view.
 *
 * Parameters:
 *   aClass       Class with methods to add.
 *   parent       The parent item.
 *   filter       The selection.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addMethodsFromClass( ParsedClass *aClass,
                                             QListViewItem *parent,
                                             CTHFilter filter )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  QList<ParsedMethod> *list;

  list = aClass->getSortedMethodList();

  addMethods( list, parent, filter );

  delete list;
}

/*------------------------------------ CClassTreeHandler::addMethods()
 * addMethods()
 *   Add a list of methods to the view.
 *
 * Parameters:
 *   list            List of methods to add.
 *   parent          The parent item.
 *   filter          The selection.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addMethods( QList<ParsedMethod> *list,
                                    QListViewItem *parent,
                                    CTHFilter filter )
{
  assert( list != NULL );
  assert( parent != NULL );

  ParsedMethod *aMethod;

  // Add the methods
  for( aMethod = list->first();
       aMethod != NULL;
       aMethod = list->next() )
  {
    if( filter == CTHALL || filter == (CTHFilter)aMethod->access() )
      addMethod( aMethod, parent );
  }
}

/*------------------------------------ CClassTreeHandler::addMethod()
 * addMethod()
 *   Add a method to the view.
 *
 * Parameters:
 *   aMethod         Method to add.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addMethod( ParsedMethod *aMethod,
                                   QListViewItem *parent )
{
  assert( aMethod );
  assert( parent != NULL );

  THType type = THPUBLIC_METHOD;
  QString str;

  if( aMethod->isProtected() )
    type = THPROTECTED_METHOD;
  else if( aMethod->isPrivate() )
    type = THPRIVATE_METHOD;

  str = aMethod->asString();
  addItem( str, type, parent );
}

/*-------------------------- CClassTreeHandler::addAttributesFromClass()
 * addAttributesFromClass()
 *   Add a the selected methods from the class to the view.
 *
 * Parameters:
 *   aClass       Class with methods to add.
 *   parent       The parent item.
 *   filter       The selection.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addAttributesFromClass( ParsedClass *aClass,
                                                QListViewItem *parent,
                                                CTHFilter filter )
{
  assert( aClass != NULL );
  assert( parent != NULL );

  QList<ParsedAttribute> *list;

  list = aClass->getSortedAttributeList();

  addAttributes( list, parent, filter );

  delete list;
}

/*--------------------------------- CClassTreeHandler::addAttributes()
 * addAttributes()
 *   Add a list of attributes to the view.
 *
 * Parameters:
 *   list            List of methods to add.
 *   parent          The parent item.
 *   filter          The selection.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addAttributes( QList<ParsedAttribute> *list,
                                       QListViewItem *parent,
                                       CTHFilter filter )
{
  assert( list != NULL );
  assert( parent != NULL );

  ParsedAttribute *aAttr;

  // Add the methods
  for( aAttr = list->first();
       aAttr != NULL;
       aAttr = list->next() )
  {
    if( filter == CTHALL || filter == (CTHFilter)aAttr->access() )
      addAttribute( aAttr, parent );
  }
}

/*------------------------------------ CClassTreeHandler::addAttribute()
 * addAttribute()
 *   Add an attribute to the view.
 *
 * Parameters:
 *   aMethod         Method to add
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addAttribute( ParsedAttribute *aAttr,
                                      QListViewItem *parent )
{
  assert( aAttr != NULL );
  assert( parent != NULL );

  THType type = THPUBLIC_ATTR;

  if( aAttr->isProtected() )
    type = THPROTECTED_ATTR;
  else if( aAttr->isPrivate() )
    type = THPRIVATE_ATTR;

  addItem( aAttr->name(), type, parent );
}

/*------------------------------------- CClassTreeHandler::addSlots()
 * addSlots()
 *   Add all slots from a class to the view.
 *
 * Parameters:
 *   aPC             Class that holds the data.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addSlotsFromClass( ParsedClass *aPC, QListViewItem *parent )
{
  ParsedMethod *aMethod;
  QString str;
  QList<ParsedMethod> *list;

  THType type = THPUBLIC_SLOT;

  list = aPC->getSortedSlotList();

  // Add the methods
  for( aMethod = list->first();
       aMethod != NULL;
       aMethod = list->next() )
  {
    if( aMethod->isProtected() )
      type = THPROTECTED_SLOT;
    else if( aMethod->isPrivate() )
      type = THPRIVATE_SLOT;

    str = aMethod->asString();
    addItem( str, type, parent );
  }

  delete list;
}

/*------------------------- CClassTreeHandler::addSignalsFromClass()
 * addSignalsFromClass()
 *   Add all signals from a class to the view.
 *
 * Parameters:
 *   aPC             Class that holds the data.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addSignalsFromClass( ParsedClass *aPC, QListViewItem *parent )
{
  ParsedMethod *aMethod;
  QString str;
  QList<ParsedMethod> *list;

  // Add all signals.
  list = aPC->getSortedSignalList();
  for( aMethod = list->first();
       aMethod != NULL;
       aMethod = list->next() )
  {
    str = aMethod->asString();
    addItem( str, THSIGNAL, parent );
  }

  delete list;
}

/*--------------------------- CClassTreeHandler::addGlobalFunctions()
 * addGlobalFunctions()
 *   Add a list of global functions to the view.
 *
 * Parameters:
 *   list            List of global functions.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addGlobalFunctions( QList<ParsedMethod> *list,
                                            QListViewItem *parent )
{
  assert( list != NULL );
  assert( parent != NULL );

  ParsedMethod *aMeth;

  for( aMeth = list->first();
       aMeth != NULL;
       aMeth = list->next() )
  {
    addGlobalFunc( aMeth, parent );
  }
}

/*-------------------------------- CClassTreeHandler::addGlobalFunc()
 * addGlobalFunc()
 *   Add a global function to the view.
 *
 * Parameters:
 *   aMethod         Method to add
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addGlobalFunc( ParsedMethod *aMethod,
                                       QListViewItem *parent )
{
  assert( aMethod != NULL );
  assert( parent != NULL );

  QString str;

  str = aMethod->asString();
  addItem( str, THGLOBAL_FUNCTION, parent );
}

/*--------------------------- CClassTreeHandler::addGlobalVariables()
 * addGlobalVariables()
 *   Add a list of global variables to the view.
 *
 * Parameters:
 *   list            List of global functions.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addGlobalVariables( QList<ParsedAttribute> *list, QListViewItem *parent )
{
  assert( list != NULL );
  assert( parent != NULL );

  ParsedAttribute *aAttr;

  for( aAttr = list->first();
       aAttr != NULL;
       aAttr = list->next() )
    addGlobalVar( aAttr, parent );
}

/*------------------------------------ CClassTreeHandler::addGlobalVar()
 * addGlobalVar()
 *   Add a global variable to the view.
 *
 * Parameters:
 *   aAttr           Attribute to add
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addGlobalVar( ParsedAttribute *aAttr,
                                      QListViewItem *parent )
{
  assert( aAttr != NULL );
  assert( parent != NULL );

  addItem( aAttr->name(), THGLOBAL_VARIABLE, parent );
}


/*------------------------------ CClassTreeHandler::addGlobalStructs()
 * addGlobalStructs()
 *   Add a list of global structures to the view.
 *
 * Parameters:
 *   list            List of global structures.
 *   parent          The parent item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::addGlobalStructs( QList<ParsedStruct> *list,
                                          QListViewItem *parent )
{
  ParsedStruct *aStruct;

  for( aStruct = list->first();
       aStruct != NULL;
       aStruct = list->next() )
    addStruct( aStruct, parent );
}

/*------------------------------- CClassTreeHandler::getCurrentNames()
 * getCurrentNames()
 *   Get the names and types of the currently selected class/declaration.
 *   class == NULL for global declarations.
 *
 * Parameters:
 *   className       Name of the parent class(if any).
 *   declName        Name of the item(==NULL for classes).
 *   idxType         The type of the selected item.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassTreeHandler::getCurrentNames( QString &parentPath,
                                         QString &itemName,
                                         THType &parentType,
                                         THType &aItemType )
{
  QListViewItem *iter;
  QListViewItem *item;
  QListViewItem *parent;
  bool isContainer;

  item = tree->currentItem();
  parent = item->parent();

  if (!parent) return ;

  aItemType = itemType();
  parentType = itemType(parent);

  // Set the container flag
  isContainer  = ( aItemType ==THCLASS ||
                   aItemType == THSTRUCT ||
                   aItemType == THSCOPE );

  // If we're viewing a container we start the classname iteration at the
  // current item
  if( isContainer )
  {
    parentPath = item->text( 0 );
    itemName = "";
    parentType = itemType();
    aItemType = parentType;

    iter = parent;
  }
  else if( parentType != THFOLDER ) // Start at the parent.
  {
    parentPath = parent->text( 0 );
    itemName = item->text(0);
    parentType = itemType( parent );
    aItemType = itemType();

    iter = parent->parent();
  }
  else // Global methods and attributes
  {
    parentPath = "";
    itemName = item->text(0);
    parentType = THFOLDER;
    aItemType = itemType();

    iter = NULL;
  }

  // Build the rest of the path
  while( iter != NULL && itemType( iter ) != THFOLDER )
  {
    parentPath = "." + parentPath;
    parentPath = iter->text(0) + parentPath;

    iter = iter->parent();
  }
}
