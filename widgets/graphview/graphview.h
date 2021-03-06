#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

// Widget based on x64dbg's DisassemblerGraphView
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.h
// - https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.cpp

#include <QAbstractScrollArea>
#include <QVector>
#include <QList>
#include <core/disassembler/disassemblerapi.h>
#include <core/graph/graph.h>
#include "../../../themeprovider.h"
#include "graphviewitem.h"

class GraphView : public QAbstractScrollArea
{
    Q_OBJECT

    public:
        explicit GraphView(QWidget *parent = nullptr);
        virtual void setDisassembler(const REDasm::DisassemblerPtr &disassembler);
        void setGraph(REDasm::Graphing::Graph *graph);
        void setSelectedBlock(GraphViewItem* item);
        void setFocusOnSelection(bool b);
        GraphViewItem* selectedItem() const;
        REDasm::Graphing::Graph* graph() const;

    public slots:
        void focusSelectedBlock();

    protected:
        void focusBlock(const GraphViewItem* item, bool force = false);

    protected:
        void mouseDoubleClickEvent(QMouseEvent* e) override;
        void mousePressEvent(QMouseEvent* e) override;
        void mouseReleaseEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;
        void wheelEvent(QWheelEvent* e) override;
        void resizeEvent(QResizeEvent* e) override;
        void paintEvent(QPaintEvent* e) override;
        void showEvent(QShowEvent* e) override;
        virtual void selectedItemChangedEvent();
        virtual void computeLayout();

    private:
        GraphViewItem* itemFromMouseEvent(QMouseEvent *e) const;
        void zoomOut(const QPoint& cursorpos);
        void zoomIn(const QPoint& cursorpos);
        void adjustSize(int vpw, int vph, const QPoint& cursorpos = QPoint(), bool fit = false);
        void precomputeArrow(const REDasm::Graphing::Edge& e);
        void precomputeLine(const REDasm::Graphing::Edge& e);
        bool updateSelectedItem(QMouseEvent* e);

    protected:
        REDasm::DisassemblerPtr m_disassembler;
        QHash<REDasm::Graphing::Node, GraphViewItem*> m_items;

    signals:
        void selectedItemChanged();

    private:
        GraphViewItem* m_selecteditem;
        REDasm::Graphing::Graph* m_graph;
        std::unordered_map< REDasm::Graphing::Edge, QVector<QLine> > m_lines;
        std::unordered_map<REDasm::Graphing::Edge, QPolygon> m_arrows;
        QPoint m_renderoffset, m_scrollbase;
        QSize m_rendersize;
        qreal m_scalefactor, m_scalestep, m_prevscalefactor;
        qreal m_scalemin, m_scalemax;
        int m_scaledirection, m_scaleboost;
        bool m_viewportready, m_scrollmode;

    private:
        bool m_focusonselection;
};

#endif // GRAPHVIEW_H
