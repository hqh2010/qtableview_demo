#include <set>

#include <QAbstractTableModel>
#include <QMap>
#include <QPainter>
#include <QMouseEvent>
#include <QVariant>
#include <QBrush>
#include <qdrawutil.h>
#include <QDebug>
#include <QPixmap>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include "MultiLevelHeaderView.h"

struct Cell
{
    Cell(int r, int c) : row(r), column(c) {}
    int row = 0;
    int column = 0;
    bool operator<(const Cell &oth) const
    {
        if (row < oth.row)
            return true;
        if (row == oth.row)
            return column < oth.column;
        return false;
    }
};

class MultiLevelHeaderModel : public QAbstractTableModel
{
public:
    MultiLevelHeaderModel(Qt::Orientation orientation, int rows, int cols, QObject *parent = 0);
    virtual ~MultiLevelHeaderModel();

public:
    // override
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setRowHeight(int row, int size);
    int getRowHeight(int row) const;
    void setColumnWidth(int col, int size);
    int getColumnWidth(int col) const;
    void setRootCell(int row, int column, int rootRow, int rootColumn);
    bool getRootCell(int row, int column, int &rootCellRow, int &rootCellColumn) const;

private:
    Qt::Orientation m_orientation;
    int m_rowCount;
    int m_columnCount;
    std::vector<int> m_rowSizes;
    std::vector<int> m_columnSizes;
    QMap<Cell, QMap<int, QVariant>> m_data;
    QMap<QString, Cell> m_rootCellMap;
};

MultiLevelHeaderModel::MultiLevelHeaderModel(Qt::Orientation orientation, int rows, int cols, QObject *parent) : QAbstractTableModel(parent), m_orientation(orientation), m_rowCount(rows), m_columnCount(cols)
{
    m_rowSizes.resize(rows, 0);
    m_columnSizes.resize(cols, 0);
}

MultiLevelHeaderModel::~MultiLevelHeaderModel()
{
}

QModelIndex MultiLevelHeaderModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

int MultiLevelHeaderModel::rowCount(const QModelIndex &parent) const
{
    return m_rowCount;
}
int MultiLevelHeaderModel::columnCount(const QModelIndex &parent) const
{
    return m_columnCount;
}

QVariant MultiLevelHeaderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_rowCount || index.row() < 0 || index.column() >= m_columnCount || index.column() < 0)
        return QVariant();

    auto it = m_data.find(Cell(index.row(), index.column()));
    if (it != m_data.end())
    {
        auto it2 = it.value().find(role);
        if (it2 != it.value().end())
            return it2.value();
    }

    // default value
    if (role == Qt::BackgroundRole)
        // return QColor(0xcfcfcf);

        if (role == Qt::SizeHintRole)
        {
            return QSize(m_columnSizes[index.column()], m_rowSizes[index.row()]);
        }
    return QVariant();
}

bool MultiLevelHeaderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        QVariant value2;
        if (role == COLUMN_SPAN_ROLE)
        {
            int col = index.column();
            int span = value.toInt();
            if (span > 0) // span size should be more than 1, else nothing to do
            {
                if (col + span - 1 >= m_columnCount) // span size should be less than whole columns,
                    span = m_columnCount - col;
                value2 = span;
            }
        }
        else if (role == ROW_SPAN_ROLE)
        {
            int row = index.row();
            int span = value.toInt();
            if (span > 0) // span size should be more than 1, else nothing to do
            {
                if (row + span - 1 >= m_rowCount)
                    span = m_rowCount - row;
                value2 = span;
            }
        }
        else
        {
            value2 = value;
        }
        if (value.isValid())
        {
            if (role == Qt::SizeHintRole)
            {
                m_columnSizes[index.column()] = value.toSize().width();
                m_rowSizes[index.row()] = value.toSize().height();
            }
            else
            {
                for(auto it = m_data.begin(); it != m_data.end(); ++it)
                {
                    if (it.key().row == index.row() && it.key().column == index.column())
                    {
                        auto it2 = it.value().find(role);
                        if (it2 != it.value().end())
                        {
                            it.value().insert(role, value2);
                            return true;
                        }
                    }
                }
                auto &map = m_data[Cell(index.row(), index.column())];
                map.insert(role, value2);
            }
        }
        // 此处可以做一些数据置空的操作

        return true;
    }
    return false;
}

Qt::ItemFlags MultiLevelHeaderModel::flags(const QModelIndex &index) const
{
    return Qt::NoItemFlags | QAbstractTableModel::flags(index);
}

void MultiLevelHeaderModel::setRowHeight(int row, int size)
{
    if (row >= 0 && row < m_rowCount)
    {
        m_rowSizes[row] = size;
        emit dataChanged(index(row, 0), index(row, m_columnCount - 1), QVector<int>() << Qt::SizeHintRole);
    }
}

int MultiLevelHeaderModel::getRowHeight(int row) const
{
    if (row >= 0 && row < m_rowCount)
        return m_rowSizes[row];
    return 0;
}

void MultiLevelHeaderModel::setColumnWidth(int col, int size)
{
    if (col >= 0 && col < m_columnCount)
    {
        m_columnSizes[col] = size;
        emit dataChanged(index(0, col), index(m_rowCount - 1, col), QVector<int>() << Qt::SizeHintRole);
    }
}

int MultiLevelHeaderModel::getColumnWidth(int col) const
{
    if (col >= 0 && col < m_columnCount)
        return m_columnSizes[col];
    return 0;
}

void MultiLevelHeaderModel::setRootCell(int row, int column, int rootRow, int rootColumn)
{
    QString key = QString("%1,%2").arg(row).arg(column);
    Cell rc{rootRow, rootColumn};
    m_rootCellMap.insert(key, rc);
}

bool MultiLevelHeaderModel::getRootCell(int row, int column, int &rootCellRow, int &rootCellColumn) const
{
    QString key = QString("%1,%2").arg(row).arg(column);
    auto it = m_rootCellMap.find(key);
    if (it == m_rootCellMap.end())
        return false;
    rootCellRow = it.value().row;
    rootCellColumn = it.value().column;
    return true;
}

MultiLevelHeaderView::MultiLevelHeaderView(Qt::Orientation orientation, int rows, int columns, QWidget *parent) : QHeaderView(orientation, parent)
{
    // create header model
    MultiLevelHeaderModel *m = new MultiLevelHeaderModel(orientation, rows, columns);

    // set default size of item
    if (orientation == Qt::Horizontal)
    {
        for (int row = 0; row < rows; ++row)
            // modify by hqh
            if (row != 0)
            {
                m->setRowHeight(row, 32);
            }
            else
            {
                m->setRowHeight(row, 64);
            }
        for (int col = 0; col < columns; ++col)
            m->setColumnWidth(col, defaultSectionSize());
    }

    setModel(m);

    connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));

    init_tool_menu();

    init_param_menu();
}

MultiLevelHeaderView::~MultiLevelHeaderView()
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    if (m)
        delete m;
    setModel(nullptr);
}

void MultiLevelHeaderView::setRowHeight(int row, int rowHeight)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    resizeSection(row, rowHeight);
}

void MultiLevelHeaderView::setColumnWidth(int col, int colWidth)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setColumnWidth(col, colWidth);
    if (orientation() == Qt::Horizontal)
        resizeSection(col, colWidth);
}

void MultiLevelHeaderView::setCellData(int row, int column, int role, const QVariant &value)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setData(m->index(row, column), value, role);
}

void MultiLevelHeaderView::setCellSpan(int row, int column, int rowSpanCount, int columnSpanCount)
{
    Q_ASSERT(rowSpanCount > 0);
    Q_ASSERT(columnSpanCount > 0);
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    QModelIndex idx = m->index(row, column);
    m->setData(idx, rowSpanCount, ROW_SPAN_ROLE);
    m->setData(idx, columnSpanCount, COLUMN_SPAN_ROLE);
    int lastRow = row + rowSpanCount;
    int lastCol = column + columnSpanCount;
    for (int i = row; i < lastRow; ++i)
    {
        for (int j = column; j < lastCol; ++j)
        {
            m->setRootCell(i, j, row, column);
            // qDebug() << "i=" << i << "j:" << j << "row:" << row << "column:" << column;
        }
    }
}

void MultiLevelHeaderView::setCellBackgroundColor(int row, int column, const QColor &color)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setData(m->index(row, column), color, Qt::BackgroundRole);
}

void MultiLevelHeaderView::setCellForegroundColor(int row, int column, const QColor &color)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setData(m->index(row, column), color, Qt::ForegroundRole);
}

void MultiLevelHeaderView::setCellText(int row, int column, const QString &text)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setData(m->index(row, column), text, Qt::DisplayRole);
}

void MultiLevelHeaderView::setCellIcon(int row, int column, const QIcon &icon)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    m->setData(m->index(row, column), icon, Qt::DecorationRole);
}

void MultiLevelHeaderView::mousePressEvent(QMouseEvent *event)
{
    QHeaderView::mousePressEvent(event);
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    const int orient = orientation();

    // add by hqh
    if (event->button() == Qt::RightButton)
    {
        // 处理右键事件
        qInfo() << "mousePressEvent ttttttttttttttttttt row:" << index.row() << ",col:" << index.column() << index.isValid();
        int row = index.row();
        if (row == 0)
        {
            popup_tool_menu(pos);
        }
        if (row == 1)
        {
            popup_param_menu(pos);
        }
    }

    if (index.isValid())
    {
        int beginSection = -1;
        int endSection = -1;
        int numbers = 0;
        numbers = getSectionRange(index, &beginSection, &endSection);
        if (numbers > 0)
        {
            emit sectionPressed(beginSection, endSection);
            return;
        }
        else
        {
            const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
            const int levelCount = (orient == Qt::Horizontal) ? m->rowCount() : m->columnCount();
            int logicalIdx = (orient == Qt::Horizontal) ? index.column() : index.row();
            int curLevel = (orient == Qt::Horizontal) ? index.row() : index.column();
            for (int i = 0; i < levelCount; ++i)
            {
                QModelIndex cellIndex = (orient == Qt::Horizontal) ? m->index(i, logicalIdx) : m->index(logicalIdx, i);
                numbers = getSectionRange(cellIndex, &beginSection, &endSection);
                if (numbers > 0)
                {
                    if (beginSection <= logicalIdx && logicalIdx <= endSection)
                    {
                        int beginLevel = (orient == Qt::Horizontal) ? cellIndex.row() : cellIndex.column();
                        QVariant levelSpanCnt = cellIndex.data((orient == Qt::Horizontal) ? ROW_SPAN_ROLE : COLUMN_SPAN_ROLE);
                        if (!levelSpanCnt.isValid())
                            continue;
                        int endLevel = beginLevel + levelSpanCnt.toInt() - 1;
                        if (beginLevel <= curLevel && curLevel <= endLevel)
                        {
                            emit sectionPressed(beginSection, endSection);
                            break;
                        }
                    }
                }
            }
        }
    }
}

QModelIndex MultiLevelHeaderView::indexAt(const QPoint &pos) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
    const int orient = orientation();
    const int ROWS = m->rowCount();
    const int COLS = m->columnCount();
    int logicalIdx = logicalIndexAt(pos);

    if (orient == Qt::Horizontal)
    {
        int dY = 0;
        for (int row = 0; row < ROWS; ++row)
        {
            dY += m->getRowHeight(row);
            if (pos.y() <= dY)
            {
                QModelIndex cellIndex = m->index(row, logicalIdx);
                return cellIndex;
            }
        }
    }
    else
    {
        int dX = 0;
        for (int col = 0; col < COLS; ++col)
        {
            dX += m->getColumnWidth(col);
            if (pos.x() <= dX)
            {
                QModelIndex cellIndex = m->index(logicalIdx, col);
                return cellIndex;
            }
        }
    }

    return QModelIndex();
}

std::set<Cell> getCellsToBeDrawn(const MultiLevelHeaderView *view, const MultiLevelHeaderModel *m, int orient, int levelCount, int logicalIdx)
{
    std::set<Cell> cellsToBeDrawn;
    for (int i = 0; i < levelCount; ++i)
    {
        int row = i, column = logicalIdx;
        if (orient == Qt::Vertical)
            std::swap(row, column);
        int rootRow, rootCol;
        if (m->getRootCell(row, column, rootRow, rootCol))
            cellsToBeDrawn.insert({rootRow, rootCol});
    }
    return cellsToBeDrawn;
}

void MultiLevelHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIdx) const
{
#if 1
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
    const int orient = orientation();
    const int levelCount = m->rowCount();
    std::set<Cell> cellsToBeDrawn = getCellsToBeDrawn(this, m, orient, levelCount, logicalIdx);

    for (const auto &cell : cellsToBeDrawn)
    {
        QRect sectionRect = getCellRect(cell.row, cell.column);
        // sectionRect.setHeight(sectionRect.height()+20);
        qInfo() << "paintSection row:"<< cell.row << ", col:" << cell.column;
        // draw section with style
        QStyleOptionHeader sectionStyle;
        initStyleOption(&sectionStyle);
        sectionStyle.textAlignment = Qt::AlignCenter;
        sectionStyle.section = logicalIdx;
        if (cell.row == 0)
        {
            // sectionStyle.iconAlignment = Qt::AlignVCenter | Qt::AlignCenter;
            // QPixmap pixmap;
            // pixmap.load("eDevice.png");
            // qInfo() << "ttttttttttttttttttttt:" << pixmap.isNull();
            // pixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // sectionStyle.icon = QIcon("");
            // sectionStyle.icon = QIcon("eDevice.png");
            // sectionStyle.icon = QIcon(pixmap);
            // sectionStyle.iconAlignment = Qt::AlignVCenter | Qt::AlignCenter;
        }
        // add by hqh
        if (cell.row == 1)
        {
            sectionStyle.textAlignment = Qt::AlignCenter;
        }

        QModelIndex cellIndex = m->index(cell.row, cell.column);

        sectionStyle.text = cellIndex.data(Qt::DisplayRole).toString();
        sectionStyle.rect = sectionRect;

        // file background or foreground color of the cell
        //        QVariant bg = cellIndex.data(Qt::BackgroundRole);
        //        QVariant fg = cellIndex.data(Qt::ForegroundRole);

        //        if (bg.canConvert(QVariant::Brush))
        //        {
        //            sectionStyle.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(bg));
        //            sectionStyle.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(bg));
        //        }
        //        if (fg.canConvert(QVariant::Brush))
        //        {
        //            sectionStyle.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(fg));
        //        }

        painter->save();
        //        sectionStyle.rect.setHeight(sectionStyle.rect.height()+20);
        qDrawShadePanel(painter, sectionStyle.rect, sectionStyle.palette, false, 1, &sectionStyle.palette.brush(QPalette::Button));
        //        painter->drawRect(sectionStyle.rect);
        style()->drawControl(QStyle::CE_HeaderLabel, &sectionStyle, painter, this);
        //         style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);
        painter->restore();
    }

#else
    painter->save();
    painter->translate(rect.bottomLeft());
    painter->rotate(-45);
    QRect newRect(0, 0, rect.height(), rect.width());
    int dx = (rect.width() - rect.height()) / 2;
    int dy = (rect.height() - rect.width()) / 2;
    newRect.translate(dx, dy);
    painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, "aaa");
    painter->restore();
#endif
}

QSize MultiLevelHeaderView::sectionSizeFromContents(int logicalIdx) const
{
    const MultiLevelHeaderModel *m = static_cast<const MultiLevelHeaderModel *>(this->model());
    const int orient = orientation();
    const int levelCount = (orient == Qt::Horizontal) ? m->rowCount() : m->columnCount();

    int w = 0, h = 0;
    if (orient == Qt::Horizontal)
    {
        w = m->getColumnWidth(logicalIdx);
        for (int i = 0; i < levelCount; ++i)
        {
            h += m->getRowHeight(i);
        }
    }
    else
    {
        for (int i = 0; i < levelCount; ++i)
        {
            w += m->getColumnWidth(i);
        }
        h = m->getRowHeight(logicalIdx);
    }
    return QSize(w, h);
}

QModelIndex MultiLevelHeaderView::columnSpanIndex(const QModelIndex &currentIdx) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    const int curRow = currentIdx.row();
    const int curCol = currentIdx.column();
    int i = curCol;
    while (i >= 0)
    {
        QModelIndex spanIndex = m->index(curRow, i);
        QVariant span = spanIndex.data(COLUMN_SPAN_ROLE);
        if (span.isValid() && spanIndex.column() + span.toInt() - 1 >= curCol)
            return spanIndex;
        i--;
    }
    return QModelIndex();
}

QModelIndex MultiLevelHeaderView::rowSpanIndex(const QModelIndex &currentIdx) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    const int curRow = currentIdx.row();
    const int curCol = currentIdx.column();
    int i = curRow;
    while (i >= 0)
    {
        QModelIndex spanIndex = m->index(i, curCol);
        QVariant span = spanIndex.data(ROW_SPAN_ROLE);
        if (span.isValid() && spanIndex.row() + span.toInt() - 1 >= curRow)
            return spanIndex;
        i--;
    }
    return QModelIndex();
}

int MultiLevelHeaderView::columnSpanSize(int row, int from, int spanCount) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    int span = 0;
    for (int i = from; i < from + spanCount; ++i)
    {
        int colWidth = m->getColumnWidth(i);
        span += colWidth;
    }
    return span;
}

int MultiLevelHeaderView::rowSpanSize(int column, int from, int spanCount) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    int span = 0;
    for (int i = from; i < from + spanCount; ++i)
    {
        int rowHeight = m->getRowHeight(i);
        span += rowHeight;
    }
    return span;
}

bool MultiLevelHeaderView::getRootCell(int row, int column, int &rootCellRow, int &rootCellColumn) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
    return m->getRootCell(row, column, rootCellRow, rootCellColumn);
}

QRect MultiLevelHeaderView::getCellRect(int row, int column) const
{
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
    const int orient = orientation();
    QModelIndex cellIndex = m->index(row, column);
    auto colSpanVar = cellIndex.data(COLUMN_SPAN_ROLE);
    auto rowSpanVar = cellIndex.data(ROW_SPAN_ROLE);
    //    Q_ASSERT(colSpanVar.isValid() && rowSpanVar.isValid());
    int colSpan = colSpanVar.toInt();
    int rowSpan = rowSpanVar.toInt();
    int w = 0, h = 0, l = 0, t = 0;
    w = columnSpanSize(row, column, colSpan);
    h = rowSpanSize(column, row, rowSpan);
    if (orient == Qt::Horizontal)
    {
        l = sectionViewportPosition(column);
        for (int i = 0; i < row; ++i)
            t += m->getRowHeight(i);
    }
    else
    {
        for (int i = 0; i < column; ++i)
            l += m->getColumnWidth(i);
        t = sectionViewportPosition(row);
    }

    QRect rect(l, t, w, h);
    return rect;
}

/**
 * @return section numbers
 */
int MultiLevelHeaderView::getSectionRange(QModelIndex &index, int *beginSection, int *endSection) const
{
    int row = index.row(), column = index.column();
    int rootRow, rootCol;
    bool success = getRootCell(row, column, rootRow, rootCol);
    if (!success)
        return 0;
    const MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(model());
    QModelIndex rootIndex = m->index(rootRow, rootCol);
    if (orientation() == Qt::Horizontal)
    {
        int colSpanCnt = rootIndex.data(COLUMN_SPAN_ROLE).toInt();
        *beginSection = rootIndex.column();
        *endSection = *beginSection + colSpanCnt - 1;
        index = rootIndex;
        return colSpanCnt;
    }
    else
    {
        int rowSpanCnt = rootIndex.data(ROW_SPAN_ROLE).toInt();
        *beginSection = rootIndex.row();
        ;
        *endSection = *beginSection + rowSpanCnt - 1;
        index = rootIndex;
        return rowSpanCnt;
    }
}

void MultiLevelHeaderView::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    MultiLevelHeaderModel *m = static_cast<MultiLevelHeaderModel *>(this->model());
    const int orient = orientation();
    const int levelCount = (orient == Qt::Horizontal) ? m->rowCount() : m->columnCount();
    if (orient == Qt::Horizontal)
    {
        m->setColumnWidth(logicalIndex, newSize);
    }

    std::set<Cell> cellsToBeDrawn = getCellsToBeDrawn(this, m, orient, levelCount, logicalIndex);
    for (const auto &cell : cellsToBeDrawn)
    {
        QRect sectionRect = getCellRect(cell.row, cell.column);
        if (orient == Qt::Horizontal)
        {
            sectionRect.setWidth(viewport()->width() - sectionRect.left());
        }
        else
        {
            sectionRect.setHeight(viewport()->height() - sectionRect.top());
        }
        viewport()->update(sectionRect);
    }
}

void MultiLevelHeaderView::add_tool(ToolNode* tool_node)
{
    qInfo() << "MultiLevelHeaderView add_tool done";
    // tool_list.insert(tool_node->pos, tool_node);
    // qInfo() << "MultiLevelHeaderView add_tool tttttt1";

    // 刷新UI
    // setCellText(0, tool_node->pos, tool_node->name);
    // qInfo() << "MultiLevelHeaderView add_tool tttttt2";
}

void MultiLevelHeaderView::init_tool_menu()
{
    MultiLevelHeaderView* header_view = this;
    _tool_menu.addAction(
        "添加工具", [&]()
        {
            QDialog add_tool_dlg;
            QVBoxLayout *vLayout = new QVBoxLayout;

            QLineEdit* name_edit = new QLineEdit;
            QLabel* name_label = new QLabel;
            QHBoxLayout *hLayout1 = new QHBoxLayout;
            QWidget *widget1 = new QWidget;

            name_label->setText("Name:");
            name_edit->setPlaceholderText("please input tool Name");
            hLayout1->addWidget(name_label);
            hLayout1->addWidget(name_edit);
            widget1->setLayout(hLayout1);
            vLayout->addWidget(widget1);

            QLineEdit* binary_edit = new QLineEdit;
            QLabel* binary_label = new QLabel;
            QHBoxLayout *hLayout2 = new QHBoxLayout;
            QWidget *widget2 = new QWidget;
            binary_label->setText("Binary:");
            binary_edit->setPlaceholderText("please input tool binary");
            hLayout2->addWidget(binary_label);
            hLayout2->addWidget(binary_edit);
            widget2->setLayout(hLayout2);
            vLayout->addWidget(widget2);

            QLineEdit* pos_edit = new QLineEdit;
            QLabel* pos_label = new QLabel;
            QHBoxLayout *hLayout3 = new QHBoxLayout;
            QWidget *widget3 = new QWidget;
            pos_label->setText("Add Tool Pos:");
            pos_edit->setPlaceholderText("please input tool pos");
            hLayout3->addWidget(pos_label);
            hLayout3->addWidget(pos_edit);
            widget3->setLayout(hLayout3);
            vLayout->addWidget(widget3);

            QPushButton* ok_btn = new QPushButton;
            QPushButton* cancel_btn = new QPushButton;
            ok_btn->setText("OK");
            cancel_btn->setText("Cancel");
            QHBoxLayout *hLayout4 = new QHBoxLayout;
            QWidget *widget4 = new QWidget;
            hLayout4->addWidget(ok_btn);
            hLayout4->addWidget(cancel_btn);
            widget4->setLayout(hLayout4);
            vLayout->addWidget(widget4);
            connect(ok_btn, &QPushButton::clicked, [&]()
                    {
                        if (name_edit->text().trimmed().isEmpty() || pos_edit->text().trimmed().isEmpty())
                        {
                            QMessageBox msgBox;
                            msgBox.setWindowTitle(tr("警告"));
                            msgBox.setText(tr("工具名称和位置Pos不能为空"));
                            msgBox.setStandardButtons(QMessageBox::Ok);
                            msgBox.setButtonText(QMessageBox::Ok, tr("确定"));
                            msgBox.setIcon(QMessageBox::Warning);
                            msgBox.exec();
                            return;
                        }

                        qInfo() << "添加的工具为："<< name_edit->text().trimmed() << ",工具位置为：" << pos_edit->text().trimmed();
                        // ToolNode *tool_node = new ToolNode(name_edit->text().trimmed(), pos_edit->text().trimmed().toInt());
                        // 添加点工具 暂时先发送基本数据类型，后续再改为结构本
                        // header_view->add_tool(tool_node);
                        emit header_add_tool(name_edit->text().trimmed(), pos_edit->text().trimmed().toInt());

                        add_tool_dlg.close();
                    });
            connect(cancel_btn, &QPushButton::clicked, [&]()
                    {
                        add_tool_dlg.close();
                    });
            add_tool_dlg.setLayout(vLayout);

            add_tool_dlg.setWindowTitle("Add Tool");
            add_tool_dlg.setFixedSize(400, 400);
            add_tool_dlg.exec(); },
        QKeySequence(Qt::Key_B));

    _tool_menu.addAction(
        "删除工具", []()
        { qDebug() << "在这里写删除工具的代码"; },
        QKeySequence(Qt::Key_Up));

    setContextMenuPolicy(Qt::CustomContextMenu);
    // modify by hqh
    connect(this, SIGNAL(sectionClicked(int)), this, SLOT(on_section_clicked(int)));
    // connect(this, &QHeaderView::customContextMenuRequested, this, &MultiLevelHeaderView::popupMenu);
}

void MultiLevelHeaderView::init_param_menu()
{
    _param_menu.addAction(
        "添加参数", [&]()
        {
            QDialog add_param_dlg;
            QVBoxLayout *vLayout = new QVBoxLayout;

            QLineEdit* name_edit = new QLineEdit;
            QLabel* name_label = new QLabel;
            QHBoxLayout *hLayout1 = new QHBoxLayout;
            QWidget *widget1 = new QWidget;
            name_label->setText("Parameter:");
            name_edit->setPlaceholderText("please input Parameter Name");
            hLayout1->addWidget(name_label);
            hLayout1->addWidget(name_edit);
            widget1->setLayout(hLayout1);
            vLayout->addWidget(widget1);

            QLineEdit* value_edit = new QLineEdit;
            QLabel* value_label = new QLabel;
            QHBoxLayout *hLayout2 = new QHBoxLayout;
            QWidget *widget2 = new QWidget;
            value_label->setText("Parameter Value:");
            value_edit->setPlaceholderText("please input Parameter Value");
            hLayout2->addWidget(value_label);
            hLayout2->addWidget(value_edit);
            widget2->setLayout(hLayout2);
            vLayout->addWidget(widget2);

            QLineEdit* pos_edit = new QLineEdit;
            QLabel* pos_label = new QLabel;
            QHBoxLayout *hLayout3 = new QHBoxLayout;
            QWidget *widget3 = new QWidget;
            pos_label->setText("Parameter Pos:");
            pos_edit->setPlaceholderText("please input Parameter Pos");
            hLayout3->addWidget(pos_label);
            hLayout3->addWidget(pos_edit);
            widget3->setLayout(hLayout3);
            vLayout->addWidget(widget3);

            QPushButton* ok_btn = new QPushButton;
            QPushButton* cancel_btn = new QPushButton;
            ok_btn->setText("OK");
            cancel_btn->setText("Cancel");
            QHBoxLayout *hLayout4 = new QHBoxLayout;
            QWidget *widget4 = new QWidget;
            hLayout4->addWidget(ok_btn);
            hLayout4->addWidget(cancel_btn);
            widget4->setLayout(hLayout4);
            vLayout->addWidget(widget4);
            connect(ok_btn, &QPushButton::clicked, [&]()
            {
                if (name_edit->text().trimmed().isEmpty() || value_edit->text().trimmed().isEmpty())
                {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("警告"));
                    msgBox.setText(tr("输入参数不能为空"));
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setButtonText(QMessageBox::Ok, tr("确定"));
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.exec();
                    return;
                }
                int tool_col = 0;
                QModelIndex index = indexAt(mapFromGlobal( _param_menu.pos()));
                if (!index.isValid())
                {
                    qInfo() << "init_param_menu QModelIndex is not valid!";

                } else
                {
                    qInfo() << "init_param_menu QModelIndex is valid row:" << index.row() << ", col:"<<index.column();
                    tool_col = index.column();
                }

                QString param_key = name_edit->text().trimmed();
                QString value_list = value_edit->text().trimmed();
                int param_pos = pos_edit->text().trimmed().toInt();
                emit header_add_param(tool_col, param_key, value_list, param_pos);
                qInfo() << "添加的参数为："<< name_edit->text().trimmed() << ", 参数值为：" << value_edit->text().trimmed()<< ",";

                add_param_dlg.close();
            });
            connect(cancel_btn, &QPushButton::clicked, [&]()
            {
                add_param_dlg.close();
            });
            add_param_dlg.setLayout(vLayout);

            add_param_dlg.setWindowTitle("Add Parameter");
            add_param_dlg.setFixedSize(400, 400);
            add_param_dlg.exec(); },
        QKeySequence(Qt::Key_B));

    _param_menu.addAction(
        "删除参数", []()
        { qDebug() << "在这里写删除工具的代码"; },
        QKeySequence(Qt::Key_Up));

    setContextMenuPolicy(Qt::CustomContextMenu);
    // modify by hqh
    connect(this, SIGNAL(sectionClicked(int)), this, SLOT(on_section_clicked(int)));
    // connect(this, &QHeaderView::customContextMenuRequested, this, &MultiLevelHeaderView::popupMenu);
}

void MultiLevelHeaderView::on_section_clicked(int pos)
{
    qInfo() << "HeaderView sectionClicked clicked:" << pos;
}

void MultiLevelHeaderView::popup_tool_menu(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    if (!index.isValid())
    {
        qDebug() << "Current QModelIndex is not valid!";
        return;
    }

    qInfo() << "popup_tool_menu pos row" << index.row() << ",col:" << index.column() << index.isValid() << currentIndex() << currentIndex().isValid();
    _tool_menu.exec(viewport()->mapToGlobal(pos));
}

void MultiLevelHeaderView::popup_param_menu(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    if (!index.isValid())
    {
        qDebug() << "Current QModelIndex is not valid!";
        return;
    }
    QString click_pos = QString("%1,%2").arg( index.row()).arg(index.column());
    QAction* action1= _param_menu.actionAt(mapToParent(pos));
    QAction* action2= _param_menu.actionAt(mapToGlobal(pos));
    QAction* action = _param_menu.menuAction();
    // auto ttt = action->text();
    // _param_menu.activeAction()->setData(click_pos);
    _param_menu.exec(viewport()->mapToGlobal(pos));
    qInfo() << "popup_param_menu "<< _param_menu.pos() << ", global pos:" << mapToGlobal(pos) << pos;
}
