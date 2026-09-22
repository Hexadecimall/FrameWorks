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
    const int count = outline ? (segments + 1) * 2 : segments * 3;
    auto *node = makeGeometryNode(outline ? QSGGeometry::DrawTriangleStrip : QSGGeometry::DrawTriangles,
                                  count, color, opacity);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    if (outline) {
        constexpr qreal strokeWidth = 2.0;
        for (int i = 0; i <= segments; ++i) {
            const qreal angle = (2.0 * std::numbers::pi_v<qreal> * i) / segments;
            const qreal cosine = std::cos(angle);
            const qreal sine = std::sin(angle);
            v[i * 2].set(rect.center().x() + cosine * rect.width() / 2,
                         rect.center().y() + sine * rect.height() / 2);
            v[i * 2 + 1].set(rect.center().x() + cosine * std::max<qreal>(0, rect.width() / 2 - strokeWidth),
                             rect.center().y() + sine * std::max<qreal>(0, rect.height() / 2 - strokeWidth));
        }
    } else {
        for (int i = 0; i < segments; ++i) {
            const qreal angleA = (2.0 * std::numbers::pi_v<qreal> * i) / segments;
            const qreal angleB = (2.0 * std::numbers::pi_v<qreal> * (i + 1)) / segments;
            v[i * 3].set(rect.center().x(), rect.center().y());
            v[i * 3 + 1].set(rect.center().x() + std::cos(angleA) * rect.width() / 2,
                             rect.center().y() + std::sin(angleA) * rect.height() / 2);
            v[i * 3 + 2].set(rect.center().x() + std::cos(angleB) * rect.width() / 2,
                             rect.center().y() + std::sin(angleB) * rect.height() / 2);
        }
    }
    return node;
}

QSGGeometryNode *arcNode(const QRectF &rect, const QColor &color, qreal opacity)
{
    constexpr int segments = 64;
    auto *node = makeGeometryNode(QSGGeometry::DrawTriangleStrip, (segments + 1) * 2, color, opacity);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    const qreal halfWidth = std::max<qreal>(1.5, 14.0 * rect.width() / 690.0);
    auto pointAt = [&rect](qreal t) {
        const qreal x = rect.left() + rect.width() * t;
        const qreal curve = 4.0 * (t - 0.5) * (t - 0.5);
        const qreal y = rect.bottom() - rect.height() * (0.12 + curve * 0.62);
        return QPointF(x, y);
    };
    for (int i = 0; i <= segments; ++i) {
        const qreal t = qreal(i) / segments;
        const QPointF point = pointAt(t);
        const QPointF before = pointAt(std::max<qreal>(0, t - 1.0 / segments));
        const QPointF after = pointAt(std::min<qreal>(1, t + 1.0 / segments));
        const QPointF tangent = after - before;
        const qreal length = std::hypot(tangent.x(), tangent.y());
        const QPointF normal = length > 0
            ? QPointF(-tangent.y() / length, tangent.x() / length) * halfWidth
            : QPointF();
        v[i * 2].set(point.x() + normal.x(), point.y() + normal.y());
        v[i * 2 + 1].set(point.x() - normal.x(), point.y() - normal.y());
    }
    return node;
}

QSGNode *selectionNode(const QRectF &rect)
{
    constexpr qreal strokeWidth = 2.0;
    auto *node = new QSGNode;
    const QColor color("#64e7ef");
    node->appendChildNode(rectangleNode({rect.left(), rect.top(), rect.width(), strokeWidth}, color, 1.0));
    node->appendChildNode(rectangleNode({rect.left(), rect.bottom() - strokeWidth, rect.width(), strokeWidth}, color, 1.0));
    node->appendChildNode(rectangleNode({rect.left(), rect.top(), strokeWidth, rect.height()}, color, 1.0));
    node->appendChildNode(rectangleNode({rect.right() - strokeWidth, rect.top(), strokeWidth, rect.height()}, color, 1.0));
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
