/*
   Copyright 2007 David Nolden <david.nolden.kdevelop@art-master.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "macronavigationcontext.h"

#include <QtGui/QTextDocument>

#include <klocale.h>

#include <language/duchain/duchain.h>

#include "../../parser/rpp/macrorepository.h"
#include "../../parser/rpp/chartools.h"

namespace Cpp {
using namespace KDevelop;
using namespace rpp;

MacroNavigationContext::MacroNavigationContext(const pp_macro& macro, QString preprocessedBody)
  : AbstractNavigationContext(TopDUContextPointer(0)),
    m_macro(copyConstantMacro(&macro)), m_body(preprocessedBody)
{}

MacroNavigationContext::~MacroNavigationContext()
{
  delete m_macro;
}

QString MacroNavigationContext::name() const
{
  return m_macro->name.str();
}

QString MacroNavigationContext::html(bool shorten)
{
  m_currentText  = "<html><body><p><small><small>";
  m_linkCount = 0;
  addExternalHtml(m_prefix);

  QString args;

  if(m_macro->formalsSize()) {
    args = "(";

    bool first = true;
    FOREACH_CUSTOM(uint b, m_macro->formals(), m_macro->formalsSize()) {
      if(!first)
        args += ", ";
      first = false;
      args += IndexedString(b).str();
    }

    args += ")";
  }

  m_currentText += (m_macro->function_like ? i18n("Function macro") : i18n("Macro")) + " " + importantHighlight(m_macro->name.str()) + " " + args +  "<br />";

  KUrl u(m_macro->file.str());
  NavigationAction action(u, KTextEditor::Cursor(m_macro->sourceLine,0));
  QList<TopDUContext*> duchains = DUChain::self()->chainsForDocument(u);

  if(!shorten) {
    m_currentText += "<br />";

    if(!m_body.isEmpty()) {
    m_currentText += labelHighlight(i18n("Preprocessed body:")) + "<br />";
    m_currentText += codeHighlight(Qt::escape(m_body));
    m_currentText += "<br />";
    }


    m_currentText += labelHighlight(i18n("Body:")) + "<br />";

    m_currentText += codeHighlight(Qt::escape(QString::fromUtf8(stringFromContents(m_macro->definition(), m_macro->definitionSize()))));
    m_currentText += "<br />";
  }

  makeLink(u.pathOrUrl(), u.pathOrUrl(), action);

  m_currentText += "</small></small></p></body></html>";
  return m_currentText;
}

}
