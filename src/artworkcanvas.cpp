#include "artworkcanvas.h"

#include "documentcontroller.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGNode>
#include <cmath>
#include <numbers>

namespace {
constexpr qreal kDocumentWidth = 1200.0;
constexpr qreal kDocumentHeight = 1500.0;

QSGGeometryNode *makeGeometryNode(QSGGeometry::DrawingMode mode, int count,
                                  const QColor &color, qreal opacity = 1.0)
{
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
    geometry->setDrawingMode(mode);
    auto *material = new QSGFlatColorMaterial;
    QColor rendered = color;
    rendered.setAlphaF(rendered.alphaF() * opacity);
    material->setColor(rendered);
    auto *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);
    return node;
}

QRectF scaledRect(const QRectF &source, qreal sx, qreal sy)
{
    return {source.x() * sx, source.y() * sy, source.width() * sx, source.height() * sy};
}

QSGGeometryNode *rectangleNode(const QRectF &rect, const QColor &color, qreal opacity)
{
    auto *node = makeGeometryNode(QSGGeometry::DrawTriangleStrip, 4, color, opacity);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    v[0].set(rect.left(), rect.top()); v[1].set(rect.left(), rect.bottom());
    v[2].set(rect.right(), rect.top()); v[3].set(rect.right(), rect.bottom());
    return node;
}

QSGGeometryNode *ellipseNode(const QRectF &rect, const QColor &color, qreal opacity, bool outline)
{
    constexpr int segments = 96;
    const int count = outline ? segments + 1 : segments + 2;
    auto *node = makeGeometryNode(outline ? QSGGeometry::DrawLineStrip : QSGGeometry::DrawTriangleFan,
                                  count, color, opacity);
    if (outline) node->geometry()->setLineWidth(2.0f);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    int offset = 0;
    if (!outline) { v[0].set(rect.center().x(), rect.center().y()); offset = 1; }
    for (int i = 0; i <= segments; ++i) {
        const qreal angle = (2.0 * std::numbers::pi_v<qreal> * i) / segments;
        v[offset + i].set(rect.center().x() + std::cos(angle) * rect.width() / 2,
                          rect.center().y() + std::sin(angle) * rect.height() / 2);
    }
    return node;
}

QSGGeometryNode *arcNode(const QRectF &rect, const QColor &color, qreal opacity)
{
    constexpr int segments = 64;
    auto *node = makeGeometryNode(QSGGeometry::DrawLineStrip, segments + 1, color, opacity);
    node->geometry()->setLineWidth(28.0f);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    for (int i = 0; i <= segments; ++i) {
        const qreal t = qreal(i) / segments;
        const qreal x = rect.left() + rect.width() * t;
        const qreal curve = 4.0 * (t - 0.5) * (t - 0.5);
        const qreal y = rect.bottom() - rect.height() * (0.12 + curve * 0.62);
        v[i].set(x, y);
    }
    return node;
}

QSGGeometryNode *selectionNode(const QRectF &rect)
{
    auto *node = makeGeometryNode(QSGGeometry::DrawLineStrip, 5, QColor("#64e7ef"));
    node->geometry()->setLineWidth(2.0f);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    v[0].set(rect.left(), rect.top()); v[1].set(rect.right(), rect.top());
    v[2].set(rect.right(), rect.bottom()); v[3].set(rect.left(), rect.bottom());
    v[4].set(rect.left(), rect.top());
    return node;
}
}

ArtworkCanvas::ArtworkCanvas(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

void ArtworkCanvas::setDocument(DocumentController *document)
{
    if (m_document == document) return;
    if (m_document) disconnect(m_document, nullptr, this, nullptr);
    m_document = document;
    if (m_document) connect(m_document, &DocumentController::documentChanged,
                            this, &ArtworkCanvas::update);
    emit documentChanged();
    update();
}

QSGNode *ArtworkCanvas::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    delete oldNode;
    auto *root = new QSGNode;
    if (!m_document || width() <= 0 || height() <= 0) return root;

    const qreal sx = width() / kDocumentWidth;
    const qreal sy = height() / kDocumentHeight;
    const auto &layers = m_document->layers();
    for (const auto &layer : layers) {
        if (!layer.visible || layer.type == "Text") continue;
        const QRectF rect = scaledRect(layer.bounds, sx, sy);
        if (layer.type == "Rectangle")
            root->appendChildNode(rectangleNode(rect, layer.fill, layer.opacity));
        else if (layer.type == "Ellipse")
            root->appendChildNode(ellipseNode(rect, layer.fill, layer.opacity, false));
        else if (layer.type == "EllipseOutline")
            root->appendChildNode(ellipseNode(rect, layer.fill, layer.opacity, true));
        else if (layer.type == "Curve")
            root->appendChildNode(arcNode(rect, layer.fill, layer.opacity));
    }

    const int selected = m_document->selectedIndex();
    if (selected >= 0 && selected < layers.size())
        root->appendChildNode(selectionNode(scaledRect(layers[selected].bounds, sx, sy)));
    return root;
}
