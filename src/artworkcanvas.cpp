#include "artworkcanvas.h"

#include "documentcontroller.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGVertexColorMaterial>
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

QColor mixColor(const QColor &from, const QColor &to, qreal amount)
{
    amount = std::clamp(amount, 0.0, 1.0);
    return QColor::fromRgbF(from.redF() + (to.redF() - from.redF()) * amount,
                            from.greenF() + (to.greenF() - from.greenF()) * amount,
                            from.blueF() + (to.blueF() - from.blueF()) * amount,
                            from.alphaF() + (to.alphaF() - from.alphaF()) * amount);
}

void setColoredVertex(QSGGeometry::ColoredPoint2D &vertex, qreal x, qreal y,
                      const QColor &color, qreal opacity = 1.0)
{
    vertex.set(x, y, color.red(), color.green(), color.blue(),
               std::clamp(qRound(color.alpha() * opacity), 0, 255));
}

QSGGeometryNode *gradientRectangleNode(const QRectF &rect)
{
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 4);
    geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
    auto *v = geometry->vertexDataAsColoredPoint2D();
    const QColor start("#ff5c35");
    const QColor end("#ff9457");
    setColoredVertex(v[0], rect.left(), rect.top(), start);
    setColoredVertex(v[1], rect.left(), rect.bottom(), mixColor(start, end, 0.5));
    setColoredVertex(v[2], rect.right(), rect.top(), mixColor(start, end, 0.5));
    setColoredVertex(v[3], rect.right(), rect.bottom(), end);
    auto *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);
    node->setMaterial(new QSGVertexColorMaterial);
    node->setFlag(QSGNode::OwnsMaterial);
    return node;
}

QColor orbColor(qreal position)
{
    const QColor start("#bffcff");
    const QColor middle("#6c7bff");
    const QColor end("#ba45ff");
    return position <= 0.5 ? mixColor(start, middle, position * 2.0)
                           : mixColor(middle, end, (position - 0.5) * 2.0);
}

QSGGeometryNode *gradientEllipseNode(const QRectF &rect, qreal opacity)
{
    constexpr int segments = 96;
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), segments * 3);
    geometry->setDrawingMode(QSGGeometry::DrawTriangles);
    auto *v = geometry->vertexDataAsColoredPoint2D();
    auto positionFor = [&rect](qreal x, qreal y) {
        return ((x - rect.left()) / rect.width() + (y - rect.top()) / rect.height()) / 2.0;
    };
    for (int i = 0; i < segments; ++i) {
        const qreal angleA = (2.0 * std::numbers::pi_v<qreal> * i) / segments;
        const qreal angleB = (2.0 * std::numbers::pi_v<qreal> * (i + 1)) / segments;
        const QPointF a(rect.center().x() + std::cos(angleA) * rect.width() / 2,
                        rect.center().y() + std::sin(angleA) * rect.height() / 2);
        const QPointF b(rect.center().x() + std::cos(angleB) * rect.width() / 2,
                        rect.center().y() + std::sin(angleB) * rect.height() / 2);
        setColoredVertex(v[i * 3], rect.center().x(), rect.center().y(), orbColor(0.5), opacity);
        setColoredVertex(v[i * 3 + 1], a.x(), a.y(), orbColor(positionFor(a.x(), a.y())), opacity);
        setColoredVertex(v[i * 3 + 2], b.x(), b.y(), orbColor(positionFor(b.x(), b.y())), opacity);
    }
    auto *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);
    node->setMaterial(new QSGVertexColorMaterial);
    node->setFlag(QSGNode::OwnsMaterial);
    return node;
}

QSGGeometryNode *dashedEllipseNode(const QRectF &rect, const QColor &color, qreal opacity)
{
    constexpr int dashCount = 84;
    constexpr qreal dashFraction = 8.0 / 28.0;
    constexpr qreal strokeWidth = 2.0;
    auto *node = makeGeometryNode(QSGGeometry::DrawTriangles, dashCount * 6, color, opacity);
    auto *v = node->geometry()->vertexDataAsPoint2D();
    for (int i = 0; i < dashCount; ++i) {
        const qreal angleA = 2.0 * std::numbers::pi_v<qreal> * i / dashCount;
        const qreal angleB = angleA + 2.0 * std::numbers::pi_v<qreal> * dashFraction / dashCount;
        const qreal cosA = std::cos(angleA), sinA = std::sin(angleA);
        const qreal cosB = std::cos(angleB), sinB = std::sin(angleB);
        const QPointF outerA(rect.center().x() + cosA * rect.width() / 2,
                             rect.center().y() + sinA * rect.height() / 2);
        const QPointF innerA(rect.center().x() + cosA * (rect.width() / 2 - strokeWidth),
                             rect.center().y() + sinA * (rect.height() / 2 - strokeWidth));
        const QPointF outerB(rect.center().x() + cosB * rect.width() / 2,
                             rect.center().y() + sinB * rect.height() / 2);
        const QPointF innerB(rect.center().x() + cosB * (rect.width() / 2 - strokeWidth),
                             rect.center().y() + sinB * (rect.height() / 2 - strokeWidth));
        const int offset = i * 6;
        v[offset].set(outerA.x(), outerA.y()); v[offset + 1].set(innerA.x(), innerA.y());
        v[offset + 2].set(outerB.x(), outerB.y()); v[offset + 3].set(outerB.x(), outerB.y());
        v[offset + 4].set(innerA.x(), innerA.y()); v[offset + 5].set(innerB.x(), innerB.y());
    }
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

QSGNode *arcNode(const QRectF &rect, const QColor &color, qreal opacity)
{
    constexpr int segments = 64;
    auto *root = new QSGNode;
    auto *stroke = makeGeometryNode(QSGGeometry::DrawTriangleStrip, (segments + 1) * 2, color, opacity);
    auto *v = stroke->geometry()->vertexDataAsPoint2D();
    const qreal halfWidth = std::max<qreal>(1.5, 24.0 * rect.width() / 690.0);
    auto pointAt = [&rect](qreal t) {
        const QPointF p0(rect.left() + rect.width() * 5.0 / 690.0, rect.top() + rect.height() * 95.0 / 280.0);
        const QPointF p1(rect.left() + rect.width() * 200.0 / 690.0, rect.top() + rect.height() * 310.0 / 280.0);
        const QPointF p2(rect.left() + rect.width() * 540.0 / 690.0, rect.bottom());
        const QPointF p3(rect.left() + rect.width() * 680.0 / 690.0, rect.top() + rect.height() * 35.0 / 280.0);
        const qreal u = 1.0 - t;
        return p0 * (u * u * u) + p1 * (3.0 * u * u * t)
             + p2 * (3.0 * u * t * t) + p3 * (t * t * t);
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
    root->appendChildNode(stroke);
    const QPointF start = pointAt(0.0);
    const QPointF end = pointAt(1.0);
    root->appendChildNode(ellipseNode({start.x() - halfWidth, start.y() - halfWidth,
                                      halfWidth * 2, halfWidth * 2}, color, opacity, false));
    root->appendChildNode(ellipseNode({end.x() - halfWidth, end.y() - halfWidth,
                                      halfWidth * 2, halfWidth * 2}, color, opacity, false));
    return root;
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
    const QPointF handles[] = {
        rect.topLeft(), QPointF(rect.center().x(), rect.top()), rect.topRight(),
        QPointF(rect.right(), rect.center().y()), rect.bottomRight(),
        QPointF(rect.center().x(), rect.bottom()), rect.bottomLeft(),
        QPointF(rect.left(), rect.center().y())
    };
    for (const QPointF &point : handles) {
        node->appendChildNode(rectangleNode({point.x() - 4, point.y() - 4, 8, 8}, color, 1.0));
        node->appendChildNode(rectangleNode({point.x() - 2, point.y() - 2, 4, 4}, QColor("#ffffff"), 1.0));
    }
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
    for (auto iterator = layers.crbegin(); iterator != layers.crend(); ++iterator) {
        const auto &layer = *iterator;
        if (!layer.visible || layer.type == "Text") continue;
        const QRectF rect = scaledRect(layer.bounds, sx, sy);
        if (layer.name == "Background") {
            if (layer.fill == QColor("#ff754d"))
                root->appendChildNode(gradientRectangleNode(rect));
            else
                root->appendChildNode(rectangleNode(rect, layer.fill, layer.opacity));
        }
        else if (layer.name == "Orb") {
            for (int blur = 12; blur >= 1; --blur) {
                const qreal spread = blur * 2.2 * (sx + sy) / 2.0;
                const QRectF shadow = rect.adjusted(-spread, -spread + 30 * sy,
                                                     spread, spread + 30 * sy);
                root->appendChildNode(ellipseNode(shadow, QColor("#000000"), 0.012, false));
            }
            if (layer.fill == QColor("#7868ff"))
                root->appendChildNode(gradientEllipseNode(rect, layer.opacity));
            else
                root->appendChildNode(ellipseNode(rect, layer.fill, layer.opacity, false));
        } else if (layer.type == "Ellipse")
            root->appendChildNode(ellipseNode(rect, layer.fill, layer.opacity, false));
        else if (layer.type == "EllipseOutline")
            root->appendChildNode(dashedEllipseNode(rect, layer.fill, layer.opacity));
        else if (layer.type == "Curve")
            root->appendChildNode(arcNode(rect, layer.fill, layer.opacity));
    }

    const int selected = m_document->selectedIndex();
    if (selected >= 0 && selected < layers.size())
        root->appendChildNode(selectionNode(scaledRect(layers[selected].bounds, sx, sy)));
    return root;
}
