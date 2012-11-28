/*
 * This file is part of KDevelop
 * Copyright 2012 Milian Wolff <mail@milianw.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEMPLATEPREVIEW_H
#define TEMPLATEPREVIEW_H

#include <QWidget>
#include <QHash>

namespace KTextEditor {
class Document;
class View;
}

namespace KDevelop {
class TemplateRenderer;
}

/**
 * A KTextEditor::View wrapper to show a preview of a template.
 */
class TemplatePreview : public QWidget
{
    Q_OBJECT
public:
    TemplatePreview(QWidget* parent, Qt::WindowFlags f = 0);
    virtual ~TemplatePreview();

    /**
     * Set the template contents which will be rendered.
     *
     * @p text the template contents
     * @p isProject set to true if the contents resemble a project template
     * @return an error message, or an empty string if everything worked
     */
    QString setText(const QString& text, bool isProject = false);

    /**
     * @return The read-only document.
     */
    KTextEditor::Document* document() const;

    /**
     * @return The renderer used for file templates.
     */
    KDevelop::TemplateRenderer* renderer() const;

private:
    Q_DISABLE_COPY(TemplatePreview)

    QScopedPointer<KDevelop::TemplateRenderer> m_renderer;
    QHash<QString, QString> m_variables;
    QScopedPointer<KTextEditor::Document> m_preview;
    KTextEditor::View* m_view;

};

#endif // TEMPLATEPREVIEW_H
