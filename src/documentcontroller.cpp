#include "documentcontroller.h"

#include <algorithm>

DocumentController::DocumentController(QObject *parent)
    : QAbstractListModel(parent)
{
    m_layers = {
        {"Satellite", "Ellipse", {860, 802, 44, 44}, QColor("#f7ff64")},
        {"Arc", "Curve", {260, 700, 690, 280}, QColor("#1a1719")},
        {"Orbit", "EllipseOutline", {230, 270, 740, 740}, QColor("#ffffff"), 0.75},
        {"Orb", "Ellipse", {300, 340, 600, 600}, QColor("#7868ff")},
        {"FOLLOWS", "Text", {85, 1210, 760, 190}, QColor("#181619")},
        {"FORM", "Text", {85, 1040, 510, 190}, QColor("#181619")},
        {"Kicker", "Text", {90, 95, 500, 60}, QColor("#181619")},
        {"Background", "Rectangle", {0, 0, 1200, 1500}, QColor("#ff754d"), 1.0, true, true}
    };
    m_selectedIndex = 3;
    pushHistory();
}

int DocumentController::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_layers.size();
}

QVariant DocumentController::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_layers.size())
        return {};
    const Layer &layer = m_layers.at(index.row());
    switch (role) {
    case NameRole: return layer.name;
    case TypeRole: return layer.type;
    case VisibleRole: return layer.visible;
    case LockedRole: return layer.locked;
    case FillRole: return layer.fill;
    case OpacityRole: return layer.opacity;
    case SelectedRole: return index.row() == m_selectedIndex;
    default: return {};
    }
}

bool DocumentController::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_layers.size())
        return false;
    Layer &layer = m_layers[index.row()];
    if (role == VisibleRole) layer.visible = value.toBool();
    else if (role == NameRole) layer.name = value.toString();
    else return false;
    emit dataChanged(index, index, {role});
    pushHistory();
    emit documentChanged();
    return true;
}

Qt::ItemFlags DocumentController::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> DocumentController::roleNames() const
{
    return {{NameRole, "layerName"}, {TypeRole, "layerType"},
            {VisibleRole, "layerVisible"}, {LockedRole, "layerLocked"},
            {FillRole, "layerFill"}, {OpacityRole, "layerOpacity"},
            {SelectedRole, "layerSelected"}};
}

const DocumentController::Layer *DocumentController::selectedLayer() const
{
    return m_selectedIndex >= 0 && m_selectedIndex < m_layers.size()
        ? &m_layers.at(m_selectedIndex) : nullptr;
}

DocumentController::Layer *DocumentController::selectedLayer()
{
    return m_selectedIndex >= 0 && m_selectedIndex < m_layers.size()
        ? &m_layers[m_selectedIndex] : nullptr;
}

qreal DocumentController::selectionX() const { const auto *l = selectedLayer(); return l ? l->bounds.x() : 0; }
qreal DocumentController::selectionY() const { const auto *l = selectedLayer(); return l ? l->bounds.y() : 0; }
qreal DocumentController::selectionWidth() const { const auto *l = selectedLayer(); return l ? l->bounds.width() : 0; }
qreal DocumentController::selectionHeight() const { const auto *l = selectedLayer(); return l ? l->bounds.height() : 0; }
QColor DocumentController::selectionFill() const { const auto *l = selectedLayer(); return l ? l->fill : QColor("#000000"); }
qreal DocumentController::selectionOpacity() const { const auto *l = selectedLayer(); return l ? l->opacity : 1.0; }

void DocumentController::selectLayer(int index)
{
    if (index < -1 || index >= m_layers.size() || index == m_selectedIndex) return;
    const int previous = m_selectedIndex;
    m_selectedIndex = index;
    if (previous >= 0) emit dataChanged(this->index(previous), this->index(previous), {SelectedRole});
    if (index >= 0) emit dataChanged(this->index(index), this->index(index), {SelectedRole});
    emit selectionChanged();
    emit selectionGeometryChanged();
    emit selectionAppearanceChanged();
    emit documentChanged();
}

void DocumentController::toggleVisibility(int index)
{
    if (index < 0 || index >= m_layers.size()) return;
    setData(this->index(index), !m_layers[index].visible, VisibleRole);
}

void DocumentController::mutateSelected(const std::function<void(Layer &)> &mutation)
{
    Layer *layer = selectedLayer();
    if (!layer || layer->locked) return;
    mutation(*layer);
    emit dataChanged(index(m_selectedIndex), index(m_selectedIndex));
    emit selectionGeometryChanged();
    emit selectionAppearanceChanged();
    pushHistory();
    emit documentChanged();
}

void DocumentController::setSelectionX(qreal value) { mutateSelected([&](Layer &l) { l.bounds.moveLeft(value); }); }
void DocumentController::setSelectionY(qreal value) { mutateSelected([&](Layer &l) { l.bounds.moveTop(value); }); }
void DocumentController::setSelectionWidth(qreal value) { mutateSelected([&](Layer &l) { l.bounds.setWidth(std::max<qreal>(1, value)); }); }
void DocumentController::setSelectionHeight(qreal value) { mutateSelected([&](Layer &l) { l.bounds.setHeight(std::max<qreal>(1, value)); }); }
void DocumentController::setSelectionFill(const QColor &value) { if (value.isValid()) mutateSelected([&](Layer &l) { l.fill = value; }); }
void DocumentController::setSelectionOpacity(qreal value) { mutateSelected([&](Layer &l) { l.opacity = std::clamp(value, 0.0, 1.0); }); }

void DocumentController::duplicateSelected()
{
    const Layer *layer = selectedLayer();
    if (!layer) return;
    Layer copy = *layer;
    copy.name += QStringLiteral(" copy");
    copy.locked = false;
    copy.bounds.translate(28, 28);
    const int insertionIndex = m_selectedIndex;
    beginInsertRows({}, insertionIndex, insertionIndex);
    m_layers.insert(insertionIndex, copy);
    endInsertRows();
    m_selectedIndex = insertionIndex;
    emit selectionChanged();
    emit selectionGeometryChanged();
    emit selectionAppearanceChanged();
    pushHistory();
    emit documentChanged();
}

void DocumentController::removeSelected()
{
    const Layer *layer = selectedLayer();
    if (!layer || layer->locked) return;
    const int row = m_selectedIndex;
    beginRemoveRows({}, row, row);
    m_layers.removeAt(row);
    endRemoveRows();
    m_selectedIndex = m_layers.isEmpty() ? -1 : std::min(row, int(m_layers.size() - 1));
    emit selectionChanged();
    emit selectionGeometryChanged();
    emit selectionAppearanceChanged();
    pushHistory();
    emit documentChanged();
}

void DocumentController::bringSelectedToFront()
{
    if (m_selectedIndex <= 0) return;
    beginMoveRows({}, m_selectedIndex, m_selectedIndex, {}, 0);
    m_layers.move(m_selectedIndex, 0);
    endMoveRows();
    m_selectedIndex = 0;
    pushHistory();
    emit documentChanged();
}

void DocumentController::sendSelectedToBack()
{
    if (m_selectedIndex < 0 || m_selectedIndex == m_layers.size() - 1) return;
    beginMoveRows({}, m_selectedIndex, m_selectedIndex, {}, m_layers.size());
    m_layers.move(m_selectedIndex, m_layers.size() - 1);
    endMoveRows();
    m_selectedIndex = m_layers.size() - 1;
    pushHistory();
    emit documentChanged();
}

void DocumentController::pushHistory()
{
    if (m_historyIndex + 1 < m_history.size()) m_history.resize(m_historyIndex + 1);
    m_history.push_back({m_layers, m_selectedIndex});
    if (m_history.size() > 100) m_history.removeFirst();
    m_historyIndex = m_history.size() - 1;
    emit historyChanged();
}

void DocumentController::restoreSnapshot(const Snapshot &snapshot)
{
    beginResetModel();
    m_layers = snapshot.layers;
    m_selectedIndex = snapshot.selectedIndex;
    endResetModel();
    emit selectionChanged();
    emit selectionGeometryChanged();
    emit selectionAppearanceChanged();
    emit historyChanged();
    emit documentChanged();
}

void DocumentController::undo()
{
    if (!canUndo()) return;
    restoreSnapshot(m_history[--m_historyIndex]);
}

void DocumentController::redo()
{
    if (!canRedo()) return;
    restoreSnapshot(m_history[++m_historyIndex]);
}
