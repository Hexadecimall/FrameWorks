#include <QSignalSpy>
#include <QTest>

#include "documentcontroller.h"

class DocumentControllerTest final : public QObject
{
    Q_OBJECT

private slots:
    void initializesPoster()
    {
        DocumentController document;
        QCOMPARE(document.rowCount(), 8);
        QCOMPARE(document.selectedIndex(), 3);
        QCOMPARE(document.selectionWidth(), 600.0);
    }

    void editsAndRestoresGeometry()
    {
        DocumentController document;
        document.setSelectionX(420);
        QCOMPARE(document.selectionX(), 420.0);
        QVERIFY(document.canUndo());
        document.undo();
        QCOMPARE(document.selectionX(), 300.0);
        document.redo();
        QCOMPARE(document.selectionX(), 420.0);
    }

    void protectsLockedLayer()
    {
        DocumentController document;
        document.selectLayer(7);
        const int count = document.rowCount();
        document.removeSelected();
        QCOMPARE(document.rowCount(), count);
    }

    void duplicatesSelection()
    {
        DocumentController document;
        const int count = document.rowCount();
        document.duplicateSelected();
        QCOMPARE(document.rowCount(), count + 1);
        QCOMPARE(document.selectionX(), 328.0);
    }

    void createsBlankDocument()
    {
        DocumentController document;
        document.newDocument();
        QCOMPARE(document.rowCount(), 1);
        QCOMPARE(document.selectedIndex(), 0);
        QVERIFY(!document.starterPoster());
        QVERIFY(!document.canUndo());
    }
};

QTEST_MAIN(DocumentControllerTest)
#include "test_documentcontroller.moc"
