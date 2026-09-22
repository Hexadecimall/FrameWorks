#pragma once

#include <QAbstractListModel>
#include <QColor>
#include <QRectF>
#include <QVector>

#include <functional>

class DocumentController : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectionChanged)
    Q_PROPERTY(qreal selectionX READ selectionX WRITE setSelectionX NOTIFY selectionGeometryChanged)
    Q_PROPERTY(qreal selectionY READ selectionY WRITE setSelectionY NOTIFY selectionGeometryChanged)
    Q_PROPERTY(qreal selectionWidth READ selectionWidth WRITE setSelectionWidth NOTIFY selectionGeometryChanged)
    Q_PROPERTY(qreal selectionHeight READ selectionHeight WRITE setSelectionHeight NOTIFY selectionGeometryChanged)
    Q_PROPERTY(QColor selectionFill READ selectionFill WRITE setSelectionFill NOTIFY selectionAppearanceChanged)
    Q_PROPERTY(qreal selectionOpacity READ selectionOpacity WRITE setSelectionOpacity NOTIFY selectionAppearanceChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    Q_PROPERTY(bool starterPoster READ starterPoster NOTIFY documentChanged)

public:
    struct Layer {
        QString name;
        QString type;
        QRectF bounds;
        QColor fill;
        qreal opacity = 1.0;
        bool visible = true;
        bool locked = false;
    };

    enum Roles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        VisibleRole,
        LockedRole,
        FillRole,
        OpacityRole,
        SelectedRole
    };

    explicit DocumentController(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    int selectedIndex() const { return m_selectedIndex; }
    qreal selectionX() const;
    qreal selectionY() const;
    qreal selectionWidth() const;
    qreal selectionHeight() const;
    QColor selectionFill() const;
    qreal selectionOpacity() const;
    bool canUndo() const { return m_historyIndex > 0; }
    bool canRedo() const { return m_historyIndex + 1 < m_history.size(); }
    bool starterPoster() const { return m_starterPoster; }
    const QVector<Layer> &layers() const { return m_layers; }

    Q_INVOKABLE void selectLayer(int index);
    Q_INVOKABLE void toggleVisibility(int index);
    Q_INVOKABLE void duplicateSelected();
    Q_INVOKABLE void removeSelected();
    Q_INVOKABLE void bringSelectedToFront();
    Q_INVOKABLE void sendSelectedToBack();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void newDocument();
    Q_INVOKABLE void openStarterDocument();

public slots:
    void setSelectionX(qreal value);
    void setSelectionY(qreal value);
    void setSelectionWidth(qreal value);
    void setSelectionHeight(qreal value);
    void setSelectionFill(const QColor &value);
    void setSelectionOpacity(qreal value);

signals:
    void selectionChanged();
    void selectionGeometryChanged();
    void selectionAppearanceChanged();
    void historyChanged();
    void documentChanged();

private:
    struct Snapshot {
        QVector<Layer> layers;
        int selectedIndex = -1;
    };

    Layer *selectedLayer();
    const Layer *selectedLayer() const;
    void mutateSelected(const std::function<void(Layer &)> &mutation);
    void pushHistory();
    void restoreSnapshot(const Snapshot &snapshot);

    QVector<Layer> m_layers;
    QVector<Snapshot> m_history;
    int m_historyIndex = -1;
    int m_selectedIndex = -1;
    bool m_starterPoster = true;
};
