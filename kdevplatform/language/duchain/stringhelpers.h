/*
    SPDX-FileCopyrightText: 2007 David Nolden <david.nolden.kdevelop@art-master.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KDEVPLATFORM_STRINGHELPERS_H
#define KDEVPLATFORM_STRINGHELPERS_H

#include <language/languageexport.h>

#include <QChar>
#include <QScopedPointer>

class QByteArray;
class QString;

namespace KDevelop {
class ParamIteratorPrivate;

/**
 * Searches a fitting closing brace from left to right: a ')' for '(', ']' for '[', ...
 */
int KDEVPLATFORMLANGUAGE_EXPORT findClose(const QString& str, int pos);

/**
 * Searches in the given string for a ',' or closing brace,
 * while skipping everything between opened braces.
 * @param str string to search
 * @param pos position where to start searching
 * @param validEnd when this is set differently, the function will stop when it finds a comma or the given character, and not at closing-braces.
 * @return  On fail, str.length() is returned, else the position of the closing character.
 */
int KDEVPLATFORMLANGUAGE_EXPORT findCommaOrEnd(const QString& str, int pos, QChar validEnd = QLatin1Char( ' ' ));

/**
 * Removes white space at the beginning and end, and replaces contiguous inner white-spaces with single white-spaces. Newlines are treated as whitespaces, the returned text will have no more newlines.
 */
QString KDEVPLATFORMLANGUAGE_EXPORT reduceWhiteSpace(const QString& str);

QString KDEVPLATFORMLANGUAGE_EXPORT stripFinalWhitespace(const QString& str);
//
/**
 * Fills all c++-style comments  within the given code with the given 'replacement' character
 * Newlines are preserved.
 */
QString KDEVPLATFORMLANGUAGE_EXPORT clearComments(const QString& str, QChar replacement = QLatin1Char( ' ' ));
/**
 * Fills all c++-strings within the given code with the given 'replacement' character
 * Comments should have been removed before.
 */
QString KDEVPLATFORMLANGUAGE_EXPORT clearStrings(const QString& str, QChar replacement = QLatin1Char( ' ' ));

/**
 * Extracts the interesting information out of a comment.
 * For example it removes all the stars at the beginning, and re-indents the text.
 */
QString KDEVPLATFORMLANGUAGE_EXPORT formatComment(const QString& comment);

/**
 * Extracts the interesting information out of a comment.
 * For example it removes all the stars at the beginning, and re-indents the text.
 */
QByteArray KDEVPLATFORMLANGUAGE_EXPORT formatComment(const QByteArray& comment);

/**
 * Remove characters in @p str from the end of @p from
 *
 * @return number of stripped characters
 */
int KDEVPLATFORMLANGUAGE_EXPORT rStrip(const QByteArray& str, QByteArray& from);

/**
 * Remove characters in @p str from the beginning of @p from
 *
 * @return number of stripped characters
 */
int KDEVPLATFORMLANGUAGE_EXPORT strip(const QByteArray& str, QByteArray& from);

/**
 * Removes all whitespace from the string
 */
QString KDEVPLATFORMLANGUAGE_EXPORT removeWhitespace(const QString& str);

/**
 * Can be used to iterate through different kinds of parameters, for example template-parameters
 */
class KDEVPLATFORMLANGUAGE_EXPORT ParamIterator
{
public:
    /**
     * @param parens Should be a string containing the two parens between which the parameters are searched.
     * Example: "<>" or "()" Optionally it can also contain one third end-character.
     * If that end-character is encountered in the prefix, the iteration will be stopped.
     *
     * Example: When "<>:" is given, ParamIterator will only parse the first identifier of a C++ scope
     */
    ParamIterator(const QString& parens, const QString& source, int start = 0);
    ~ParamIterator();

    ParamIterator& operator ++();

    /**
     * Returns current found parameter
     */
    QString operator *();

    /**
     * Returns whether there is a current found parameter
     */
    operator bool() const;

    /**
     * Returns the text in front of the first opening-paren(if none found then the whole text)
     */
    QString prefix() const;

    uint position() const;

private:
    const QScopedPointer<class ParamIteratorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ParamIterator)
};
}

#endif
