#pragma once

#include "documentcontroller.h"

#include <QPointer>
#include <QQuickItem>

class ArtworkCanvas : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(DocumentController *document READ document WRITE setDocument NOTIFY documentChanged)

public:
    explicit ArtworkCanvas(QQuickItem *parent = nullptr);
    DocumentController *document() const { return m_document; }
    void setDocument(DocumentController *document);

signals:
    void documentChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    QPointer<DocumentController> m_document;
};
