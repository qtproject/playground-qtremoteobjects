/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB)
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtRemoteObjects module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "RepParser.h"

#include <QTemporaryFile>
#include <QTest>
#include <QTextStream>

Q_DECLARE_METATYPE(ASTProperty::Modifier)

class tst_Parser : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testProperties_data();
    void testProperties();
};


void tst_Parser::testProperties_data()
{
    QTest::addColumn<QString>("propertyDeclaration");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<QString>("expectedName");
    QTest::addColumn<QString>("expectedDefaultValue");
    QTest::addColumn<ASTProperty::Modifier>("expectedModifier");

    QTest::newRow("default") << "PROP(QString foo)" << "QString" << "foo" << QString() << ASTProperty::ReadWrite;
    QTest::newRow("readonly") << "PROP(QString foo READONLY)" << "QString" << "foo" << QString() << ASTProperty::ReadOnly;
    QTest::newRow("constant") << "PROP(QString foo CONSTANT)" << "QString" << "foo" << QString() << ASTProperty::Constant;
    QTest::newRow("defaultWithValue") << "PROP(int foo=1)" << "int" << "foo" << "1" << ASTProperty::ReadWrite;
    QTest::newRow("readonlyWithValue") << "PROP(int foo=1 READONLY)" << "int" << "foo" << "1" << ASTProperty::ReadOnly;
    QTest::newRow("constantWithValue") << "PROP(int foo=1 CONSTANT)" << "int" << "foo" << "1" << ASTProperty::Constant;
    QTest::newRow("defaultWhitespaces") << "PROP(  QString   foo  )" << "QString" << "foo" << QString() << ASTProperty::ReadWrite;
    QTest::newRow("readonlyWhitespaces") << "PROP(  QString   foo   READONLY  )" << "QString" << "foo" << QString() << ASTProperty::ReadOnly;
    QTest::newRow("constantWhitespaces") << "PROP(  QString   foo   CONSTANT  )" << "QString" << "foo" << QString() << ASTProperty::Constant;
    QTest::newRow("defaultWithValueWhitespaces") << "PROP(  int foo  = 1 )" << "int" << "foo" << "1" << ASTProperty::ReadWrite;
    QTest::newRow("readonlyWithValueWhitespaces") << "PROP(  int foo = 1 READONLY  )" << "int" << "foo" << "1" << ASTProperty::ReadOnly;
    QTest::newRow("constantWithValueWhitespaces") << "PROP(  int foo = 1 CONSTANT )" << "int" << "foo" << "1" << ASTProperty::Constant;
    QTest::newRow("templatetype") << "PROP(QVector<int> bar)" << "QVector<int>" << "bar" << QString() << ASTProperty::ReadWrite;
    QTest::newRow("nested templatetype") << "PROP(QMap<int, QVector<int> > bar)" << "QMap<int, QVector<int> >" << "bar" << QString() << ASTProperty::ReadWrite;
    QTest::newRow("non-int default value") << "PROP(double foo=1.1 CONSTANT)" << "double" << "foo" << "1.1" << ASTProperty::Constant;
}

void tst_Parser::testProperties()
{
    QFETCH(QString, propertyDeclaration);
    QFETCH(QString, expectedType);
    QFETCH(QString, expectedName);
    QFETCH(QString, expectedDefaultValue);
    QFETCH(ASTProperty::Modifier, expectedModifier);

    QTemporaryFile file;
    file.open();
    QTextStream stream(&file);
    stream << "class TestClass" << endl;
    stream << "{" << endl;
    stream << propertyDeclaration << endl;
    stream << "};" << endl;
    file.close();

    RepParser parser(file.fileName());
    QVERIFY(parser.parse());

    const AST ast = parser.ast();
    QCOMPARE(ast.classes.count(), 1);

    const ASTClass astClass = ast.classes.first();
    const QVector<ASTProperty> properties = astClass.properties();
    QCOMPARE(properties.count(), 1);

    const ASTProperty property = properties.first();
    QCOMPARE(property.type(), expectedType);
    QCOMPARE(property.name(), expectedName);
    QCOMPARE(property.defaultValue(), expectedDefaultValue);
    QCOMPARE(property.modifier(), expectedModifier);
}

QTEST_APPLESS_MAIN(tst_Parser)

#include "tst_parser.moc"